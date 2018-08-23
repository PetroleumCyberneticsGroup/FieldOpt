////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2017     Statoil ASA
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
// <http://www.gnu.org/licenses/gpl.html> for more details.
//
////////////////////////////////////////////////////////////////////
//
// Modified by M.Bellout on 3/5/18.
//

#ifndef FIELDOPT_GEOMETRY_TOOLS_H
#define FIELDOPT_GEOMETRY_TOOLS_H

// RESINSIGHT: FWK/VIZFWK/LIBCORE\LIBGEOMETRY ----------------------
#include "rixx_core_geom/cvfBase.h"
#include "rixx_core_geom/cvfRay.h"
#include "rixx_core_geom/cvfPlane.h"
#include "rixx_core_geom/cvfVector3.h"
#include "rixx_core_geom/cvfMatrix4.h"

// RESINSIGHT: FWK/APPFWK/COMMONCODE\VIZEXT\PROJDATAMOD ------------
#include "rixx_app_fwk/cvfStructGrid.h"
#include "rixx_app_fwk/cafHexGridIntersectionTools.h"

// RESINSIGHT: APPLICATIONCODE/RESERVOIRDATAMODEL ------------------
#include "rixx_res_mod/cvfGeometryTools.h"
#include "rixx_res_mod/RigCellGeometryTools.h"

// FieldOpt::RESINXX -----------------------------------------------
//#include "well_path.h"

// -----------------------------------------------------------------
namespace cvf {

// -----------------------------------------------------------------
using std::vector;
using std::array;
using std::set;
using std::list;
using std::pair;

// =================================================================
// Internal class for intersection point info
struct HexIntersectionInfo {

 public:
  HexIntersectionInfo(cvf::Vec3d intersectionPoint,
                      bool isIntersectionEntering,
                      cvf::StructGridInterface::FaceType face,
                      size_t hexIndex)
      : m_intersectionPoint(intersectionPoint),
        m_isIntersectionEntering(isIntersectionEntering),
        m_face(face),
        m_hexIndex(hexIndex) {}

  cvf::Vec3d m_intersectionPoint;
  bool m_isIntersectionEntering;
  cvf::StructGridInterface::FaceType m_face;
  size_t m_hexIndex;
};

// =================================================================
bool operator<(const HexIntersectionInfo& hi1,
               const HexIntersectionInfo& hi2);

// =================================================================
// SAVE FOR LATER
// Specialized Line - Hex intersection
struct RigHexIntersectionTools {

  static int lineHexCellIntersection(const cvf::Vec3d p1,
                                     const cvf::Vec3d p2,
                                     const cvf::Vec3d hexCorners[8],
                                     const size_t hexIndex,
                                     vector<HexIntersectionInfo> *intersections);

  static bool isPointInCell(const cvf::Vec3d point,
                            const cvf::Vec3d hexCorners[8]);

  static bool planeHexCellIntersection(
      cvf::Vec3d *hexCorners,
      cvf::Plane fracturePlane,
      list<pair<cvf::Vec3d, cvf::Vec3d > > &intersectionLineSegments);

  static bool planeHexIntersectionPolygons(array<cvf::Vec3d, 8> hexCorners,
                                           cvf::Mat4d transformMatrixForPlane,
                                           vector<vector<cvf::Vec3d > > &polygons);
};

}

#endif //FIELDOPT_GEOMETRY_TOOLS_H
