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
// Modified by M.Bellout on 3/6/18.
//

// -----------------------------------------------------------------
#include "rifaultncc.h"
#include "rigrid.h"
#include "../geometry_tools.h"

// ╦═╗  ╦  ╔═╗  ╔═╗  ╔╗╔  ╔╗╔  ╔═╗  ╔═╗  ╔╦╗  ╦  ╔═╗  ╔╗╔
// ╠╦╝  ║  ║    ║ ║  ║║║  ║║║  ║╣   ║     ║   ║  ║ ║  ║║║
// ╩╚═  ╩  ╚═╝  ╚═╝  ╝╚╝  ╝╚╝  ╚═╝  ╚═╝   ╩   ╩  ╚═╝  ╝╚╝
// =================================================================


// ╦═╗  ╦  ╔╗╔  ╔╗╔  ╔═╗  ╔╦╗  ╔═╗  ╔╦╗  ╔═╗
// ╠╦╝  ║  ║║║  ║║║  ║     ║║  ╠═╣   ║   ╠═╣
// ╩╚═  ╩  ╝╚╝  ╝╚╝  ╚═╝  ═╩╝  ╩ ╩   ╩   ╩ ╩
// =================================================================
RINNCData::RINNCData() {
}

// -----------------------------------------------------------------
void RINNCData::processConnections(const RIGrid& mainGrid) {

  // ---------------------------------------------------------------
  //cvf::Trace::show("NNC: Total number: " + cvf::String((int)m_connections.size()));
  cout << "NNC: Total number: "
       << cvf::String((int)m_connections.size()).toStdString() << endl;

  // ---------------------------------------------------------------
  for (size_t cnIdx = 0; cnIdx < m_connections.size(); ++cnIdx) {

    // -------------------------------------------------------------
    const RICell& c1 = mainGrid.globalCellArray()[m_connections[cnIdx].m_c1GlobIdx];
    const RICell& c2 = mainGrid.globalCellArray()[m_connections[cnIdx].m_c2GlobIdx];

    // -------------------------------------------------------------
    bool foundAnyOverlap = false;
    std::vector<size_t> connectionPolygon;
    std::vector<cvf::Vec3d> connectionIntersections;
    cvf::StructGridInterface::FaceType connectionFace = cvf::StructGridInterface::NO_FACE;

    connectionFace = calculateCellFaceOverlap(c1, c2, mainGrid,
                                              &connectionPolygon,
                                              &connectionIntersections);

    if (connectionFace != cvf::StructGridInterface::NO_FACE)
    {
      foundAnyOverlap = true;
      // Found an overlap polygon. Store data about connection

      m_connections[cnIdx].m_c1Face = connectionFace;
      for (size_t pIdx = 0; pIdx < connectionPolygon.size(); ++pIdx)
      {
        if (connectionPolygon[pIdx] < mainGrid.nodes().size())
          m_connections[cnIdx].m_polygon.push_back(mainGrid.nodes()[connectionPolygon[pIdx]]);
        else
          m_connections[cnIdx].m_polygon.push_back(connectionIntersections[connectionPolygon[pIdx] - mainGrid.nodes().size()]);
      }

      // Add to search map, possibly not needed
      //m_cellIdxToFaceToConnectionIdxMap[m_connections[cnIdx].m_c1GlobIdx][connectionFace].push_back(cnIdx);
      //m_cellIdxToFaceToConnectionIdxMap[m_connections[cnIdx].m_c2GlobIdx][cvf::StructGridInterface::oppositeFace(connectionFace].push_back(cnIdx);
    }
    else
    {
      //cvf::Trace::show("NNC: No overlap found for : C1: " + cvf::String((int)m_connections[cnIdx].m_c1GlobIdx) + "C2: " + cvf::String((int)m_connections[cnIdx].m_c2GlobIdx));
    }
  }
}

