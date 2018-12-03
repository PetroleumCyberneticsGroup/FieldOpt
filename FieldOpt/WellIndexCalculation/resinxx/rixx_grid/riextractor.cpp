////////////////////////////////////////////////////////////////////
//
// Copyright (C) Statoil ASA
// Copyright (C) Ceetron Solutions AS
//
// ResInsight is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// ResInsight is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// See the GNU General Public License at
// <http://www.gnu.org/licenses/gpl.html>
// for more details.
//
////////////////////////////////////////////////////////////////////
//
// Modified by M.Bellout on 3/9/18.
//

// -----------------------------------------------------------------
#include "riextractor.h"

// ╦═╗  ╦  ╔═╗  ═╗ ╦  ╔╦╗  ╦═╗  ╔═╗  ╔═╗  ╔╦╗  ╔═╗  ╦═╗
// ╠╦╝  ║  ║╣   ╔╩╦╝   ║   ╠╦╝  ╠═╣  ║     ║   ║ ║  ╠╦╝
// ╩╚═  ╩  ╚═╝  ╩ ╚═   ╩   ╩╚═  ╩ ╩  ╚═╝   ╩   ╚═╝  ╩╚═
// =================================================================
RIExtractor::RIExtractor(const RICaseData* aCase,
                         const Reservoir::WellIndexCalculation::WellPath& wellpath)
    : m_caseData(aCase), m_wellPath(&wellpath) { }

// -----------------------------------------------------------------
RIExtractor::~RIExtractor() { }

// -----------------------------------------------------------------
vector<WellPathCellIntersectionInfo>
RIExtractor::cellIntersectionInfosAlongWellPath() const {

  vector<WellPathCellIntersectionInfo> infoVector;
  if (m_intersectedCellsGlobIdx.empty()) return infoVector;

  for (size_t i = 0; i < m_intersectedCellsGlobIdx.size() - 1; i=i+2) {

    CVF_ASSERT(m_intersectedCellsGlobIdx[i] ==
        m_intersectedCellsGlobIdx[i + 1]);

    WellPathCellIntersectionInfo cellInfo;

    cellInfo.globCellIndex = m_intersectedCellsGlobIdx[i];
    cellInfo.startPoint = m_intersections[i];
    cellInfo.endPoint = m_intersections[i+1];
    cellInfo.startMD = m_measuredDepth[i];
    cellInfo.endMD = m_measuredDepth[i+1];

    cellInfo.intersectedCellFaceIn = m_intersectedCellFaces[i];
    cellInfo.intersectedCellFaceOut = m_intersectedCellFaces[i+1];

    cellInfo.intersectionLengthsInCellCS =
        this->calculateLengthInCell(cellInfo.globCellIndex,
                                    cellInfo.startPoint,
                                    cellInfo.endPoint);

    cellInfo.dxdydz =
        m_caseData.p()->mainGrid()->cell(cellInfo.globCellIndex).dxdydz();

    infoVector.push_back(cellInfo);
  }

  return infoVector;
}

//--------------------------------------------------------------------
const vector<size_t>&
RIExtractor::intersectedCellsGlobIdx() {
  return m_intersectedCellsGlobIdx;
}

//--------------------------------------------------------------------
void
RIExtractor::insertIntersectionsInMap(
    const vector<cvf::HexIntersectionInfo> &intersections,
    cvf::Vec3d p1, double md1,
    cvf::Vec3d p2, double md2,
    map<RIMDCellIdxEnterLeaveKey, cvf::HexIntersectionInfo > *uniqueIntersections) {

  for (size_t intIdx = 0; intIdx < intersections.size(); ++intIdx) {

    double lenghtAlongLineSegment1 =
        (intersections[intIdx].m_intersectionPoint - p1).length();

    double lenghtAlongLineSegment2 =
        (p2 - intersections[intIdx].m_intersectionPoint).length();

    double measuredDepthDiff = md2 - md1;
    double lineLength = lenghtAlongLineSegment1 + lenghtAlongLineSegment2;
    double measuredDepthOfPoint = 0.0;

    if (lineLength > 0.00001) {
      measuredDepthOfPoint =
          md1 + measuredDepthDiff*lenghtAlongLineSegment1/(lineLength);
    }
    else {
      measuredDepthOfPoint = md1;
    }

    uniqueIntersections->
        insert(std::make_pair(
        RIMDCellIdxEnterLeaveKey(
            measuredDepthOfPoint,
            intersections[intIdx].m_hexIndex,
            intersections[intIdx].m_isIntersectionEntering),
        intersections[intIdx]));
  }
}


