//
// Created by bellout on 3/2/18.
//

// -----------------------------------------------------------------
#include "well_path.h"

namespace cvf {
class HexIntersectionInfo;
}

// -----------------------------------------------------------------
namespace Reservoir {
namespace WellIndexCalculation {

// ╦ ╦  ╔═╗  ╦    ╦    ╔═╗  ╔═╗  ╔╦╗  ╦ ╦
// ║║║  ║╣   ║    ║    ╠═╝  ╠═╣   ║   ╠═╣
// ╚╩╝  ╚═╝  ╩═╝  ╩═╝  ╩    ╩ ╩   ╩   ╩ ╩
// =================================================================
WellPath::WellPath()
    : m_hasDatumElevation(false),
      m_datumElevation(0.0) {}

// -----------------------------------------------------------------
vector<cvf::HexIntersectionInfo>
WellPath::findRawHexCellIntersections(const RIGrid* grid,
                                      const vector<cvf::Vec3d>& coords) {

  vector<cvf::HexIntersectionInfo> intersections;

  // ---------------------------------------------------------------
  const QDateTime tstart = QDateTime::currentDateTime();
  std::stringstream str0, str1;
  str0 << "Find raw hexcell intersections. grid->nodes().size() = "
       << grid->nodes().size() << " coords.size() = " << coords.size();

  CVF_ASSERT(grid->nodes().size() > 0);
  // print_dbg_msg_wic_ri(__func__, str0.str(), 0.0, 1);

  for (size_t i = 0; i < coords.size() - 1; ++i) {

    // Dbg: coord i
    str1.str("");
    str1 << "coord[i=" << i << "]=( "
         << std::setw(10) << std::setprecision(3) << std::fixed
         << coords[i].x() << ", "
         << coords[i].y() << ", "
         << coords[i].z() << " )";
    print_dbg_msg_wic_ri(__func__, str1.str(), 0.0, 0);

    // Dbg: coord i+1
    str1.str("");
    str1 << "coord[i=" << i + 1 << "]=( "
         << std::setw(10) << std::setprecision(3) << std::fixed
         << coords[i+1].x() << ", "
         << coords[i+1].y() << ", "
         << coords[i+1].z() << " )";
    print_dbg_msg_wic_ri(__func__, str1.str(), 0.0, 0);

    // Add coords to bbox
    cvf::BoundingBox bb;
    bb.add(coords[i]);
    bb.add(coords[i + 1]);

    // Find cells close to bbox
    vector<size_t> closeCells = findCloseCells(grid, bb);

    // Loop through cell neighborhood
    array<cvf::Vec3d, 8> hexCorners;
    for (size_t closeCell : closeCells) {

      // Get current cell
      const RICell& cell = grid->globalCellArray()[closeCell];
      if (cell.isInvalid()) {
        print_dbg_msg_wic_ri(__func__, "Cell is invalid", 0.0, 0);
        continue;
      }

      // Get corner vertices of current cell
      grid->cellCornerVertices(closeCell, hexCorners.data());

      // Dbg
      str1.str("");
      for (int i=0; i < hexCorners.size(); i++) {

        if (i < 1 && intersections.size() < 4) {
          str1 << std::setw(10) << std::setprecision(3) << std::fixed
               << "hexCorner[i=" << i << "]=( "
               << hexCorners[i].x() << ", "
               << hexCorners[i].y() << ", "
               << hexCorners[i].z() << " ) -- intersection.sz = "
               << intersections.size();
          // print_dbg_msg_wic_ri(__func__, str1.str(), 0.0, 0);
        }
      }

      //
      cvf::RigHexIntersectionTools::lineHexCellIntersection(
          coords[i], coords[i + 1], hexCorners.data(),
          closeCell, &intersections);
    } // End: for (size_t closeCell : closeCells)
  }

  str1.str("");
  str1 << "# of intersections found = " << intersections.size();
  print_dbg_msg_wic_ri(__func__, str1.str(), 0.0, 0);
  // print_dbg_msg_wic_ri(__func__, str0.str(), time_since_msecs(tstart), 2);

  return intersections;
}

// -----------------------------------------------------------------
cvf::Vec3d WellPath::calculateLengthInCell(
    const array<cvf::Vec3d, 8>& hexCorners,
    const cvf::Vec3d& startPoint,
    const cvf::Vec3d& endPoint) {

  cvf::Vec3d vec = endPoint - startPoint;
  cvf::Vec3d iAxisDirection;
  cvf::Vec3d jAxisDirection;
  cvf::Vec3d kAxisDirection;

  RigCellGeometryTools::findCellLocalXYZ(hexCorners,
                                         iAxisDirection,
                                         jAxisDirection,
                                         kAxisDirection);

  cvf::Mat3d localCellCoordinateSystem(
      iAxisDirection.x(), jAxisDirection.x(), kAxisDirection.x(),
      iAxisDirection.y(), jAxisDirection.y(), kAxisDirection.y(),
      iAxisDirection.z(), jAxisDirection.z(), kAxisDirection.z());

  return vec.getTransformedVector(localCellCoordinateSystem.getInverted());
}

// -----------------------------------------------------------------
cvf::Vec3d WellPath::calculateLengthInCell(const RIGrid* grid,
                                           size_t cellIndex,
                                           const cvf::Vec3d& startPoint,
                                           const cvf::Vec3d& endPoint) {

  std::array<cvf::Vec3d, 8> hexCorners;
  grid->cellCornerVertices(cellIndex, hexCorners.data());

  return calculateLengthInCell(hexCorners, startPoint, endPoint);
}

// -----------------------------------------------------------------
vector<size_t> WellPath::findCloseCells(const RIGrid* grid,
                                        const cvf::BoundingBox& bb) {

  vector<size_t> closeCells;
  grid->findIntersectingCells(bb, &closeCells);
  return closeCells;
}

// -----------------------------------------------------------------
size_t WellPath::findCellFromCoords(const RIGrid* grid,
                                    const cvf::Vec3d& coords,
                                    bool* foundCell) {

  cvf::BoundingBox bb;
  bb.add(coords);
  vector<size_t> closeCells = findCloseCells(grid, bb);

  array<cvf::Vec3d, 8> hexCorners;
  for (size_t closeCell : closeCells) {

    const RICell& cell = grid->globalCellArray()[closeCell];
    if (cell.isInvalid()) continue;

    grid->cellCornerVertices(closeCell, hexCorners.data());

    if (cvf::RigHexIntersectionTools::isPointInCell(coords, hexCorners.data())) {
      *foundCell = true;
      return closeCell;
    }
  }

  *foundCell = false;
  return 0;
}

// -----------------------------------------------------------------
double
WellPath::calculateTransmissibility(const cvf::Vec3d& internalCellLengths,
                                    double skinFactor,
                                    double wellRadius,
                                    size_t cellIndex,
                                    bool useLateralNTG,
                                    IntersectedCell &icell,
                                    size_t volScaleConst,
                                    CellDir dirForVolScaling) {

  // -------------------------------------------------------------
  double dx, dy, dz, permx, permy, permz;

  dx = icell.dx();
  dy = icell.dy();
  dz = icell.dz();

  permx = icell.permx()[0];
  permy = icell.permy()[0];
  permz = icell.permz()[0];

  // -------------------------------------------------------------
  if (volScaleConst != 1) {
    if (dirForVolScaling == CellDir::DIR_I) dx = dx / volScaleConst;
    if (dirForVolScaling == CellDir::DIR_J) dy = dy / volScaleConst;
    if (dirForVolScaling == CellDir::DIR_K) dz = dz / volScaleConst;
  }

  // -------------------------------------------------------------
  double darcy = 0.008527;
  double latNtg = 1.0;
  double ntg = 1.0;

  // -------------------------------------------------------------
  double transx =
      wellBoreTransmissibilityComponent(internalCellLengths.x() * latNtg,
                                        permy, permz, dy, dz, wellRadius,
                                        skinFactor, darcy);

  double transy =
      wellBoreTransmissibilityComponent(internalCellLengths.y() * latNtg,
                                        permx, permz, dx, dz, wellRadius,
                                        skinFactor, darcy);

  double transz =
      wellBoreTransmissibilityComponent(internalCellLengths.z() * ntg,
                                        permy, permx, dy, dx, wellRadius,
                                        skinFactor, darcy);

  // DBG ---------------------------------------------------------
  icell.set_segment_calculation_data(0,"dx",dx);
  icell.set_segment_calculation_data(0,"dy",dy);
  icell.set_segment_calculation_data(0,"dz",dz);

  icell.set_segment_calculation_data(0,"Lx",internalCellLengths.x() * latNtg);
  icell.set_segment_calculation_data(0,"Ly",internalCellLengths.y() * latNtg);
  icell.set_segment_calculation_data(0,"Lz",internalCellLengths.z() * ntg);

  icell.set_segment_calculation_data(0,"wx_m",transx);
  icell.set_segment_calculation_data(0,"wy_m",transy);
  icell.set_segment_calculation_data(0,"wz_m",transz);

  // -------------------------------------------------------------
  // Return total trans.factor
  double wcf = cvf::Math::sqrt(
      pow(transx, 2.0) + pow(transy, 2.0) + pow(transz, 2.0));

  return wcf;
}

// -----------------------------------------------------------------
double
WellPath::wellBoreTransmissibilityComponent(
    double cellPerforationVectorComponent,
    double permeabilityNormalDirection1,
    double permeabilityNormalDirection2,
    double cellSizeNormalDirection1,
    double cellSizeNormalDirection2,
    double wellRadius,
    double skinFactor,
    double cDarcyForRelevantUnit) {

  double K =
      cvf::Math::sqrt(permeabilityNormalDirection1 * permeabilityNormalDirection2);

  double nominator =
      cDarcyForRelevantUnit * 2 * cvf::PI_D * K * cellPerforationVectorComponent;

  // DBG : perm values -------------------------------------------
  std::stringstream str;
  str << setw(1) << "K=" << setw(7) << setprecision(3) << fixed << K;
  // print_dbg_msg_wic_ri(__func__, str.str(), 0.0, 0, false);

  // -------------------------------------------------------------
  double peaceManRad = peacemanRadius(permeabilityNormalDirection1,
                                      permeabilityNormalDirection2,
                                      cellSizeNormalDirection1,
                                      cellSizeNormalDirection2);

  double denominator = log(peaceManRad / wellRadius) + skinFactor;

  // -------------------------------------------------------------
  double trans = nominator / denominator;
  return trans;
}

// -----------------------------------------------------------------
double WellPath::peacemanRadius(double permeabilityNormalDirection1,
                                   double permeabilityNormalDirection2,
                                   double cellSizeNormalDirection1,
                                   double cellSizeNormalDirection2) {

  // -------------------------------------------------------------
  double numerator = cvf::Math::sqrt(
      pow(cellSizeNormalDirection2, 2.0) *
          pow(permeabilityNormalDirection1 / permeabilityNormalDirection2, 0.5)
          + pow(cellSizeNormalDirection1, 2.0) *
              pow(permeabilityNormalDirection2 / permeabilityNormalDirection1, 0.5) );

  // -------------------------------------------------------------
  double denominator = pow((permeabilityNormalDirection1 / permeabilityNormalDirection2), 0.25 )
      + pow((permeabilityNormalDirection2 / permeabilityNormalDirection1), 0.25 );

  double r0 = 0.28 * numerator / denominator;

  return r0;
}

// -----------------------------------------------------------------
CellDir
WellPath::calculateDirectionInCell(WellPathCellIntersectionInfo cell,
                                   IntersectedCell &icell) {

  // -------------------------------------------------------------
  double dx, dy, dz;
  dx = icell.dxdydz()[0];
  dy = icell.dxdydz()[1];
  dz = icell.dxdydz()[2];

  // DBG : dx, dy, dz values ---------------------------------------
  std::stringstream str;
  str << setw(3) << "dx=" << setw(7) << setprecision(3) << fixed << dx << " ";
  str << setw(3) << "dy=" << setw(7) << setprecision(3) << fixed << dy << " ";
  str << setw(3) << "dz=" << setw(7) << setprecision(3) << fixed << dz << "\n";
  // print_dbg_msg_wic_ri(__func__, str.str(), 0.0, 0, false);

  // ---------------------------------------------------------------
  double xLengthFraction = fabs(cell.intersectionLengthsInCellCS.x() / dx);
  double yLengthFraction = fabs(cell.intersectionLengthsInCellCS.y() / dy);
  double zLengthFraction = fabs(cell.intersectionLengthsInCellCS.z() / dz);

  if (xLengthFraction > yLengthFraction && xLengthFraction > zLengthFraction) {
    return CellDir::DIR_I;
  }
  else if (yLengthFraction > xLengthFraction && yLengthFraction > zLengthFraction) {
    return CellDir::DIR_J;
  }
  else {
    return CellDir::DIR_K;
  }
}

////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2017 Statoil ASA
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

// ╦═╗  ╦  ╔═╗  ╔═╗  ╔╦╗  ╔═╗  ╔╦╗  ╔═╗  ╔╦╗  ╔═╗
// ╠╦╝  ║  ║    ║ ║  ║║║  ╠═╝   ║║  ╠═╣   ║   ╠═╣
// ╩╚═  ╩  ╚═╝  ╚═╝  ╩ ╩  ╩    ═╩╝  ╩ ╩   ╩   ╩ ╩
// =================================================================
RICompData::RICompData(const QString wellName,
                       const IJKCellIndex& cellIndex)
    : m_wellName(wellName),
      m_cellIndex(cellIndex),
      m_saturation(HUGE_VAL),
      m_transmissibility(HUGE_VAL),
      m_diameter(HUGE_VAL),
      m_kh(HUGE_VAL),
      m_skinFactor(HUGE_VAL),
      m_dFactor(HUGE_VAL),
      m_direction(DIR_UNDEF),
      m_connectionState(OPEN),
      m_count(1),
      m_wpimult(HUGE_VAL),
      m_isMainBore(false),
      m_readyForExport(false),
      m_completionType(CT_UNDEFINED) {
}

// -----------------------------------------------------------------
RICompData::~RICompData() {
}

// -----------------------------------------------------------------
RICompData::RICompData(const RICompData& other) {
  copy(*this, other);
}

// -----------------------------------------------------------------
RICompData
RICompData::combine(const vector<RICompData>& completions) {

  CVF_ASSERT(!completions.empty());

  // ---------------------------------------------------------------
  auto it = completions.cbegin();
  RICompData result(*it);
  ++it;

  // ---------------------------------------------------------------
  for (; it != completions.cend(); ++it) {

    // -------------------------------------------------------------
    if (it->completionType() != result.completionType()) {

      // QString("Cannot combine completions of different types "
      //            "in same cell [%1, %2, %3]")
      //    .arg(result.m_cellIndex.i)
      //    .arg(result.m_cellIndex.j)
      //    .arg(result.m_cellIndex.k);

      continue;
    }
    if (onlyOneIsDefaulted(
        result.m_transmissibility, it->m_transmissibility)) {
      // "Transmissibility defaulted in one but not
      // both, will produce erroneous result";
    }
    else {
      result.m_transmissibility += it->m_transmissibility;
    }

    // -------------------------------------------------------------
    result.m_metadata.reserve(
        result.m_metadata.size() + it->m_metadata.size());

    result.m_metadata.insert(result.m_metadata.end(),
                             it->m_metadata.begin(),
                             it->m_metadata.end());

    result.m_count += it->m_count;
  }

  return result;
}

// -----------------------------------------------------------------
bool RICompData::operator<(const RICompData& other) const {

  if (m_wellName != other.m_wellName) {
    return (m_wellName < other.m_wellName);
  }

  return m_cellIndex < other.m_cellIndex;
}

// -----------------------------------------------------------------
RICompData& RICompData::operator=(const RICompData& other) {

  if (this != &other) {
    copy(*this, other);
  }
  return *this;
}

// -----------------------------------------------------------------
void RICompData::setFromFracture(double transmissibility,
                                 double skinFactor) {

  m_completionType = FRACTURE;
  m_transmissibility = transmissibility;
  m_skinFactor = skinFactor;
}

// -----------------------------------------------------------------
void
RICompData::setTransAndWPImultBackgroundDataFromPerforation(double transmissibility,
                                                            double skinFactor,
                                                            double diameter,
                                                            CellDir direction) {

  m_completionType = PERFORATION;
  m_transmissibility = transmissibility;
  m_skinFactor = skinFactor;
  m_diameter = diameter;
  m_direction = direction;
  m_isMainBore = true;
}

// -----------------------------------------------------------------
void
RICompData::setCombinedValuesExplicitTrans(double transmissibility,
                                           CompletionType completionType) {
  m_completionType = completionType;
  m_transmissibility = transmissibility;
  m_readyForExport = true;
}

// -----------------------------------------------------------------
void
RICompData::setCombinedValuesImplicitTransWPImult(double wpimult,
                                                  CellDir celldirection,
                                                  double skinFactor,
                                                  double wellDiameter,
                                                  CompletionType completionType) {
  m_wpimult = wpimult;
  m_direction = celldirection;
  m_completionType = completionType;
  m_skinFactor = skinFactor;
  m_diameter = wellDiameter;
  m_readyForExport = true;
}

// -----------------------------------------------------------------
void RICompData::addMetadata(const QString& name,
                             const QString& comment) {
  m_metadata.push_back(RICompMetaData(name, comment));
}

// -----------------------------------------------------------------
bool RICompData::isDefaultValue(double val) {
  return val == HUGE_VAL;
}

// -----------------------------------------------------------------
bool RICompData::onlyOneIsDefaulted(double first, double second) {

  if (first == HUGE_VAL) {
    if (second == HUGE_VAL) {
      // Both have default values
      return false;

    } else {
      // First has default value, second does not
      return true;
    }
  }
  if (second == HUGE_VAL) {
    // Second has default value, first does not
    return true;
  }

  // Neither has default values
  return false;
}

// -----------------------------------------------------------------
void RICompData::copy(RICompData& target, const RICompData& from) {

  target.m_metadata = from.m_metadata;
  target.m_wellName = from.m_wellName;
  target.m_cellIndex = from.m_cellIndex;
  target.m_connectionState = from.m_connectionState;
  target.m_saturation = from.m_saturation;
  target.m_transmissibility = from.m_transmissibility;
  target.m_diameter = from.m_diameter;
  target.m_kh = from.m_kh;
  target.m_skinFactor = from.m_skinFactor;
  target.m_dFactor = from.m_dFactor;
  target.m_direction = from.m_direction;
  target.m_isMainBore = from.m_isMainBore;
  target.m_readyForExport = from.m_readyForExport;
  target.m_count = from.m_count;
  target.m_wpimult = from.m_wpimult;
  target.m_completionType = from.m_completionType;
}


}
}
