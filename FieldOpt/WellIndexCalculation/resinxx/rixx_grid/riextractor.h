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

#ifndef FIELDOPT_RIEXTRACTOR_H
#define FIELDOPT_RIEXTRACTOR_H

// -----------------------------------------------------------------
// STD / EIGEN
//#include <string>
//#include <Eigen/Core>
//#include <Eigen/Dense>

// RESINSIGHT: FWK/VIZFWK/LIBCORE ----------------------------------
//#include "../rixx_core_geom/cvfObject.h"

// RESINSIGHT: FWK/APPFWK/COMMONCODE -------------------------------

// RESINSIGHT: ECL/GRID/READER/OUTPUT ------------------------------
//#include "../rixx_grid/ricasedata.h"

// -----------------------------------------------------------------
#include "../well_path.h"
//#include "../geometry_tools.h"

// -----------------------------------------------------------------
//class RIECLCaseData;
//class WellPath;

namespace Reservoir {
namespace WellIndexCalculation {
class WellPath;
}
}

// -----------------------------------------------------------------
namespace cvf {
class BoundingBox;
class HexIntersectionInfo;
}

// -----------------------------------------------------------------
using std::string;
using std::vector;
using std::map;

// ╦ ╦  ╔═╗  ╔═╗  ╔╦╗  ╦ ╦  ╔═╗  ╔═╗  ╦    ╦    ╦  ╔╗╔  ╔╦╗  ╦═╗  ═╗ ╦  ╦  ╔╗╔  ╔═╗
// ║║║  ╠═╝  ╠═╣   ║   ╠═╣  ║    ║╣   ║    ║    ║  ║║║   ║   ╠╦╝  ╔╩╦╝  ║  ║║║  ╠╣
// ╚╩╝  ╩    ╩ ╩   ╩   ╩ ╩  ╚═╝  ╚═╝  ╩═╝  ╩═╝  ╩  ╝╚╝   ╩   ╩╚═  ╩ ╚═  ╩  ╝╚╝  ╚
//====================================================================
struct WellPathCellIntersectionInfo {

  size_t globCellIndex;
  cvf::Vec3d startPoint;
  cvf::Vec3d endPoint;
  double startMD;
  double endMD;
  cvf::Vec3d intersectionLengthsInCellCS;
  Eigen::Vector3d dxdydz;

  cvf::StructGridInterface::FaceType intersectedCellFaceIn;
  cvf::StructGridInterface::FaceType intersectedCellFaceOut;
};

// ╦═╗  ╦  ╔═╗  ═╗ ╦  ╔╦╗  ╦═╗  ╔═╗  ╔═╗  ╔╦╗  ╦  ╔═╗  ╔╗╔  ╔╦╗  ╔═╗  ╔═╗  ╦    ╔═╗
// ╠╦╝  ║  ║╣   ╔╩╦╝   ║   ╠╦╝  ╠═╣  ║     ║   ║  ║ ║  ║║║   ║   ║ ║  ║ ║  ║    ╚═╗
// ╩╚═  ╩  ╚═╝  ╩ ╚═   ╩   ╩╚═  ╩ ╩  ╚═╝   ╩   ╩  ╚═╝  ╝╚╝   ╩   ╚═╝  ╚═╝  ╩═╝  ╚═╝
//====================================================================
struct RigWellLogExtractionTools {

  static bool isEqualDepth(double d1, double d2) {
    double depthDiff = d1 - d2;

    const double tolerance = 0.1;// Meters To handle inaccuracies across faults

    return (fabs(depthDiff) < tolerance); // Equal depth
  }
};

