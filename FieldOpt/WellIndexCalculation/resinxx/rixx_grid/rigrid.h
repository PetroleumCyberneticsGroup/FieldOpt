////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011-     Statoil ASA
// Copyright (C) 2013-     Ceetron Solutions AS
// Copyright (C) 2011-2012 Ceetron AS
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

#ifndef FIELDOPT_RIGRID_H
#define FIELDOPT_RIGRID_H

// -----------------------------------------------------------------
// STD
#include <string>
#include <vector>
#include <map>

// FIELDOPT --------------------------------------------------------
#include "../../../Reservoir/grid/eclgrid.h"

// RESINSIGHT: FWK/VIZFWK/LIBCORE\LIBGEOMETRY ----------------------
#include "../rixx_core_geom/cvfBoundingBox.h"
#include "../rixx_core_geom/cvfBoundingBoxTree.h"
#include "../rixx_core_geom/cvfCollection.h"

// -----------------------------------------------------------------
#include "ricell.h"
#include "rifaultncc.h"

// -----------------------------------------------------------------
class RIGrid;
class RICell;
class RILocalGrid;
class RIActiveCellInfo;

class RINNCData;
class RIFault;

// -----------------------------------------------------------------
using std::string;
using std::vector;

// ╦═╗  ╦  ╔═╗  ╦═╗  ╦  ╔╦╗  ╔╗   ╔═╗  ╔═╗  ╔═╗
// ╠╦╝  ║  ║ ╦  ╠╦╝  ║   ║║  ╠╩╗  ╠═╣  ╚═╗  ║╣
// ╩╚═  ╩  ╚═╝  ╩╚═  ╩  ═╩╝  ╚═╝  ╩ ╩  ╚═╝  ╚═╝
// =================================================================
class RIGridBase : public cvf::StructGridInterface
{
 public:
  // ---------------------------------------------------------------
  explicit RIGridBase(RIGrid* mainGrid);
  virtual ~RIGridBase(void);

  // ---------------------------------------------------------------
  void setGridPointDimensions(const cvf::Vec3st& gridDimensions)
  { m_gridPointDimensions = gridDimensions;}

  // ---------------------------------------------------------------
  cvf::Vec3st gridPointDimensions()
  { return m_gridPointDimensions; }

  // ---------------------------------------------------------------
  size_t cellCount() const
  { return cellCountI() * cellCountJ() * cellCountK(); }

  // ---------------------------------------------------------------
  RICell& cell(size_t gridLocalCellIndex);

  // ---------------------------------------------------------------
  const RICell& cell(size_t gridLocalCellIndex) const;

  size_t reservoirCellIndex(size_t gridLocalCellIndex) const;

  // ---------------------------------------------------------------
  void setIndexToStartOfCells(size_t indexToStartOfCells)
  { m_indexToStartOfCells = indexToStartOfCells; }

  // ---------------------------------------------------------------
  void setGridIndex(size_t index) { m_gridIndex = index; }

  size_t gridIndex() const { return m_gridIndex; }

  // ---------------------------------------------------------------
  void setGridId(int id) { m_gridId = id; }

  int gridId() const { return m_gridId; }

  // ---------------------------------------------------------------
  double characteristicIJCellSize() const;

  string gridName() const;
  void setGridName(const string& gridName);

  // ---------------------------------------------------------------
  bool isMainGrid() const;

  RIGrid* mainGrid() const { return m_mainGrid; }

  // ---------------------------------------------------------------
  cvf::BoundingBox boundingBox();

  // ---------------------------------------------------------------
  size_t addCoarseningBox(size_t i1, size_t i2, size_t j1, size_t j2, size_t k1, size_t k2);

 protected:
  // ---------------------------------------------------------------
  friend class RIGrid; //::initAllSubGridsParentGridPointer();
  void initSubGridParentPointer();
  void initSubCellsMainGridCellIndex();