//--------------------------------------------------------------------
void RIExtractor::populateReturnArrays(
    map<RIMDCellIdxEnterLeaveKey, cvf::HexIntersectionInfo > &uniqueIntersections) {

  // For same MD and same cell, remove enter/leave pairs, as
  // they only touch the wellpath, and should not contribute.
  {
    map<RIMDCellIdxEnterLeaveKey, cvf::HexIntersectionInfo >::iterator
        it1 = uniqueIntersections.begin();

    map<RIMDCellIdxEnterLeaveKey, cvf::HexIntersectionInfo >::iterator
        it2 = uniqueIntersections.begin();

    vector<map<RIMDCellIdxEnterLeaveKey, cvf::HexIntersectionInfo >::iterator>
        iteratorsToIntersectonsToErase;

    while (it2 != uniqueIntersections.end()) {
      ++it2;

      if (it2 != uniqueIntersections.end()) {

        if (RigWellLogExtractionTools::isEqualDepth(it1->first.measuredDepth,
                                                    it2->first.measuredDepth)) {

          if (it1->first.hexIndex == it2->first.hexIndex) {

            // Remove the two from the map, as they just are a touch of the cell surface
            CVF_TIGHT_ASSERT(!it1->first.isEnteringCell && it2->first.isEnteringCell);

            iteratorsToIntersectonsToErase.push_back(it1);
            iteratorsToIntersectonsToErase.push_back(it2);
          }
        }
      }
      ++it1;
    }

    // Erase all the intersections that are not needed
    for (size_t erItIdx = 0; erItIdx < iteratorsToIntersectonsToErase.size(); ++erItIdx) {
      uniqueIntersections.erase(iteratorsToIntersectonsToErase[erItIdx]);
    }
  }

  // Copy the map into a different sorting regime,
  // with enter leave more significant than cell index
  map<RIMDEnterLeaveCellIdxKey, cvf::HexIntersectionInfo > sortedUniqueIntersections;
  {
    map<RIMDCellIdxEnterLeaveKey, cvf::HexIntersectionInfo >::iterator
        it = uniqueIntersections.begin();

    while (it != uniqueIntersections.end()) {

      sortedUniqueIntersections.insert(
          make_pair(RIMDEnterLeaveCellIdxKey(
              it->first.measuredDepth,
              it->first.isEnteringCell,
              it->first.hexIndex),
                    it->second));

      ++it;
    }
  }

  // Add points for the endpoint of the wellpath, if it starts/ends inside a cell
  {
    // Add an intersection for the well startpoint that is inside the first cell
    map<RIMDEnterLeaveCellIdxKey, cvf::HexIntersectionInfo >::iterator
        it = sortedUniqueIntersections.begin();

    // Leaving a cell as first intersection. Well starts inside a cell.
    if (it != sortedUniqueIntersections.end() && !it->first.isEnteringCell) {

      // Needs wellpath start point in front
      cvf::HexIntersectionInfo firstLeavingPoint = it->second;
      firstLeavingPoint.m_intersectionPoint =  m_wellPath->m_wellPathPoints[0];
      firstLeavingPoint.m_face = cvf::StructGridInterface::NO_FACE;
      firstLeavingPoint.m_isIntersectionEntering = true;

      sortedUniqueIntersections.
          insert(
          make_pair(RIMDEnterLeaveCellIdxKey(m_wellPath->m_measuredDepths[0],
                                                   true, firstLeavingPoint.m_hexIndex),
                         firstLeavingPoint));
    }

    // Add an intersection for the well endpoint possibly inside the last cell.
    map<RIMDEnterLeaveCellIdxKey, cvf::HexIntersectionInfo >::reverse_iterator
        rit = sortedUniqueIntersections.rbegin();

    // Entering a cell as last intersection. Well ends inside a cell.
    if (rit != sortedUniqueIntersections.rend() && rit->first.isEnteringCell) {

      // Needs wellpath end point at end
      cvf::HexIntersectionInfo lastEnterPoint = rit->second;
      lastEnterPoint.m_intersectionPoint =  m_wellPath->m_wellPathPoints.back();
      lastEnterPoint.m_isIntersectionEntering = false;
      lastEnterPoint.m_face = cvf::StructGridInterface::NO_FACE;

      sortedUniqueIntersections.
          insert(
          make_pair(RIMDEnterLeaveCellIdxKey(m_wellPath->m_measuredDepths.back(),
                                                   false, lastEnterPoint.m_hexIndex),
                         lastEnterPoint));
    }
  }

  // Filter and store the intersections pairwise as cell enter-leave pairs
  // Discard points that does not have a match .
  {
    map<RIMDEnterLeaveCellIdxKey, cvf::HexIntersectionInfo >::iterator
        it1 = sortedUniqueIntersections.begin();
    map<RIMDEnterLeaveCellIdxKey, cvf::HexIntersectionInfo >::iterator
        it2;

    while (it1 != sortedUniqueIntersections.end()) {
      it2 = it1;
      ++it2;

      if (it2 == sortedUniqueIntersections.end()) break;

      if (RIMDEnterLeaveCellIdxKey::isProperCellEnterLeavePair(it1->first, it2->first)) {
        appendIntersectionToArrays(it1->first.measuredDepth, it1->second);
        ++it1;
        appendIntersectionToArrays(it1->first.measuredDepth, it1->second);
        ++it1;
      }
      else {

        // If we haven't a proper pair, try our best to recover these variants:
        // 1-2 3 4 5 6 7 8 9 10 11-12
        //     +---+
        //       +---+
        //             +---+

        map<RIMDEnterLeaveCellIdxKey, cvf::HexIntersectionInfo >::iterator
            it11 = it1;
        map<RIMDEnterLeaveCellIdxKey, cvf::HexIntersectionInfo >::iterator
            it21 = it2;

        // Check if we have overlapping cells (typically at a fault)
        ++it21;
        if (it21 != sortedUniqueIntersections.end()
            && RIMDEnterLeaveCellIdxKey::isProperCellEnterLeavePair(it11->first, it21->first)) {

          // Found 3 to 5 connection
          appendIntersectionToArrays(it11->first.measuredDepth,
                                     it11->second);
          appendIntersectionToArrays(it21->first.measuredDepth,
                                     it21->second);

          ++it11; ++it21;
          if (it21 != sortedUniqueIntersections.end()
              &&  RIMDEnterLeaveCellIdxKey::isProperCellEnterLeavePair(it11->first,
                                                                        it21->first)) {
            // Found a 4 to 6 connection
            appendIntersectionToArrays(it11->first.measuredDepth, it11->second);
            appendIntersectionToArrays(it21->first.measuredDepth, it21->second);

            it1 = it21;
            ++it1;
            continue;
          }
          else {
//            RiaLogging::warning(QString("Well Log Extraction : ")
//                                    + QString::fromStdString(m_wellCaseErrorMsgName)
//                                    + (" Discards a point at MD:  ")
//                                    + QString::number((double)(it1->first.measuredDepth)));

            // Found that 8 to 10 is not connected, after finding 7 to 9
            it1 = it21; // Discard 8 by Jumping to 10
            continue;
          }
        }
        else {
//          RiaLogging::warning(QString("Well Log Extraction : ")
//                                  + QString::fromStdString(m_wellCaseErrorMsgName)
//                                  + (" Discards a point at MD:  ")
//                                  + QString::number((double)(it1->first.measuredDepth)));

          // Found that 10 to 11 is not connected, and not 10 to 12 either
          ++it1; // Discard 10 and jump to 11 and hope that recovers us
          continue;
        }

        CVF_ASSERT(false); // Should never end here
      }
    }
  }
}

