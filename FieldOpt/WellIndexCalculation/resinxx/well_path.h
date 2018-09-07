////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011-2012 Statoil ASA, Ceetron AS
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
// Modified by M.Bellout on 3/2/18.
//

// -----------------------------------------------------------------
#ifndef FIELDOPT_WELL_PATH_H
#define FIELDOPT_WELL_PATH_H

// -----------------------------------------------------------------
// STD
#include <vector>
#include <iostream>
#include <string>
#include <array>
#include <list>
#include <set>
#include <utility>
#include <cstddef>

// -----------------------------------------------------------------
// EIGEN
#include <Eigen/Core>
#include <Eigen/Dense>

// -----------------------------------------------------------------
// Qt
#include "QString"
#include "QDateTime"

// FieldOpt --------------------------------------------------------
#include "Reservoir/grid/cell.h"
#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"
#include "Settings/model.h"
#include "WellIndexCalculation/intersected_cell.h"
#include "Utilities/debug.hpp"
#include "Utilities/time.hpp"

// RESINSIGHT: FWK/VIZFWK/LIBCORE\LIBGEOMETRY ----------------------
#include "rixx_core_geom/cvfBase.h"
#include "rixx_core_geom/cvfAssert.h"
#include "rixx_core_geom/cvfVector2.h"
#include "rixx_core_geom/cvfVector3.h"
#include "rixx_core_geom/cvfPlane.h"
#include "rixx_core_geom/cvfBoundingBox.h"
#include "rixx_core_geom/cvfRay.h"
#include "rixx_core_geom/cvfMath.h"

// RESINSIGHT: FWK/APPFWK/COMMONCODE\VIZEXT\PROJDATAMOD ------------
#include "rixx_app_fwk/cvfStructGrid.h"
#include "rixx_app_fwk/cafHexGridIntersectionTools.h"

// RESINSIGHT: APPLICATIONCODE/RESERVOIRDATAMODEL ------------------
//#include "rixx_res_mod/cvfGeometryTools.h"
#include "rixx_res_mod/RigCellGeometryTools.h"

// RESINSIGHT: APPLICATIONCODE/PROJECTDATAMO/MODVIZ ----------------
#include "rixx_prj_viz/RimIntersection.h"

// FieldOpt::RESINXX -----------------------------------------------
#include "geometry_tools.h"
#include "rixx_grid/ricasedata.h"
#include "rixx_grid/rigrid.h"
#include "rixx_grid/ricell.h"
#include "rixx_grid/riextractor.h"

// -----------------------------------------------------------------
namespace cvf {
class HexIntersectionInfo;
}

// -----------------------------------------------------------------
class WellPathCellIntersectionInfo;

// -----------------------------------------------------------------
namespace Reservoir {
namespace WellIndexCalculation {

// -----------------------------------------------------------------
using std::vector;
using std::array;

// -----------------------------------------------------------------
enum WellConnectionState {
  OPEN,
  SHUT,
  AUTO
};

// -----------------------------------------------------------------
enum CellDir {
  DIR_I,
  DIR_J,
  DIR_K,
  DIR_UNDEF
};

// ╦ ╦  ╔═╗  ╦    ╦    ╔═╗  ╔═╗  ╔╦╗  ╦ ╦
// ║║║  ║╣   ║    ║    ╠═╝  ╠═╣   ║   ╠═╣
// ╚╩╝  ╚═╝  ╩═╝  ╩═╝  ╩    ╩ ╩   ╩   ╩ ╩
// =================================================================
class WellPath : public cvf::Object
{
 public:
  WellPath();
  ~WellPath(){};

  // ---------------------------------------------------------------
  vector <cvf::Vec3d> m_wellPathPoints;
  vector<double> m_measuredDepths;

  // ---------------------------------------------------------------
  // void setDatumElevation(double value);
  // bool hasDatumElevation() const;
  // double datumElevation() const;

  // ---------------------------------------------------------------
  // cvf::Vec3d interpolatedPointAlongWellPath(double measuredDepth) const;
  // double wellPathAzimuthAngle(const cvf::Vec3d& position) const;

  // ---------------------------------------------------------------
  // void twoClosestPoints(const cvf::Vec3d& position,
  //                      cvf::Vec3d* p1,
  //                      cvf::Vec3d* p2) const;