  // Interface implementation
 public:
  // ---------------------------------------------------------------
  virtual size_t gridPointCountI() const;
  virtual size_t gridPointCountJ() const;
  virtual size_t gridPointCountK() const;

  // ---------------------------------------------------------------
  virtual cvf::Vec3d minCoordinate() const;
  virtual cvf::Vec3d maxCoordinate() const;
  virtual cvf::Vec3d displayModelOffset() const;

  // ---------------------------------------------------------------
  virtual size_t cellIndexFromIJK(
      size_t i, size_t j, size_t k ) const;

  // ---------------------------------------------------------------
  virtual bool ijkFromCellIndex(
      size_t cellIndex,
      size_t* i, size_t* j, size_t* k ) const;

  // ---------------------------------------------------------------
  virtual bool cellIJKFromCoordinate(
      const cvf::Vec3d& coord,
      size_t* i, size_t* j, size_t* k ) const;

  // ---------------------------------------------------------------
  virtual void cellCornerVertices(
      size_t cellIndex,
      cvf::Vec3d vertices[8] ) const;

  // ---------------------------------------------------------------
  virtual cvf::Vec3d cellCentroid(size_t cellIndex ) const;

  // ---------------------------------------------------------------
  virtual void cellMinMaxCordinates(
      size_t cellIndex,
      cvf::Vec3d* minCoordinate,
      cvf::Vec3d* maxCoordinate ) const;

  // ---------------------------------------------------------------
  virtual size_t gridPointIndexFromIJK(
      size_t i, size_t j, size_t k ) const;

  // ---------------------------------------------------------------
  virtual cvf::Vec3d gridPointCoordinate(
      size_t i, size_t j, size_t k ) const;

  // ---------------------------------------------------------------
  virtual bool isCellValid(
      size_t i, size_t j, size_t k ) const;

  // ---------------------------------------------------------------
  virtual bool cellIJKNeighbor(
      size_t i, size_t j, size_t k,
      FaceType face, size_t* neighborCellIndex ) const;

 private:
  // ---------------------------------------------------------------
  std::string m_gridName;

  // ---------------------------------------------------------------
  cvf::Vec3st m_gridPointDimensions;

  // ---------------------------------------------------------------
  ///< Index into the global cell array stored in
  ///< main-grid where this grids cells starts.
  size_t m_indexToStartOfCells;

  // ---------------------------------------------------------------
  ///< The LGR index of this grid. Starts with 1.
  ///< Main grid has index 0.
  size_t m_gridIndex;

  // ---------------------------------------------------------------
  ///< The LGR id of this grid. Main grid has id 0.
  int m_gridId;

  // ---------------------------------------------------------------
  RIGrid* m_mainGrid;

  cvf::BoundingBox m_boundingBox;

  // ---------------------------------------------------------------
  std::vector<caf::SizeTArray6> m_coarseningBoxInfo;

};

////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011-     Statoil ASA
// Copyright (C) 2013-     Ceetron Solutions AS
// Copyright (C) 2011-2012 Ceetron AS
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

// ╦═╗  ╦  ╔═╗  ╦═╗  ╦  ╔╦╗
// ╠╦╝  ║  ║ ╦  ╠╦╝  ║   ║║
// ╩╚═  ╩  ╚═╝  ╩╚═  ╩  ═╩╝
// =================================================================
class RIGrid : public RIGridBase
{
 public:
  RIGrid(string file_path);
  virtual ~RIGrid();

  // CELL ----------------------------------------------------------
  vector<cvf::Vec3d>& nodes() { return m_nodes; }
  const vector<cvf::Vec3d>& nodes() const { return m_nodes; }

  vector<RICell>& globalCellArray() { return m_cells; }
  const vector<RICell>& globalCellArray() const { return m_cells; }

//  const RICell&
//  cellByGridAndGridLocalCellIdx(size_t gridIdx,
//                                size_t gridLocalCellIdx) const;
//
//  size_t
//  reservoirCellIndexByGridAndGridLocalCellIndex(size_t gridIdx,
//                                                size_t gridLocalCellIdx) const;