// -----------------------------------------------------------------
cvf::StructGridInterface::FaceType
RINNCData::calculateCellFaceOverlap(const RICell &c1,
                                     const RICell &c2,
                                     const RIGrid &mainGrid,
                                     std::vector<size_t>* connectionPolygon,
                                     std::vector<cvf::Vec3d>* connectionIntersections) {

  // Try to find the shared face

  bool isPossibleNeighborInDirection[6]={ true, true, true, true, true, true };

  if ( c1.hostGrid() == c2.hostGrid() )
  {
    char hasNeighbourInAnyDirection = 0;

    size_t i1, j1, k1;
    c1.hostGrid()->ijkFromCellIndex(c1.gridLocalCellIndex(), &i1, &j1, &k1);
    size_t i2, j2, k2;
    c2.hostGrid()->ijkFromCellIndex(c2.gridLocalCellIndex(), &i2, &j2, &k2);


    isPossibleNeighborInDirection[cvf::StructGridInterface::POS_I] = ((i1 + 1) == i2);
    isPossibleNeighborInDirection[cvf::StructGridInterface::NEG_I] = ((i2 + 1) == i1);
    isPossibleNeighborInDirection[cvf::StructGridInterface::POS_J] = ((j1 + 1) == j2);
    isPossibleNeighborInDirection[cvf::StructGridInterface::NEG_J] = ((j2 + 1) == j1);
    isPossibleNeighborInDirection[cvf::StructGridInterface::POS_K] = ((k1 + 1) == k2);
    isPossibleNeighborInDirection[cvf::StructGridInterface::NEG_K] = ((k2 + 1) == k1);

    hasNeighbourInAnyDirection =
        isPossibleNeighborInDirection[cvf::StructGridInterface::POS_I]
            + isPossibleNeighborInDirection[cvf::StructGridInterface::NEG_I]
            + isPossibleNeighborInDirection[cvf::StructGridInterface::POS_J]
            + isPossibleNeighborInDirection[cvf::StructGridInterface::NEG_J]
            + isPossibleNeighborInDirection[cvf::StructGridInterface::POS_K]
            + isPossibleNeighborInDirection[cvf::StructGridInterface::NEG_K];


    // If cell 2 is not adjancent with respect to any of the six ijk directions,
    // assume that we have no overlapping area.

    if ( !hasNeighbourInAnyDirection )
    {
      // Add to search map
      //m_cellIdxToFaceToConnectionIdxMap[m_connections[cnIdx].m_c1GlobIdx][cvf::StructGridInterface::NO_FACE].push_back(cnIdx);
      //m_cellIdxToFaceToConnectionIdxMap[m_connections[cnIdx].m_c2GlobIdx][cvf::StructGridInterface::NO_FACE].push_back(cnIdx);

      //cvf::Trace::show("NNC: No direct neighbors : C1: " + cvf::String((int)m_connections[cnIdx].m_c1GlobIdx) + " C2: " + cvf::String((int)m_connections[cnIdx].m_c2GlobIdx));
      return cvf::StructGridInterface::NO_FACE;
    }
  }

#if 0
  // Possibly do some testing to avoid unneccesary overlap calculations
    cvf::Vec3d normal;
    for ( char fIdx = 0; fIdx < 6; ++fIdx )
    {
        if ( isPossibleNeighborInDirection[fIdx] )
        {
            cvf::Vec3d fc1 = c1.faceCenter((cvf::StructGridInterface::FaceType)(fIdx));
            cvf::Vec3d fc2 = c2.faceCenter(cvf::StructGridInterface::oppositeFace((cvf::StructGridInterface::FaceType)(fIdx)));
            cvf::Vec3d fc1ToFc2 = fc2 - fc1;
            normal = c1.faceNormalWithAreaLenght((cvf::StructGridInterface::FaceType)(fIdx));
            normal.normalize();
            // Check that face centers are approx in the face plane
            if ( normal.dot(fc1ToFc2) < 0.01*fc1ToFc2.length() )
            {

            }
        }
    }
#endif

  for ( char fIdx = 0; fIdx < 6; ++fIdx ) {

    if ( !isPossibleNeighborInDirection[fIdx] ) {
      continue;
    }

    // Calculate connection polygon

    std::vector<size_t> polygon;
    std::vector<cvf::Vec3d> intersections;
    caf::SizeTArray4 face1;
    caf::SizeTArray4 face2;
    c1.faceIndices((cvf::StructGridInterface::FaceType)(fIdx), &face1);
    c2.faceIndices(cvf::StructGridInterface::oppositeFace((cvf::StructGridInterface::FaceType)(fIdx)), &face2);

    bool foundOverlap = cvf::GeometryTools::calculateOverlapPolygonOfTwoQuads(
        &polygon,
        &intersections,
        (cvf::EdgeIntersectStorage<size_t>*)NULL,
        cvf::wrapArrayConst(&mainGrid.nodes()),
        face1.data(),
        face2.data(),
        1e-6);

    if ( foundOverlap )
    {
      if (connectionPolygon)(*connectionPolygon) = polygon;
      if (connectionIntersections) (*connectionIntersections) = intersections;
      return (cvf::StructGridInterface::FaceType)(fIdx);
    }
  }

  return cvf::StructGridInterface::NO_FACE;
}