// ╦═╗  ╦  ╔╦╗  ╔╦╗  ╔═╗  ╔═╗  ╦    ╦    ╦  ╔╦╗  ═╗ ╦  ╔═╗  ╔╗╔  ╔╦╗  ╔═╗  ╦═╗  ╦    ╦╔═  ╔═╗  ╦ ╦
// ╠╦╝  ║  ║║║   ║║  ║    ║╣   ║    ║    ║   ║║  ╔╩╦╝  ║╣   ║║║   ║   ║╣   ╠╦╝  ║    ╠╩╗  ║╣   ╚╦╝
// ╩╚═  ╩  ╩ ╩  ═╩╝  ╚═╝  ╚═╝  ╩═╝  ╩═╝  ╩  ═╩╝  ╩ ╚═  ╚═╝  ╝╚╝   ╩   ╚═╝  ╩╚═  ╩═╝  ╩ ╩  ╚═╝   ╩
//====================================================================
// Class used to sort the intersections along the wellpath, Use as key
// in a map - Sorting according to MD first, then Cell Idx, then Leaving
// before entering cell
struct RIMDCellIdxEnterLeaveKey {

  RIMDCellIdxEnterLeaveKey(double md,
                           size_t cellIdx,
                           bool entering):
      measuredDepth(md), hexIndex(cellIdx), isEnteringCell(entering){}

  double measuredDepth;
  size_t hexIndex;
  bool isEnteringCell; // As opposed to leaving.
  bool isLeavingCell() const { return !isEnteringCell;}

  bool operator < (const RIMDCellIdxEnterLeaveKey& other) const {

    if (RigWellLogExtractionTools::isEqualDepth(measuredDepth, other.measuredDepth)) {

      if (hexIndex == other.hexIndex) {

        if (isEnteringCell == other.isEnteringCell) {
          // Completely equal: intersections at cell edges
          // or corners or edges of the face triangles
          return false;
        }

        return !isEnteringCell; // Sort Leaving cell before (less than) entering cell
      }

      return (hexIndex < other.hexIndex);
    }

    // The depths are not equal
    return (measuredDepth < other.measuredDepth);
  }
};

// ╦═╗  ╦  ╔╦╗  ╔╦╗  ╔═╗  ╔╗╔  ╔╦╗  ╔═╗  ╦═╗  ╦    ╦  ╦  ╔═╗  ╔═╗  ╔═╗  ╦    ╦    ╦  ╔╦╗  ═╗ ╦  ╦╔═
// ╠╦╝  ║  ║║║   ║║  ║╣   ║║║   ║   ║╣   ╠╦╝  ║    ╚╗╔╝  ║╣   ║    ║╣   ║    ║    ║   ║║  ╔╩╦╝  ╠╩╗
// ╩╚═  ╩  ╩ ╩  ═╩╝  ╚═╝  ╝╚╝   ╩   ╚═╝  ╩╚═  ╩═╝   ╚╝   ╚═╝  ╚═╝  ╚═╝  ╩═╝  ╩═╝  ╩  ═╩╝  ╩ ╚═  ╩ ╩
//====================================================================
// Class used to sort the intersections along the wellpath, use as key
// in a map - Sorting according to MD first,then Leaving before entering
// cell, then Cell Idx.
struct RIMDEnterLeaveCellIdxKey
{
  RIMDEnterLeaveCellIdxKey(double md,
                           bool entering,
                           size_t cellIdx ):
      measuredDepth(md), hexIndex(cellIdx), isEnteringCell(entering){}

  double measuredDepth;
  bool   isEnteringCell; // As opposed to leaving.
  bool   isLeavingCell() const { return !isEnteringCell;}
  size_t hexIndex;

  bool operator < (const RIMDEnterLeaveCellIdxKey& other) const {

    if (RigWellLogExtractionTools::isEqualDepth(measuredDepth,
                                                other.measuredDepth)) {

      if (isEnteringCell == other.isEnteringCell) {

        if (hexIndex == other.hexIndex) {
          // Completely equal: intersections at cell edges
          // or corners or edges of the face triangles
          return false;
        }

        return (hexIndex < other.hexIndex);
      }

      // Sort Leaving cell before (less than) entering cell
      return isLeavingCell();
    }

    // The depths are not equal
    return (measuredDepth < other.measuredDepth);
  }