//--------------------------------------------------------------------
void RIExtractor::appendIntersectionToArrays(double measuredDepth,
                                             const cvf::HexIntersectionInfo& intersection) {

  m_measuredDepth.push_back(measuredDepth);

  m_trueVerticalDepth.push_back(fabs(intersection.m_intersectionPoint[2]));

  m_intersections.push_back(intersection.m_intersectionPoint);

  m_intersectedCellsGlobIdx.push_back(intersection.m_hexIndex);

  m_intersectedCellFaces.push_back(intersection.m_face);
}

////////////////////////////////////////////////////////////////////
//
// Copyright (C) Statoil ASA
// Copyright (C) Ceetron Solutions AS
//
// ResInsight is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// ResInsight is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// See the GNU General Public License at
// <http://www.gnu.org/licenses/gpl.html>
// for more details.
//
////////////////////////////////////////////////////////////////////
//
// Modified by M.Bellout on 3/9/18.
//

// ╦═╗  ╦  ╔═╗  ╔═╗  ╦    ╔═╗  ═╗ ╦  ╔╦╗  ╦═╗  ╔═╗  ╔═╗  ╔╦╗  ╔═╗  ╦═╗
// ╠╦╝  ║  ║╣   ║    ║    ║╣   ╔╩╦╝   ║   ╠╦╝  ╠═╣  ║     ║   ║ ║  ╠╦╝
// ╩╚═  ╩  ╚═╝  ╚═╝  ╩═╝  ╚═╝  ╩ ╚═   ╩   ╩╚═  ╩ ╩  ╚═╝   ╩   ╚═╝  ╩╚═
//==================================================================
RIECLExtractor::RIECLExtractor(const RICaseData* aCase,
                               const Reservoir::WellIndexCalculation::WellPath& wellpath)
    : m_caseData(aCase), RIExtractor(aCase, wellpath) {
  calculateIntersection();
}