// -----------------------------------------------------------------
vector<double>&
RINNCData::makeStaticConnectionScalarResult(QString nncDataType) {
  std::vector< std::vector<double> >& results = m_connectionResults[nncDataType];
  results.resize(1);
  results[0].resize(m_connections.size(), HUGE_VAL);
  return results[0];
}

// -----------------------------------------------------------------
const vector<double>*
RINNCData::staticConnectionScalarResult(size_t scalarResultIndex) const
{
  QString nncDataType = getNNCDataTypeFromScalarResultIndex(scalarResultIndex);
  if (nncDataType.isNull()) return nullptr;

  std::map<QString, std::vector< std::vector<double> > >::const_iterator it = m_connectionResults.find(nncDataType);

  if (it != m_connectionResults.end())
  {
    CVF_ASSERT(it->second.size() == 1);
    return &(it->second[0]);
  }
  else
  {
    return nullptr;
  }
}

// -----------------------------------------------------------------
const vector<double>*
RINNCData::staticConnectionScalarResultByName(const QString& nncDataType) const {

  std::map<QString, std::vector< std::vector<double> > >::const_iterator
      it = m_connectionResults.find(nncDataType);

  if (it != m_connectionResults.end()) {

    CVF_ASSERT(it->second.size() == 1);
    return &(it->second[0]);
  } else {

    return nullptr;
  }
}

// -----------------------------------------------------------------
std::vector< std::vector<double> >&
RINNCData::makeDynamicConnectionScalarResult(QString nncDataType,
                                              size_t timeStepCount)
{
  auto& results = m_connectionResults[nncDataType];
  results.resize(timeStepCount);
  return results;
}

// -----------------------------------------------------------------
const std::vector< std::vector<double> >*
RINNCData::dynamicConnectionScalarResult(size_t scalarResultIndex) const {

  QString nncDataType = getNNCDataTypeFromScalarResultIndex(scalarResultIndex);
  if (nncDataType.isNull()) return nullptr;

  auto it = m_connectionResults.find(nncDataType);

  if (it != m_connectionResults.end()) {
    return &(it->second);
  } else {
    return nullptr;
  }
}


// -----------------------------------------------------------------
const vector<double>*
RINNCData::dynamicConnectionScalarResult(size_t scalarResultIndex,
                                          size_t timeStep) const {

  QString nncDataType = getNNCDataTypeFromScalarResultIndex(scalarResultIndex);
  if (nncDataType.isNull()) return nullptr;

  auto it = m_connectionResults.find(nncDataType);

  if (it != m_connectionResults.end())
  {
    if (it->second.size() > timeStep)
    {
      return &(it->second[timeStep]);
    }
  }
  return nullptr;
}