  static bool isProperCellEnterLeavePair(const RIMDEnterLeaveCellIdxKey& key1,
                                         const RIMDEnterLeaveCellIdxKey& key2 )
  {
    return ( key1.hexIndex == key2.hexIndex
        && key1.isEnteringCell && key2.isLeavingCell()
        && !RigWellLogExtractionTools::isEqualDepth(key1.measuredDepth,
                                                    key2.measuredDepth) );
  }
};

// ╦═╗  ╦  ╔═╗  ═╗ ╦  ╔╦╗  ╦═╗  ╔═╗  ╔═╗  ╔╦╗  ╔═╗  ╦═╗
// ╠╦╝  ║  ║╣   ╔╩╦╝   ║   ╠╦╝  ╠═╣  ║     ║   ║ ║  ╠╦╝
// ╩╚═  ╩  ╚═╝  ╩ ╚═   ╩   ╩╚═  ╩ ╩  ╚═╝   ╩   ╚═╝  ╩╚═
//==================================================================
class RIExtractor : public cvf::Object
{
 public:
  RIExtractor(const RICaseData* aCase,
              const Reservoir::WellIndexCalculation::WellPath& wellpath);
  virtual ~RIExtractor();

  const vector<double>& measuredDepth()
  { return m_measuredDepth; }

  const vector<double>& trueVerticalDepth()
  { return m_trueVerticalDepth; }

  const vector<size_t>& intersectedCellsGlobIdx();

  const Reservoir::WellIndexCalculation::WellPath* wellPathData()
  { return m_wellPath.p();}

  vector<WellPathCellIntersectionInfo>
  cellIntersectionInfosAlongWellPath() const;

 protected:

  static void insertIntersectionsInMap(
      const vector<cvf::HexIntersectionInfo> &intersections,
      cvf::Vec3d p1, double md1,
      cvf::Vec3d p2, double md2,
      map<RIMDCellIdxEnterLeaveKey, cvf::HexIntersectionInfo > *uniqueIntersections);

  void populateReturnArrays(
      map<RIMDCellIdxEnterLeaveKey, cvf::HexIntersectionInfo > &uniqueIntersections);

  void appendIntersectionToArrays(double measuredDepth,
                                  const cvf::HexIntersectionInfo& intersection);

  virtual cvf::Vec3d calculateLengthInCell(size_t cellIndex,
                                           const cvf::Vec3d& startPoint,
                                           const cvf::Vec3d& endPoint) const = 0;

  vector<cvf::Vec3d> m_intersections;
  vector<size_t> m_intersectedCellsGlobIdx;

  vector<cvf::StructGridInterface::FaceType> m_intersectedCellFaces;

  cvf::cref<Reservoir::WellIndexCalculation::WellPath> m_wellPath;

 protected:
  cvf::cref<RICaseData> m_caseData;

 private:
  vector<double> m_measuredDepth;
  vector<double> m_trueVerticalDepth;

};

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
class RIECLExtractor : public RIExtractor
{
 public:
  RIECLExtractor(const RICaseData* aCase,
                 const Reservoir::WellIndexCalculation::WellPath& wellpath);

//  void curveData(
//      const RIResultAccessor* resultAccessor,
//      std::vector<double>* values );

  const RICaseData* caseData() { return m_caseData.p(); }


 protected:
  void calculateIntersection();
  std::vector<size_t> findCloseCells(const cvf::BoundingBox& bb);

  virtual cvf::Vec3d calculateLengthInCell(
      size_t cellIndex,
      const cvf::Vec3d& startPoint,
      const cvf::Vec3d& endPoint) const override;

  cvf::cref<RICaseData> m_caseData;
};

#endif //FIELDOPT_RIEXTRACTOR_H