// -----------------------------------------------------------------
void RIECLExtractor::calculateIntersection() {

  map<RIMDCellIdxEnterLeaveKey, cvf::HexIntersectionInfo > uniqueIntersections;

  const vector<cvf::Vec3d>& nodeCoords = m_caseData->mainGrid()->nodes();
  bool isCellFaceNormalsOut = m_caseData->mainGrid()->isFaceNormalsOutwards();

  if (!m_wellPath->m_wellPathPoints.size()) return ;

  for (size_t wpp = 0; wpp < m_wellPath->m_wellPathPoints.size() - 1; ++wpp) {

    vector<cvf::HexIntersectionInfo> intersections;
    cvf::Vec3d p1 = m_wellPath->m_wellPathPoints[wpp];
    cvf::Vec3d p2 = m_wellPath->m_wellPathPoints[wpp+1];

    // Add coords to bbox
    cvf::BoundingBox bb;
    bb.add(p1);
    bb.add(p2);

    // Find cells close to bbox
    vector<size_t> closeCells = findCloseCells(bb);

    // Loop through cell neighborhood
    cvf::Vec3d hexCorners[8];
    for (size_t cIdx = 0; cIdx < closeCells.size(); ++cIdx) {

      // Get current cell
      const RICell& cell =
          m_caseData->mainGrid()->globalCellArray()[closeCells[cIdx]];

      if (cell.isInvalid()) continue;

      // Get corner vertices of current cell
      const caf::SizeTArray8& cornerIndices = cell.cornerIndices();

      hexCorners[0] = nodeCoords[cornerIndices[0]];
      hexCorners[1] = nodeCoords[cornerIndices[1]];
      hexCorners[2] = nodeCoords[cornerIndices[2]];
      hexCorners[3] = nodeCoords[cornerIndices[3]];
      hexCorners[4] = nodeCoords[cornerIndices[4]];
      hexCorners[5] = nodeCoords[cornerIndices[5]];
      hexCorners[6] = nodeCoords[cornerIndices[6]];
      hexCorners[7] = nodeCoords[cornerIndices[7]];

      cvf::RigHexIntersectionTools::lineHexCellIntersection(
          p1, p2, hexCorners, closeCells[cIdx], &intersections);

    } // End: for (size_t cIdx = 0; cIdx < closeCells.size(); ++cIdx)

    if (!isCellFaceNormalsOut) {
      for (size_t intIdx = 0; intIdx < intersections.size(); ++intIdx) {
        intersections[intIdx].m_isIntersectionEntering =
            !intersections[intIdx].m_isIntersectionEntering ;
      }
    }

    // Now, with all the intersections of this piece of line, we need to sort
    // them in order, and set the measured depth and corresponding cell index

    // Inserting the intersections in this map will remove identical
    // intersections and sort them according to MD, CellIdx, Leave/enter

    double md1 = m_wellPath->m_measuredDepths[wpp];
    double md2 = m_wellPath->m_measuredDepths[wpp+1];

    insertIntersectionsInMap(intersections,
                             p1, md1, p2, md2,
                             &uniqueIntersections);

  }

  this->populateReturnArrays(uniqueIntersections);
}

// -----------------------------------------------------------------
///
// -----------------------------------------------------------------
//void RIECLExtractor::curveData(const RigResultAccessor* resultAccessor,
//                                           vector<double>* values) {
//
//  CVF_TIGHT_ASSERT(values);
//  values->resize(m_intersections.size());
//
//  for (size_t cpIdx = 0; cpIdx < m_intersections.size(); ++cpIdx) {
//    size_t cellIdx = m_intersectedCellsGlobIdx[cpIdx];
//    cvf::StructGridInterface::FaceType cellFace = m_intersectedCellFaces[cpIdx];
//
//    (*values)[cpIdx] = resultAccessor->cellFaceScalarGlobIdx(cellIdx, cellFace);
//  }
//
//}


// -----------------------------------------------------------------
///
// -----------------------------------------------------------------
vector<size_t> RIECLExtractor::findCloseCells(const cvf::BoundingBox& bb) {

  vector<size_t> closeCells;
  m_caseData->mainGrid()->findIntersectingCells(bb, &closeCells);
  return closeCells;
}

// -----------------------------------------------------------------
///
// -----------------------------------------------------------------
cvf::Vec3d RIECLExtractor::calculateLengthInCell(size_t cellIndex,
                                                 const cvf::Vec3d& startPoint,
                                                 const cvf::Vec3d& endPoint) const {

  array<cvf::Vec3d, 8> hexCorners;
  m_caseData->mainGrid()->cellCornerVertices(cellIndex,
                                             hexCorners.data());

  return WellPath::calculateLengthInCell(hexCorners,
                                         startPoint,
                                         endPoint);
}