// -----------------------------------------------------------------
const vector<vector<double>>*
RINNCData::dynamicConnectionScalarResultByName(
    const QString& nncDataType) const {

  auto it = m_connectionResults.find(nncDataType);
  if (it != m_connectionResults.end()) {
    return &(it->second);
  }
  return nullptr;
}

// -----------------------------------------------------------------
const vector<double>*
RINNCData::dynamicConnectionScalarResultByName(const QString& nncDataType,
                                                size_t timeStep) const {

  auto it = m_connectionResults.find(nncDataType);
  if (it != m_connectionResults.end()) {

    if (it->second.size() > timeStep) {
      return &(it->second[timeStep]);
    }
  }
  return nullptr;
}

// -----------------------------------------------------------------
vector< vector<double> >&
RINNCData::makeGeneratedConnectionScalarResult(QString nncDataType,
                                                size_t timeStepCount) {
  auto& results = m_connectionResults[nncDataType];
  results.resize(timeStepCount);
  return results;
}

// -----------------------------------------------------------------
const vector< vector<double> >*
RINNCData::generatedConnectionScalarResult(size_t scalarResultIndex) const {

  QString nncDataType = getNNCDataTypeFromScalarResultIndex(scalarResultIndex);
  if (nncDataType.isNull()) return nullptr;

  auto it = m_connectionResults.find(nncDataType);

  if (it != m_connectionResults.end()) {
    return &(it->second);
  }
  else {
    return nullptr;
  }
}

// -----------------------------------------------------------------
const vector<double>*
RINNCData::generatedConnectionScalarResult(size_t scalarResultIndex,
                                            size_t timeStep) const {
  QString nncDataType = getNNCDataTypeFromScalarResultIndex(scalarResultIndex);
  if (nncDataType.isNull()) return nullptr;

  auto it = m_connectionResults.find(nncDataType);

  if (it != m_connectionResults.end()) {

    if (it->second.size() > timeStep) {
      return &(it->second[timeStep]);
    }
  }
  return nullptr;
}

// -----------------------------------------------------------------
vector< vector<double> >*
RINNCData::generatedConnectionScalarResult(size_t scalarResultIndex) {

  QString nncDataType = getNNCDataTypeFromScalarResultIndex(scalarResultIndex);
  if (nncDataType.isNull()) return nullptr;

  auto it = m_connectionResults.find(nncDataType);

  if (it != m_connectionResults.end()) {
    return &(it->second);
  } else {
    return nullptr;
  }
}

// -----------------------------------------------------------------
vector<double>*
RINNCData::generatedConnectionScalarResult(size_t scalarResultIndex,
                                            size_t timeStep) {

  QString nncDataType = getNNCDataTypeFromScalarResultIndex(scalarResultIndex);
  if (nncDataType.isNull()) return nullptr;

  auto it = m_connectionResults.find(nncDataType);

  if (it != m_connectionResults.end()) {
    if (it->second.size() > timeStep) {
      return &(it->second[timeStep]);
    }
  }
  return nullptr;
}

// -----------------------------------------------------------------
const vector<vector<double>>*
RINNCData::generatedConnectionScalarResultByName(const QString& nncDataType) const {

  auto it = m_connectionResults.find(nncDataType);
  if (it != m_connectionResults.end()) {
    return &(it->second);
  }
  return nullptr;
}

// -----------------------------------------------------------------
const vector<double>*
RINNCData::generatedConnectionScalarResultByName(const QString& nncDataType,
                                                  size_t timeStep) const {

  auto it = m_connectionResults.find(nncDataType);
  if (it != m_connectionResults.end()) {

    if (it->second.size() > timeStep) {
      return &(it->second[timeStep]);
    }
  }
  return nullptr;
}

