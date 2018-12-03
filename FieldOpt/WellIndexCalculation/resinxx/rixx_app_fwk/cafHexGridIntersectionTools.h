//##################################################################
//
// Custom Visualization Core library
// Copyright (C) 2011-2013 Ceetron AS
//
// This library may be used under the terms of either the GNU General
// Public License or the GNU Lesser General Public License as follows:
//
// GNU General Public License Usage
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// See the GNU General Public License at
// <<http://www.gnu.org/licenses/gpl.html>>
// for more details.
//
// GNU Lesser General Public License Usage
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1 of
// the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// See the GNU Lesser General Public License at
// <<http://www.gnu.org/licenses/lgpl-2.1.html>>
// for more details.
//
//##################################################################
//
// Modified by M.Bellout on 3/5/18.
//

// -----------------------------------------------------------------
#pragma once

// -----------------------------------------------------------------
// STD
#include <vector>

// RESINSIGHT: FWK/VIZFWK/LIBCORE\LIBGEOMETRY ----------------------
#include "../rixx_core_geom/cvfBase.h"
#include "../rixx_core_geom/cvfVector3.h"

// -----------------------------------------------------------------
namespace cvf {
class Plane;
};

// -----------------------------------------------------------------
namespace caf {

// ╦ ╦  ╔═╗  ═╗ ╦  ╔═╗  ╦═╗  ╦  ╔╦╗  ╦  ╔╗╔  ╔╦╗  ╔═╗  ╦═╗  ╔═╗  ╔═╗  ╔═╗
// ╠═╣  ║╣   ╔╩╦╝  ║ ╦  ╠╦╝  ║   ║║  ║  ║║║   ║   ║╣   ╠╦╝  ╚═╗  ║╣   ║
// ╩ ╩  ╚═╝  ╩ ╚═  ╚═╝  ╩╚═  ╩  ═╩╝  ╩  ╝╚╝   ╩   ╚═╝  ╩╚═  ╚═╝  ╚═╝  ╚═╝
//====================================================================
class HexGridIntersectionTools
{
 public:

  //------------------------------------------------------------------
  struct ClipVx
  {
    ClipVx();

    cvf::Vec3d  vx;

    // ---------------------------------------------------------------
    double      normDistFromEdgeVx1;
    size_t      clippedEdgeVx1Id;
    size_t      clippedEdgeVx2Id;

    // ---------------------------------------------------------------
    // Pointing to real vertices, or indices to ClipVx's
    // in the supplied triangle vertices array
    bool isVxIdsNative;

    // ---------------------------------------------------------------
    // Helper data to make it possible to track what set
    // of ClipVx's the indices is reffering to in case of
    // consecutive clips
    int derivedVxLevel;
  };

  // -----------------------------------------------------------------
  static bool planeLineIntersect(
      const cvf::Plane& plane,
      const cvf::Vec3d& a,
      const cvf::Vec3d& b,
      cvf::Vec3d* intersection,
      double* normalizedDistFromA);

  // -----------------------------------------------------------------
  static bool planeTriangleIntersection(
      const cvf::Plane& plane,
      const cvf::Vec3d& p1, size_t p1Id,
      const cvf::Vec3d& p2, size_t p2Id,
      const cvf::Vec3d& p3, size_t p3Id,
      ClipVx* newVx1, ClipVx* newVx2,
      bool* isMostVxesOnPositiveSide);

  // -----------------------------------------------------------------
  static void clipTrianglesBetweenTwoParallelPlanes(
      const std::vector<ClipVx>& triangleVxes,
      const std::vector<int>& cellFaceForEachTriangleEdge,
      const cvf::Plane& p1Plane,
      const cvf::Plane& p2Plane,
      std::vector<ClipVx>* clippedTriangleVxes,
      std::vector<int>* cellFaceForEachClippedTriangleEdge);

  // -----------------------------------------------------------------
  static cvf::Vec3d planeLineIntersectionForMC(
      const cvf::Plane& plane,
      const cvf::Vec3d& p1,
      const cvf::Vec3d& p2,
      double* normalizedDistFromP1);

  // -----------------------------------------------------------------
  static int planeHexIntersectionMC(
      const cvf::Plane& plane,
      const cvf::Vec3d cell[8],
      const size_t hexCornersIds[8],
      std::vector<ClipVx>* triangleVxes,
      std::vector<int>* cellFaceForEachTriangleEdge);

};

}; // namespace caf