  // GRID ----------------------------------------------------------
  void addLocalGrid(RILocalGrid* localGrid);

  size_t gridCount() const { return m_localGrids.size() + 1; }

  size_t gridCount() { return m_localGrids.size() + 1; }

  RIGridBase* gridByIndex(size_t localGridIndex);

  // ---------------------------------------------------------------
  const RIGridBase* gridByIndex(size_t localGridIndex) const;

  RIGridBase* gridById(int localGridId);

  // NNC -----------------------------------------------------------
  RINNCData* nncData();
  void setFaults(const cvf::Collection<RIFault>& faults);

  const cvf::Collection<RIFault>& faults() { return m_faults; }

  void calculateFaults(const RIActiveCellInfo* activeCellInfo);

  void distributeNNCsToFaults();

  const RIFault* findFaultFromCellIndexAndCellFace(
      size_t reservoirCellIndex,
      cvf::StructGridInterface::FaceType face) const;

  bool isFaceNormalsOutwards() const;

  // ---------------------------------------------------------------
  void computeCachedData();
  void initAllSubGridsParentGridPointer();

  // OVERRIDES -----------------------------------------------------
  virtual cvf::Vec3d displayModelOffset() const;

  void setDisplayModelOffset(cvf::Vec3d offset);

  void setFlipAxis(bool flipXAxis, bool flipYAxis);

  void findIntersectingCells(const cvf::BoundingBox& inputBB,
                             vector<size_t>* cellIndices) const;

  cvf::BoundingBox boundingBox() const;

  // RIADEFINES ----------------------------------------------------
  QString undefinedGridFaultName()
  { return "Undefined Grid Faults"; };

  QString undefinedGridFaultWithInactiveName()
  { return "Undefined Grid Faults With Inactive"; };

  // VARIABLES -----------------------------------------------------
 private:
  void initAllSubCellsMainGridCellIndex();
  void buildCellSearchTree();
  bool hasFaultWithName(const QString& name) const;

  // ---------------------------------------------------------------
  // Global vertex table
  vector<cvf::Vec3d> m_nodes;

  // ---------------------------------------------------------------
  // Global array of all cells in
  // reservoir (incl. the ones in LGR's)
  vector<RICell> m_cells;

  // ---------------------------------------------------------------
  ///< List of all the LGR's in this reservoir
  cvf::Collection<RILocalGrid> m_localGrids;

  // ---------------------------------------------------------------
  // Mapping from LGR Id to index.
  vector<size_t> m_gridIdToIndexMapping;

  cvf::Collection<RIFault> m_faults;

  // ---------------------------------------------------------------
  RINNCData* m_nncData;
  // cvf::ref<RINNCData> m_nncData;

  // ---------------------------------------------------------------
  // RIFaultsPrCellAccumulator* m_faultsPrCellAcc;
  cvf::ref<RIFaultsPrCellAccumulator> m_faultsPrCellAcc;

  // ---------------------------------------------------------------
  cvf::Vec3d m_displayModelOffset;
  cvf::ref<cvf::BoundingBoxTree> m_cellSearchTree;
  mutable cvf::BoundingBox m_boundingBox;

  bool m_flipXAxis;
  bool m_flipYAxis;
};

////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011-     Statoil ASA
// Copyright (C) 2013-     Ceetron Solutions AS
// Copyright (C) 2011-2012 Ceetron AS
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