// -----------------------------------------------------------------
vector<vector<double>>*
RINNCData::generatedConnectionScalarResultByName(const QString& nncDataType) {

  auto it = m_connectionResults.find(nncDataType);
  if (it != m_connectionResults.end()) {
    return &(it->second);
  }
  return nullptr;
}

// -----------------------------------------------------------------
vector<double>*
RINNCData::generatedConnectionScalarResultByName(const QString& nncDataType,
                                                  size_t timeStep) {

  auto it = m_connectionResults.find(nncDataType);
  if (it != m_connectionResults.end()) {
    if (it->second.size() > timeStep) {
      return &(it->second[timeStep]);
    }
  }
  return nullptr;
}

// -----------------------------------------------------------------
vector<QString>
RINNCData::availableProperties(NNCResultType resultType) const {

  vector<QString> properties;

  for (auto it : m_connectionResults) {

    if (resultType == NNC_STATIC && it.second.size() == 1
        && it.second[0].size() > 0 && isNative(it.first)) {

      properties.push_back(it.first);
    }
    else if (resultType == NNC_DYNAMIC && it.second.size() > 1
        && it.second[0].size() > 0 && isNative(it.first)) {

      properties.push_back(it.first);
    }
    else if (resultType == NNC_GENERATED && !isNative(it.first)) {

      properties.push_back(it.first);
    }
  }

  return properties;
}

// -----------------------------------------------------------------
void RINNCData::setScalarResultIndex(const QString& nncDataType,
                                      size_t scalarResultIndex) {
  m_resultIndexToNNCDataType[scalarResultIndex] = nncDataType;
}

// -----------------------------------------------------------------
bool RINNCData::hasScalarValues(size_t scalarResultIndex) {

  QString nncDataType = getNNCDataTypeFromScalarResultIndex(scalarResultIndex);
  if (nncDataType.isNull()) return false;

  auto it = m_connectionResults.find(nncDataType);
  return (it != m_connectionResults.end());
}

// -----------------------------------------------------------------
const QString
RINNCData::getNNCDataTypeFromScalarResultIndex(size_t scalarResultIndex) const {

  auto it = m_resultIndexToNNCDataType.find(scalarResultIndex);
  if (it != m_resultIndexToNNCDataType.end()) {
    return it->second;
  }
  return QString();
}