  // std::pair<std::vector<cvf::Vec3d>, std::vector<double> >
  // clippedPointSubset(double startMD, double endMD) const;

   // std::vector<cvf::Vec3d>
   // wellPathPointsIncludingInterpolatedIntersectionPoint(
   //    double intersectionMeasuredDepth) const;

  // ---------------------------------------------------------------
  static vector<cvf::HexIntersectionInfo>
  findRawHexCellIntersections(const RIGrid* grid,
                              const vector<cvf::Vec3d>& coords);

  static cvf::Vec3d calculateLengthInCell(const array<cvf::Vec3d, 8>& hexCorners,
                                          const cvf::Vec3d& startPoint,
                                          const cvf::Vec3d& endPoint);

  static cvf::Vec3d calculateLengthInCell(const RIGrid* grid,
                                          size_t cellIndex,
                                          const cvf::Vec3d& startPoint,
                                          const cvf::Vec3d& endPoint);

  static vector<size_t> findCloseCells(const RIGrid* grid,
                                       const cvf::BoundingBox& bb);

  static size_t findCellFromCoords(const RIGrid* grid,
                                   const cvf::Vec3d& coords,
                                   bool* foundCell);

  // ---------------------------------------------------------------
  double calculateTransmissibility(// RICaseData* eclipseCase,
                                   const cvf::Vec3d& internalCellLengths,
                                   double skinFactor,
                                   double wellRadius,
                                   size_t cellIndex,
                                   bool useLateralNTG,
                                   IntersectedCell &icell,
                                   size_t volumeScaleConstant = 1,
                                   CellDir directionForVolumeScaling = CellDir::DIR_I);

  double peacemanRadius(double permeabilityNormalDirection1,
                        double permeabilityNormalDirection2,
                        double cellSizeNormalDirection1,
                        double cellSizeNormalDirection2);

  double wellBoreTransmissibilityComponent(double cellPerforationVectorComponent,
                                           double permeabilityNormalDirection1,
                                           double permeabilityNormalDirection2,
                                           double cellSizeNormalDirection1,
                                           double cellSizeNormalDirection2,
                                           double wellRadius,
                                           double skinFactor,
                                           double cDarcyForRelevantUnit);

  CellDir calculateDirectionInCell(// RICaseData* eclipseCase,
                                   WellPathCellIntersectionInfo cellInfo,
                                   IntersectedCell &icell);

 private:
  bool m_hasDatumElevation;
  double m_datumElevation;
};

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

// ╦   ╦  ╦╔═  ╔═╗  ╔═╗  ╦    ╦    ╦  ╔╗╔  ╔╦╗  ╔═╗  ═╗ ╦
// ║   ║  ╠╩╗  ║    ║╣   ║    ║    ║  ║║║   ║║  ║╣   ╔╩╦╝
// ╩  ╚╝  ╩ ╩  ╚═╝  ╚═╝  ╩═╝  ╩═╝  ╩  ╝╚╝  ═╩╝  ╚═╝  ╩ ╚═
// =================================================================
class IJKCellIndex {
 public:
  IJKCellIndex() {};
  IJKCellIndex(size_t i, size_t j, size_t k) : i(i), j(j), k(k) {};

  IJKCellIndex(const IJKCellIndex& other) {
    i = other.i;
    j = other.j;
    k = other.k;
  }

  bool operator==(const IJKCellIndex& other) const {
    return i == other.i && j == other.j && k == other.k;
  }

  bool operator<(const IJKCellIndex& other) const {

    if (i != other.i) return i < other.i;
    if (j != other.j) return j < other.j;
    if (k != other.k) return k < other.k;
    return false;
  }

  size_t i;
  size_t j;
  size_t k;
};

// ╦═╗  ╦  ╔═╗  ╔═╗  ╔╦╗  ╔═╗  ╔╦╗  ╔═╗  ╔╦╗  ╔═╗  ╔╦╗  ╔═╗  ╔╦╗  ╔═╗
// ╠╦╝  ║  ║    ║ ║  ║║║  ╠═╝  ║║║  ║╣    ║   ╠═╣   ║║  ╠═╣   ║   ╠═╣
// ╩╚═  ╩  ╚═╝  ╚═╝  ╩ ╩  ╩    ╩ ╩  ╚═╝   ╩   ╩ ╩  ═╩╝  ╩ ╩   ╩   ╩ ╩
// =================================================================
struct RICompMetaData {
  RICompMetaData(const QString& name,
                        const QString& comment)
      : name(name), comment(comment) {}