// ╦═╗  ╦  ╔═╗  ╔═╗  ╔╦╗  ╦  ╦  ╦  ╔═╗  ╔═╗  ╔═╗  ╦    ╦    ╦  ╔╗╔  ╔═╗  ╔═╗
// ╠╦╝  ║  ╠═╣  ║     ║   ║  ╚╗╔╝  ║╣   ║    ║╣   ║    ║    ║  ║║║  ╠╣   ║ ║
// ╩╚═  ╩  ╩ ╩  ╚═╝   ╩   ╩   ╚╝   ╚═╝  ╚═╝  ╚═╝  ╩═╝  ╩═╝  ╩  ╝╚╝  ╚    ╚═╝
// =================================================================
class RIActiveCellInfo : public cvf::Object
{
 public:
  // ---------------------------------------------------------------
  RIActiveCellInfo();
  // ~RIActiveCellInfo(){}; // MB

  // ---------------------------------------------------------------
  void setReservoirCellCount(size_t reservoirCellCount);
  size_t reservoirCellCount() const;
  size_t reservoirActiveCellCount() const;
  size_t reservoirCellResultCount() const;
  bool isCoarseningActive() const;

  // ---------------------------------------------------------------
  bool isActive(size_t reservoirCellIndex) const;
  size_t cellResultIndex(size_t reservoirCellIndex) const;

  void setCellResultIndex(size_t reservoirCellIndex,
                          size_t globalResultCellIndex);

  // ---------------------------------------------------------------
  void setGridCount(size_t gridCount);

  void setGridActiveCellCounts(size_t gridIndex,
                               size_t activeCellCount);

  void gridActiveCellCounts(size_t gridIndex,
                            size_t& activeCellCount) const;

  // ---------------------------------------------------------------
  void computeDerivedData();

  void setIJKBoundingBox(const cvf::Vec3st& min,
                         const cvf::Vec3st& max);

  void IJKBoundingBox(cvf::Vec3st& min,
                      cvf::Vec3st& max) const;

  // ---------------------------------------------------------------
  cvf::BoundingBox geometryBoundingBox() const;

  void setGeometryBoundingBox(cvf::BoundingBox bb);

  // ---------------------------------------------------------------
  void clear();

 private:
  // ---------------------------------------------------------------
  class GridActiveCellCounts
  {
   public:
    GridActiveCellCounts();

    size_t activeCellCount() const;
    void setActiveCellCount(size_t activeCellCount);

   private:
    size_t m_activeCellCount;
  };


 private:
  vector<GridActiveCellCounts> m_perGridActiveCellInfo;

  vector<size_t> m_cellIndexToResultIndex;

  size_t m_reservoirActiveCellCount;
  size_t m_reservoirCellResultCount;

  cvf::Vec3st m_activeCellPositionMin;
  cvf::Vec3st m_activeCellPositionMax;

  cvf::BoundingBox m_activeCellsBoundingBox;
};

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
// <http://www.gnu.org/licenses/gpl.html> for more details.
//
////////////////////////////////////////////////////////////////////
//
// Modified by M.Bellout on 3/5/18.
//

// ╦═╗  ╦  ╦    ╔═╗  ╔═╗  ╔═╗  ╦    ╔═╗  ╦═╗  ╦  ╔╦╗
// ╠╦╝  ║  ║    ║ ║  ║    ╠═╣  ║    ║ ╦  ╠╦╝  ║   ║║
// ╩╚═  ╩  ╩═╝  ╚═╝  ╚═╝  ╩ ╩  ╩═╝  ╚═╝  ╩╚═  ╩  ═╩╝
// =================================================================
class RILocalGrid : public RIGridBase
{
 public:
  explicit RILocalGrid(RIGrid* mainGrid);
  virtual ~RILocalGrid();

  RIGridBase* parentGrid() const
  { return m_parentGrid; }

  void setParentGrid(RIGridBase * parentGrid)
  { m_parentGrid = parentGrid; }

  size_t positionInParentGrid() const
  { return m_positionInParentGrid; }

  void setPositionInParentGrid(size_t positionInParentGrid)
  { m_positionInParentGrid = positionInParentGrid; }

 private:
  RIGridBase* m_parentGrid;
  size_t m_positionInParentGrid;

};

#endif //FIELDOPT_RIGRID_H