// -----------------------------------------------------------------
bool RINNCData::isNative(QString nncDataType) const {

  if (nncDataType == RINNCData::propertyNameCombTrans() ||
      nncDataType == RINNCData::propertyNameFluxGas() ||
      nncDataType == RINNCData::propertyNameFluxOil() ||
      nncDataType == RINNCData::propertyNameFluxWat() ||
      nncDataType == RINNCData::propertyNameRiCombMult() ||
      nncDataType == RINNCData::propertyNameRiCombTrans() ||
      nncDataType == RINNCData::propertyNameRiCombTransByArea()) {
    return true;
  }
  return false;
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
// Modified by M.Bellout on 3/6/18.
//

// ╦═╗  ╦  ╔═╗  ╔═╗  ╦ ╦  ╦    ╔╦╗  ╔═╗  ╔═╗  ╦═╗  ╔═╗  ╔═╗  ╦    ╦
// ╠╦╝  ║  ╠╣   ╠═╣  ║ ║  ║     ║   ╚═╗  ╠═╝  ╠╦╝  ║    ║╣   ║    ║
// ╩╚═  ╩  ╚    ╩ ╩  ╚═╝  ╩═╝   ╩   ╚═╝  ╩    ╩╚═  ╚═╝  ╚═╝  ╩═╝  ╩═╝
// =================================================================
//RIFaultsPrCellAccumulator RIFault::m_faultsPrCellAcc;

// ╦═╗  ╦  ╔═╗  ╔═╗  ╦ ╦  ╦    ╔╦╗
// ╠╦╝  ║  ╠╣   ╠═╣  ║ ║  ║     ║
// ╩╚═  ╩  ╚    ╩ ╩  ╚═╝  ╩═╝   ╩
// =================================================================
RIFault::RIFault() {
}

// -----------------------------------------------------------------
void
RIFault::addCellRangeForFace(cvf::StructGridInterface::FaceType face,
                              const cvf::CellRange& cellRange) {

  size_t faceIndex = static_cast<size_t>(face);
  CVF_ASSERT(faceIndex < 6);

  m_cellRangesForFaces[faceIndex].push_back(cellRange);
}

// -----------------------------------------------------------------
void RIFault::setName(const QString& name) {
  m_name = name;
}

// -----------------------------------------------------------------
QString RIFault::name() const {
  return m_name;
}

// -----------------------------------------------------------------
std::vector<RIFault::FaultFace>& RIFault::faultFaces() {
  return m_faultFaces;
}

// -----------------------------------------------------------------
const vector<RIFault::FaultFace>& RIFault::faultFaces() const {
  return m_faultFaces;
}

// -----------------------------------------------------------------
void
RIFault::computeFaultFacesFromCellRanges(const RIGrid* mainGrid) {

  if (!mainGrid) return;

  m_faultFaces.clear();

  for (size_t faceType = 0; faceType < 6; faceType++) {

    cvf::StructGridInterface::FaceType faceEnum = cvf::StructGridInterface::FaceType(faceType);

    const std::vector<cvf::CellRange>& cellRanges = m_cellRangesForFaces[faceType];

    for (size_t i = 0; i < cellRanges.size(); i++)
    {
      const cvf::CellRange& cellRange = cellRanges[i];

      cvf::Vec3st min, max;
      cellRange.range(min, max);

      for (size_t i = min.x(); i <= max.x(); i++)
      {
        if (i >= mainGrid->cellCountI())
        {
          continue;
        }

        for (size_t j = min.y(); j <= max.y(); j++)
        {
          if (j >= mainGrid->cellCountJ())
          {
            continue;
          }

          for (size_t k = min.z(); k <= max.z(); k++)
          {
            if (k >= mainGrid->cellCountK())
            {
              continue;
            }

            // Do not need to compute global grid cell index as for a maingrid localIndex == globalIndex
            //size_t reservoirCellIndex = grid->reservoirCellIndex(gridLocalCellIndex);

            size_t ni, nj, nk;
            mainGrid->neighborIJKAtCellFace(i, j, k, faceEnum, &ni, &nj, &nk);
            if (ni < mainGrid->cellCountI() && nj < mainGrid->cellCountJ() && nk < mainGrid->cellCountK())
            {
              size_t gridLocalCellIndex = mainGrid->cellIndexFromIJK(i, j, k);
              size_t oppositeCellIndex = mainGrid->cellIndexFromIJK(ni, nj, nk);

              m_faultFaces.push_back(FaultFace(gridLocalCellIndex, faceEnum, oppositeCellIndex));
            }
            else
            {
              //cvf::Trace::show("Warning: Undefined Fault neighbor detected.");
            }
          }
        }
      }
    }
  }
}

// -----------------------------------------------------------------
void
RIFault::accumulateFaultsPrCell(RIFaultsPrCellAccumulator* faultsPrCellAcc,
                                 int faultIdx) {

  for (size_t ffIdx = 0; ffIdx < m_faultFaces.size(); ++ffIdx) {
    const FaultFace& ff = m_faultFaces[ffIdx];

    // Could detect overlapping faults here .... if (faultsPrCellAcc->faultIdx(ff.m_nativeReservoirCellIndex, ff.m_nativeFace) >= 0)

    faultsPrCellAcc->setFaultIdx(ff.m_nativeReservoirCellIndex, ff.m_nativeFace, faultIdx);
    faultsPrCellAcc->setFaultIdx(ff.m_oppositeReservoirCellIndex, cvf::StructGridInterface::oppositeFace(ff.m_nativeFace), faultIdx);

  }
}