  QString name;
  QString comment;
};

// ╦═╗  ╦  ╔═╗  ╔═╗  ╔╦╗  ╔═╗  ╔╦╗  ╔═╗  ╔╦╗  ╔═╗
// ╠╦╝  ║  ║    ║ ║  ║║║  ╠═╝   ║║  ╠═╣   ║   ╠═╣
// ╩╚═  ╩  ╚═╝  ╚═╝  ╩ ╩  ╩    ═╩╝  ╩ ╩   ╩   ╩ ╩
// =================================================================
class RICompData
{
 public:
  enum CompletionType {
    FISHBONES,
    FRACTURE,
    PERFORATION,
    CT_UNDEFINED
  };

  // ---------------------------------------------------------------
  RICompData(const QString wellName,
             const IJKCellIndex& cellIndex);
  ~RICompData();
  RICompData(const RICompData& other);

  // ---------------------------------------------------------------
  static RICompData combine(const vector<RICompData>& completions);

  bool operator<(const RICompData& other) const;
  RICompData& operator=(const RICompData& other);

  // ---------------------------------------------------------------
  void setFromFracture(double transmissibility, double skinFactor);

  void setTransAndWPImultBackgroundDataFromPerforation(double transmissibility,
                                                       double skinFactor,
                                                       double diameter,
                                                       CellDir direction);

  void setCombinedValuesExplicitTrans(double transmissibility,
                                      CompletionType completionType);

  void setCombinedValuesImplicitTransWPImult(double wpimult,
                                             CellDir celldirection,
                                             double skinFactor,
                                             double wellDiameter,
                                             CompletionType completionType);

  // ---------------------------------------------------------------
  void addMetadata(const QString& name, const QString& comment);
  static bool isDefaultValue(double val);

  // ---------------------------------------------------------------
  const vector<RICompMetaData>& metadata() const { return m_metadata; }
  const QString& wellName() const { return m_wellName; }
  const IJKCellIndex& cellIndex() const { return m_cellIndex; }

  // ---------------------------------------------------------------
  WellConnectionState connectionState() const { return m_connectionState; }
  double saturation() const { return m_saturation; }
  double transmissibility() const { return m_transmissibility; }
  double diameter() const { return m_diameter; } //TODO: should be ft or m
  double kh() const { return m_kh; }
  double skinFactor() const { return m_skinFactor; }
  double dFactor() const { return m_dFactor; }

  // ---------------------------------------------------------------
  CellDir direction() const { return m_direction; }
  size_t count() const { return m_count; }
  double wpimult() const { return m_wpimult; }

  // ---------------------------------------------------------------
  CompletionType completionType() const { return m_completionType; }
  bool isMainBore() const { return m_isMainBore; }
  bool readyForExport() const { return m_readyForExport; }

  vector<RICompMetaData>   m_metadata;

 private:
  // ---------------------------------------------------------------
  QString m_wellName;
  IJKCellIndex m_cellIndex;
  WellConnectionState m_connectionState;

  // ---------------------------------------------------------------
  double m_saturation; //TODO: remove, always use default in Eclipse?
  double m_transmissibility;
  double m_diameter;
  double m_kh; //TODO: Remove, always use default in Eclipse?
  double m_skinFactor;
  double m_dFactor; //TODO: Remove, always use default in Eclipse?

  // ---------------------------------------------------------------
  CellDir       m_direction;
  bool m_isMainBore; //to use mainbore for Eclipse calculation
  bool m_readyForExport;

  // ---------------------------------------------------------------
  size_t m_count; //TODO: Remove, usage replaced by WPImult
  double m_wpimult;

  CompletionType m_completionType;

 private:
  // ---------------------------------------------------------------
  static bool onlyOneIsDefaulted(double first, double second);
  static void copy(RICompData& target, const RICompData& from);
};


}
}

#endif //FIELDOPT_WELL_PATH_H
