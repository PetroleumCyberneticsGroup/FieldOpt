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

// -----------------------------------------------------------------
#pragma once

// -----------------------------------------------------------------
// STD
#include <array>
#include <list>
#include <vector>

// RESINSIGHT: FWK/VIZFWK/LIBCORE\LIBGEOMETRY ----------------------
#include "../rixx_core_geom/cvfBase.h"
#include "../rixx_core_geom/cvfVector3.h"
#include "../rixx_core_geom/cvfBoundingBox.h"
#include "../rixx_core_geom/cvfPlane.h"

// -----------------------------------------------------------------
using std::vector;
using std::list;
using std::pair;
using std::array;

// ╦═╗  ╦  ╔═╗  ╔═╗  ╔═╗  ╦    ╦    ╔═╗  ╔═╗  ╔═╗  ╔╦╗  ╔═╗  ╔╦╗  ╦═╗  ╦ ╦
// ╠╦╝  ║  ║ ╦  ║    ║╣   ║    ║    ║ ╦  ║╣   ║ ║  ║║║  ║╣    ║   ╠╦╝  ╚╦╝
// ╩╚═  ╩  ╚═╝  ╚═╝  ╚═╝  ╩═╝  ╩═╝  ╚═╝  ╚═╝  ╚═╝  ╩ ╩  ╚═╝   ╩   ╩╚═   ╩
// =================================================================
class RigCellGeometryTools
{
 public:

  static void
  createPolygonFromLineSegments(
      list<pair<cvf::Vec3d, cvf::Vec3d>> &intersectionLineSegments,
      vector<vector<cvf::Vec3d>> &polygons);

  static void findCellLocalXYZ(const array<cvf::Vec3d, 8>& hexCorners,
                               cvf::Vec3d &localXdirection,
                               cvf::Vec3d &localYdirection,
                               cvf::Vec3d &localZdirection);

  static double
  polygonLengthInLocalXdirWeightedByArea(vector<cvf::Vec3d> polygon2d);

  static vector<vector<cvf::Vec3d> >
  intersectPolygons(vector<cvf::Vec3d> polygon1,
                    vector<cvf::Vec3d> polygon2);

  enum ZInterpolationType { INTERPOLATE_LINE_Z, USE_HUGEVAL, USE_ZERO};

  static vector<vector<cvf::Vec3d> >
  clipPolylineByPolygon(const vector<cvf::Vec3d>& polyLine,
                        const vector<cvf::Vec3d>& polygon,
                        ZInterpolationType interpolType = USE_ZERO);

  static pair<cvf::Vec3d, cvf::Vec3d>
  getLineThroughBoundingBox(cvf::Vec3d lineDirection,
                            cvf::BoundingBox polygonBBox,
                            cvf::Vec3d pointOnLine);

  static double
  getLengthOfPolygonAlongLine(const pair<cvf::Vec3d, cvf::Vec3d>& line,
                              const vector<cvf::Vec3d>& polygon);

 private:
  static vector<cvf::Vec3d>
  ajustPolygonToAvoidIntersectionsAtVertex(const vector<cvf::Vec3d>& polyLine,
                                           const vector<cvf::Vec3d>& polygon);

};
