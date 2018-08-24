////////////////////////////////////////////////////////////
//
// Copyright (C) 2011-     Statoil ASA
// Copyright (C) 2013-     Ceetron Solutions AS
// Copyright (C) 2011-2012 Ceetron AS
//
// ResInsight is free software: you can redistribute it
// and/or modify it under the terms of the GNU General
// Public License
// as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any
// later version.
//
// ResInsight is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.
//
// See the GNU General Public License at
// <http://www.gnu.org/licenses/gpl.html>
// for more details.
//
////////////////////////////////////////////////////////////
//
// Modified by M.Bellout on 3/5/18.
//

// ---------------------------------------------------------
// STD
#include <string>

// ---------------------------------------------------------
#include "rigrid.h"

// ---------------------------------------------------------
using std::string;

//==========================================================
// ╦═╗  ╦  ╔═╗  ╦═╗  ╦  ╔╦╗  ╔╗   ╔═╗  ╔═╗  ╔═╗
// ╠╦╝  ║  ║ ╦  ╠╦╝  ║   ║║  ╠╩╗  ╠═╣  ╚═╗  ║╣
// ╩╚═  ╩  ╚═╝  ╩╚═  ╩  ═╩╝  ╚═╝  ╩ ╩  ╚═╝  ╚═╝
// =========================================================
RIGridBase::RIGridBase(RIGrid* mainGrid):
    m_gridPointDimensions(0,0,0),
    m_mainGrid(mainGrid),
    m_indexToStartOfCells(0) {

  // -------------------------------------------------------
  if (mainGrid == nullptr) {
    m_gridIndex = 0;
    m_gridId    = 0;

  } else {
    m_gridIndex = cvf::UNDEFINED_SIZE_T;
    m_gridId = cvf::UNDEFINED_INT;
  }
}

// =========================================================
RIGridBase::~RIGridBase(void) {
  // cout << "[wic-rixx]deleting vars.----- RIGridBase" << endl;
}

// =========================================================
void RIGridBase::setGridName(const string& gridName) {
  m_gridName = gridName;
}

// =========================================================
string RIGridBase::gridName() const {
  return m_gridName;
}

// =========================================================
RICell& RIGridBase::cell(size_t gridLocalCellIndex) {

  CVF_ASSERT(m_mainGrid);
  CVF_ASSERT(m_indexToStartOfCells + gridLocalCellIndex
                 < m_mainGrid->globalCellArray().size());

  // -------------------------------------------------------
  return m_mainGrid->
      globalCellArray()[m_indexToStartOfCells + gridLocalCellIndex];
}

// =========================================================
const RICell& RIGridBase::cell(size_t gridLocalCellIndex) const {

  CVF_ASSERT(m_mainGrid);

  // -------------------------------------------------------
  return m_mainGrid->
      globalCellArray()[m_indexToStartOfCells + gridLocalCellIndex];
}

// =========================================================
void RIGridBase::initSubGridParentPointer() {

  RIGridBase* grid = this;

  // -------------------------------------------------------
  size_t cellIdx;
  for (cellIdx = 0; cellIdx < grid->cellCount(); ++cellIdx) {
    RICell& cell = grid->cell(cellIdx);

    if (cell.subGrid()) {
      cell.subGrid()->setParentGrid(grid);
    }
  }
}

// =========================================================
// Find the cell index to the maingrid cell containing this
// cell, and store it as m_mainGridCellIndex in each cell.
void RIGridBase::initSubCellsMainGridCellIndex() {

  RIGridBase* grid = this;

  // -------------------------------------------------------
  if (grid->isMainGrid()) {

    size_t cellIdx;
    // -----------------------------------------------------
    for (cellIdx = 0; cellIdx < grid->cellCount(); ++cellIdx) {
      RICell& cell = grid->cell(cellIdx);
      cell.setMainGridCellIndex(cellIdx);
    }

  } else {

    size_t cellIdx;
    // -----------------------------------------------------
    for (cellIdx = 0; cellIdx < grid->cellCount(); ++cellIdx) {
      RILocalGrid* localGrid = static_cast<RILocalGrid*>(grid);
      RIGridBase* parentGrid = localGrid->parentGrid();

      // ---------------------------------------------------
      RICell& cell = localGrid->cell(cellIdx);
      size_t parentCellIndex = cell.parentCellIndex();

      // ---------------------------------------------------
      while (!parentGrid->isMainGrid()) {

        const RICell& parentCell = parentGrid->cell(parentCellIndex);
        parentCellIndex = parentCell.parentCellIndex();

        localGrid = static_cast<RILocalGrid*>(parentGrid);
        parentGrid = localGrid->parentGrid();

      }

      cell.setMainGridCellIndex(parentCellIndex);
    }
  }
}

// =========================================================
/// For main grid, this will work with reservoirCellIndices
/// retreiving the correct lgr cells as well. The cell() call
/// retreives correct cell, because main grid has offset of 0,
/// and we access the global cell array in main grid.
void
RIGridBase::cellCornerVertices(size_t cellIndex,
                               cvf::Vec3d vertices[8]) const {

  const caf::SizeTArray8& indices = cell(cellIndex).cornerIndices();

  vertices[0].set(m_mainGrid->nodes()[indices[0]]);
  vertices[1].set(m_mainGrid->nodes()[indices[1]]);
  vertices[2].set(m_mainGrid->nodes()[indices[2]]);
  vertices[3].set(m_mainGrid->nodes()[indices[3]]);
  vertices[4].set(m_mainGrid->nodes()[indices[4]]);
  vertices[5].set(m_mainGrid->nodes()[indices[5]]);
  vertices[6].set(m_mainGrid->nodes()[indices[6]]);
  vertices[7].set(m_mainGrid->nodes()[indices[7]]);
}

// =========================================================
size_t
RIGridBase::cellIndexFromIJK(size_t i,
                             size_t j,
                             size_t k) const {

  CVF_TIGHT_ASSERT(i != cvf::UNDEFINED_SIZE_T
                       && j != cvf::UNDEFINED_SIZE_T
                       && k != cvf::UNDEFINED_SIZE_T );
  CVF_TIGHT_ASSERT(i < m_gridPointDimensions.x()
                       && j < m_gridPointDimensions.y()
                       && k < m_gridPointDimensions.z()  );

  size_t ci = i +
      j*(m_gridPointDimensions.x() - 1) +
      k*((m_gridPointDimensions.x() - 1)*(m_gridPointDimensions.y() - 1));

  return ci;
}

// =========================================================
void
RIGridBase::cellMinMaxCordinates(size_t cellIndex,
                                 cvf::Vec3d* minCoordinate,
                                 cvf::Vec3d* maxCoordinate) const {
}

// =========================================================
bool
RIGridBase::ijkFromCellIndex(size_t cellIndex,
                             size_t* i,
                             size_t* j,
                             size_t* k) const {

  // -------------------------------------------------------
  CVF_TIGHT_ASSERT(cellIndex < cellCount());

  size_t index = cellIndex;

  // -------------------------------------------------------
  if (cellCountI() <= 0 || cellCountJ() <= 0) {
    return false;
  }

  // -------------------------------------------------------
  *k = index/(cellCountI()*cellCountJ());
  index -= (*k)*(cellCountI()*cellCountJ());

  // -------------------------------------------------------
  *j = index/cellCountI();
  index -= (*j)*cellCountI();

  // -------------------------------------------------------
  *i = index;

  return true;
}

// =========================================================
size_t RIGridBase::gridPointIndexFromIJK(size_t i,
                                         size_t j,
                                         size_t k) const {
  return 0;
}

// =========================================================
bool RIGridBase::cellIJKFromCoordinate(
    const cvf::Vec3d& coord,
    size_t* i, size_t* j, size_t* k) const {

  return false;
}

// =========================================================
cvf::Vec3d RIGridBase::gridPointCoordinate(size_t i,
                                           size_t j,
                                           size_t k) const {
  cvf::Vec3d pos;
  return pos;
}

// =========================================================
cvf::Vec3d RIGridBase::minCoordinate() const {
  cvf::Vec3d v;
  return v;
}

// =========================================================
size_t RIGridBase::gridPointCountI() const {
  return m_gridPointDimensions.x();
}

// =========================================================
size_t RIGridBase::gridPointCountJ() const {
  return m_gridPointDimensions.y();
}

// =========================================================
size_t RIGridBase::gridPointCountK() const {
  return m_gridPointDimensions.z();
}

// =========================================================
cvf::Vec3d RIGridBase::cellCentroid(size_t cellIndex) const {
  cvf::Vec3d v;
  return v;
}

// =========================================================
cvf::Vec3d RIGridBase::maxCoordinate() const {
  cvf::Vec3d v;
  return v;
}

// =========================================================
bool RIGridBase::isCellValid(size_t i,
                             size_t j,
                             size_t k) const {

  // -------------------------------------------------------
  if (i >= cellCountI()
      || j >= cellCountJ()
      || k >= cellCountK()) {
    return false;
  }

  // -------------------------------------------------------
  size_t idx = cellIndexFromIJK(i, j, k);
  const RICell& c = cell(idx);
  return !c.isInvalid();
}

// =========================================================
bool
RIGridBase::cellIJKNeighbor(size_t i, size_t j, size_t k,
                            FaceType face,
                            size_t* neighborCellIndex) const {

  // -------------------------------------------------------
  size_t ni, nj, nk;
  neighborIJKAtCellFace(i, j, k, face, &ni, &nj, &nk);

  // -------------------------------------------------------
  if (!isCellValid(ni, nj, nk)) {
    return false;
  }

  // -------------------------------------------------------
  if (neighborCellIndex) {
    *neighborCellIndex = cellIndexFromIJK(ni, nj, nk);
  }

  return true;
}

// =========================================================
bool RIGridBase::isMainGrid() const {
  return this == m_mainGrid;
}

// =========================================================
// Models with large absolute values for coordinate scalars
// will often end up with z-fighting due to numerical limits
// in float used by OpenGL to represent a position.
// displayModelOffset() is intended to be subtracted
// from domain model coordinate when building geometry
// for visualization
//
//  Vec3d domainModelCoord
//  Vec3d coordForVisualization
cvf::Vec3d RIGridBase::displayModelOffset() const {
  return m_mainGrid->displayModelOffset();
}

// =========================================================
// Returns min size of the I and J characteristic cell sizes
double RIGridBase::characteristicIJCellSize() const {

  // -------------------------------------------------------
  double characteristicCellSize = HUGE_VAL;

  // -------------------------------------------------------
  double cellSizeI, cellSizeJ, cellSizeK;

  this->characteristicCellSizes(&cellSizeI,
                                &cellSizeJ,
                                &cellSizeK);

  // -------------------------------------------------------
  if (cellSizeI < characteristicCellSize)
    characteristicCellSize = cellSizeI;

  if (cellSizeJ < characteristicCellSize)
    characteristicCellSize = cellSizeJ;

  return characteristicCellSize;
}

// =========================================================
size_t
RIGridBase::reservoirCellIndex(
    size_t gridLocalCellIndex) const {
  return m_indexToStartOfCells + gridLocalCellIndex;
}

// =========================================================
cvf::BoundingBox RIGridBase::boundingBox() {

  // -------------------------------------------------------
  if (!m_boundingBox.isValid()) {
    cvf::Vec3d cornerVerts[8];

    // -----------------------------------------------------
    for (size_t i = 0; i < cellCount(); i++) {
      cellCornerVertices(i, cornerVerts);

      // ---------------------------------------------------
      for (size_t j = 0; j < 8; j++) {
        m_boundingBox.add(cornerVerts[j]);
      }
    }
  }

  return m_boundingBox;
}

// =========================================================
size_t
RIGridBase::addCoarseningBox(size_t i1, size_t i2, size_t j1,
                             size_t j2, size_t k1, size_t k2) {

  // -------------------------------------------------------
  caf::SizeTArray6 box;
  box[0] = i1;
  box[1] = i2;
  box[2] = j1;
  box[3] = j2;
  box[4] = k1;
  box[5] = k2;

  // -------------------------------------------------------
  m_coarseningBoxInfo.push_back(box);

  size_t coarseningBoxIndex = m_coarseningBoxInfo.size() - 1;

  // -------------------------------------------------------
  for (size_t k = k1; k <= k2; k++) {

    for (size_t j = j1; j <= j2; j++) {

      for (size_t i = i1; i <= i2; i++) {

        size_t cellIdx = this->cellIndexFromIJK(i, j, k);

        RICell& c = this->cell(cellIdx);
        CVF_ASSERT(c.coarseningBoxIndex() == cvf::UNDEFINED_SIZE_T);

        c.setCoarseningBoxIndex(coarseningBoxIndex);
      }
    }
  }

  return coarseningBoxIndex;
}


////////////////////////////////////////////////////////////
//
// Copyright (C) 2011-     Statoil ASA
// Copyright (C) 2013-     Ceetron Solutions AS
// Copyright (C) 2011-2012 Ceetron AS
//
// ResInsight is free software: you can redistribute it
// and/or modify it under the terms of the GNU General
// Public License
// as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any
// later version.
//
// ResInsight is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.
//
// See the GNU General Public License at
// <http://www.gnu.org/licenses/gpl.html>
// for more details.
//
////////////////////////////////////////////////////////////
//
// Modified by M.Bellout on 3/5/18.
//

// ╦═╗  ╦  ╔═╗  ╦═╗  ╦  ╔╦╗
// ╠╦╝  ║  ║ ╦  ╠╦╝  ║   ║║
// ╩╚═  ╩  ╚═╝  ╩╚═  ╩  ═╩╝
// =================================================================
RIGrid::RIGrid(string file_path)
    : RIGridBase(this), ECLGrid(file_path) {
  m_displayModelOffset = cvf::Vec3d::ZERO;
  m_gridIndex = 0;
  m_gridId = 0;
  m_gridIdToIndexMapping.push_back(0);

  m_flipXAxis = false;
  m_flipYAxis = false;

}


// =========================================================
RIGrid::~RIGrid() {

  // -------------------------------------------------------
//#ifdef FIELDOPT_BUILD

  // -------------------------------------------------------
  // Should be reverted to smart pointers
  // delete m_nncData;

  for(int i=0; i < m_faults.size(); ++i) {

    if (m_faults.at(i) != NULL) {

      printf("%s%s%s", FRED, "!m_faults.at(i) != nullptr ", AEND);
      delete m_faults.at(i); // ?
      printf("%s%s%s\n", FRED, "TRUE (~RIGrid)", AEND);

    } else {

      printf("%s%s%s\n", FRED, "!m_faults.at(i) != nullptr FALSE (~RIGrid)", AEND);

    }

  }

  // -------------------------------------------------------
  for(int i=0; i < m_localGrids.size(); ++i) {

    if (m_localGrids.at(i) != NULL) {

      printf("%s%s%s", FRED, "!m_localGrids.at(i) != nullptr ", AEND);
      delete m_localGrids.at(i);
      printf("%s%s%s\n", FRED, "TRUE (~RIGrid)", AEND);

    } else {

      printf("%s%s%s\n", FRED, "!m_localGrids.at(i) != nullptr FALSE (~RIGrid)", AEND);

    }

  }

  printf("%s%s%s\n", FRED, "FIELDOPT_BUILD (~RIGrid)", AEND);

//#elif ADGPRS_LIB_BUILD
//
//  printf("%s%s%s\n", FRED, "ADGPRS_BUILD (~RIGrid)", AEND);
//
//#endif

  // -------------------------------------------------------
  cout << "[wic-rixx]deleting vars.----- RIGrid" << endl;

}

// =========================================================
void RIGrid::addLocalGrid(RILocalGrid* localGrid) {

  // -------------------------------------------------------
  // The grid ID must be set.
  CVF_ASSERT(localGrid && localGrid->gridId() != cvf::UNDEFINED_INT);
  // We cant handle negative ID's if they exist.
  CVF_ASSERT(localGrid->gridId() >= 0);

  // -------------------------------------------------------
  // Maingrid itself has grid index 0
  m_localGrids.push_back(localGrid);
  localGrid->setGridIndex(m_localGrids.size());

  // -------------------------------------------------------
  if (m_gridIdToIndexMapping.size() <= static_cast<size_t>(localGrid->gridId())) {
    m_gridIdToIndexMapping.resize(localGrid->gridId() + 1,
                                  cvf::UNDEFINED_SIZE_T);
  }

  // -------------------------------------------------------
  m_gridIdToIndexMapping[localGrid->gridId()] = localGrid->gridIndex();
}

// =========================================================
void RIGrid::initAllSubGridsParentGridPointer() {

  // -------------------------------------------------------
  if (m_localGrids.size()
      && m_localGrids[0]->parentGrid() == nullptr ) {
    initSubGridParentPointer();
    size_t i;

    // -----------------------------------------------------
    for ( i = 0; i < m_localGrids.size(); ++i ) {
      m_localGrids[i]->initSubGridParentPointer();
    }
  }
}

// =========================================================
void RIGrid::initAllSubCellsMainGridCellIndex() {

  // -------------------------------------------------------
  initSubCellsMainGridCellIndex();
  size_t i;

  // -------------------------------------------------------
  for (i = 0; i < m_localGrids.size(); ++i) {
    m_localGrids[i]->initSubCellsMainGridCellIndex();
  }
}

// =========================================================
cvf::Vec3d RIGrid::displayModelOffset() const {
  return m_displayModelOffset;
}

// =========================================================
void RIGrid::setDisplayModelOffset(cvf::Vec3d offset) {
  m_displayModelOffset = offset;
}

// =========================================================
// Initialize pointers from grid to parent grid
// Compute cell ranges for active and valid cells
// Compute bounding box in world coordinates based
// on node coordinates
void RIGrid::computeCachedData() {

  // -------------------------------------------------------
  initAllSubGridsParentGridPointer();
  initAllSubCellsMainGridCellIndex();

  buildCellSearchTree();

  // -------------------------------------------------------
  cout << FLGREEN
       << "[wic-rixx]computeCachedData-- (ricasedata.cpp)"
       << AEND << endl;
}

// =========================================================
// Returns the grid with index \a localGridIndex.
// Main Grid itself has index 0. First LGR starts on 1
RIGridBase* RIGrid::gridByIndex(size_t localGridIndex) {

  if (localGridIndex == 0) return this;
  CVF_ASSERT(localGridIndex - 1 < m_localGrids.size()) ;
  return m_localGrids[localGridIndex-1].p();
}

// =========================================================
// Returns the grid with index \a localGridIndex.
// Main Grid itself has index 0. First LGR starts on 1
const RIGridBase* RIGrid::gridByIndex(
    size_t localGridIndex) const {

  if (localGridIndex == 0) return this;
  CVF_ASSERT(localGridIndex - 1 < m_localGrids.size()) ;
  return m_localGrids[localGridIndex-1].p();
}

// =========================================================
void RIGrid::setFlipAxis(bool flipXAxis, bool flipYAxis) {

  bool needFlipX = false;
  bool needFlipY = false;

  if (m_flipXAxis != flipXAxis) {
    needFlipX = true;
  }

  if (m_flipYAxis != flipYAxis) {
    needFlipY = true;
  }

  if (needFlipX || needFlipY) {
    for (size_t i = 0; i < m_nodes.size(); i++) {
      if (needFlipX) {
        m_nodes[i].x() *= -1.0;
      }

      if (needFlipY) {
        m_nodes[i].y() *= -1.0;
      }
    }

    m_flipXAxis = flipXAxis;
    m_flipYAxis = flipYAxis;
  }
}

// =========================================================
RIGridBase* RIGrid::gridById(int localGridId) {
  CVF_ASSERT (localGridId >= 0 &&
      static_cast<size_t>(localGridId) < m_gridIdToIndexMapping.size());
  return this->gridByIndex(m_gridIdToIndexMapping[localGridId]);
}

// =========================================================
RINNCData* RIGrid::nncData() {

//  if (m_nncData.isNull()) {
  if (m_nncData == NULL) {
    m_nncData = new RINNCData;
  }

  return m_nncData;
}

// =========================================================
void RIGrid::setFaults(const cvf::Collection<RIFault>& faults) {
  m_faults = faults;

#pragma omp parallel for
  for (int i = 0; i < static_cast<int>(m_faults.size()); i++) {
    m_faults[i]->computeFaultFacesFromCellRanges(this->mainGrid());
  }
}

// =========================================================
bool RIGrid::hasFaultWithName(const QString& name) const {

  for (auto fault : m_faults) {
    if (fault->name() == name) {
      return true;
    }
  }
  return false;
}

// =========================================================
void
RIGrid::calculateFaults(const RIActiveCellInfo* activeCellInfo) {

  // ---------------------------------------------------------------
  if (hasFaultWithName(undefinedGridFaultName())
      && hasFaultWithName(undefinedGridFaultWithInactiveName())) {
    //RiaLogging::debug(QString("Calculate faults already run for grid."));
    return;
  }

  // ---------------------------------------------------------------
  cout << FLGREEN
       << "[wic-rixx]RIFaultsPrCellAccm- (ricasedata.cpp)"
       << AEND << endl;
  m_faultsPrCellAcc = new RIFaultsPrCellAccumulator(m_cells.size());

  // ---------------------------------------------------------------
  // Spread fault idx'es on the cells from the faults
  for (size_t fIdx = 0 ; fIdx < m_faults.size(); ++fIdx) {
    m_faults[fIdx]->accumulateFaultsPrCell(m_faultsPrCellAcc.p(),
                                           static_cast<int>(fIdx));
  }
  // ---------------------------------------------------------------
  // Find the geometrical faults that is in addition: Has no user
  // defined (eclipse) fault assigned. Separate the grid faults that
  // has an inactive cell as member.
  RIFault* unNamedFault = new RIFault;
  unNamedFault->setName(undefinedGridFaultName());

  // ---------------------------------------------------------------
  int unNamedFaultIdx = static_cast<int>(m_faults.size());
  m_faults.push_back(unNamedFault);

  // ---------------------------------------------------------------
  RIFault* unNamedFaultWithInactive = new RIFault;
  unNamedFaultWithInactive->setName(undefinedGridFaultWithInactiveName());

  // ---------------------------------------------------------------
  int unNamedFaultWithInactiveIdx = static_cast<int>(m_faults.size());
  m_faults.push_back(unNamedFaultWithInactive);

  // ---------------------------------------------------------------
  const vector<cvf::Vec3d>& vxs = m_mainGrid->nodes();

  for (int gcIdx = 0 ; gcIdx < static_cast<int>(m_cells.size()); ++gcIdx) {
    if ( m_cells[gcIdx].isInvalid()) {
      continue;
    }

    // -------------------------------------------------------------
    size_t neighborReservoirCellIdx;
    size_t neighborGridCellIdx;
    size_t i, j, k;
    RIGridBase* hostGrid = NULL;
    bool firstNO_FAULTFaceForCell = true;
    bool isCellActive = true;

    // -------------------------------------------------------------
    char upperLimitForFaceType = cvf::StructGridInterface::FaceType::POS_K;

    // -------------------------------------------------------------
    // Compare only I and J faces
    for (char faceIdx = 0; faceIdx < upperLimitForFaceType; ++faceIdx) {
      cvf::StructGridInterface::FaceType face =
          cvf::StructGridInterface::FaceType(faceIdx);

      // -----------------------------------------------------------
      // For faces that has no used defined Fault assigned:
      if (m_faultsPrCellAcc->faultIdx(gcIdx, face) ==
          RIFaultsPrCellAccumulator::NO_FAULT) {

        // ---------------------------------------------------------
        // Find neighbor cell
        if (firstNO_FAULTFaceForCell) {
          // To avoid doing this for every face,
          // and only when detecting a NO_FAULT

          // -------------------------------------------------------
          hostGrid = m_cells[gcIdx].hostGrid();
          hostGrid->ijkFromCellIndex(m_cells[gcIdx].gridLocalCellIndex(), &i,&j, &k);
          isCellActive = activeCellInfo->isActive(gcIdx);

          firstNO_FAULTFaceForCell = false;
        }

        // ---------------------------------------------------------
        // If:
        if(!hostGrid->cellIJKNeighbor(i, j, k, face,
                                      &neighborGridCellIdx)) {
          continue;
        }

        // ---------------------------------------------------------
        // If:
        neighborReservoirCellIdx =
            hostGrid->reservoirCellIndex(neighborGridCellIdx);
        if (m_cells[neighborReservoirCellIdx].isInvalid()) {
          continue;
        }

        // ---------------------------------------------------------
        bool isNeighborCellActive =
            activeCellInfo->isActive(neighborReservoirCellIdx);

        double tolerance = 1e-6;

        // ---------------------------------------------------------
        caf::SizeTArray4 faceIdxs;
        m_cells[gcIdx].faceIndices(face, &faceIdxs);
        caf::SizeTArray4 nbFaceIdxs;
        m_cells[neighborReservoirCellIdx].faceIndices(
            cvf::StructGridInterface::oppositeFace(face), &nbFaceIdxs);

        // ---------------------------------------------------------
        bool sharedFaceVertices = true;
        if (sharedFaceVertices &&
            vxs[faceIdxs[0]].pointDistance(vxs[nbFaceIdxs[0]]) > tolerance )
          sharedFaceVertices = false;

        // ---------------------------------------------------------
        if (sharedFaceVertices &&
            vxs[faceIdxs[1]].pointDistance(vxs[nbFaceIdxs[3]]) > tolerance )
          sharedFaceVertices = false;

        // ---------------------------------------------------------
        if (sharedFaceVertices &&
            vxs[faceIdxs[2]].pointDistance(vxs[nbFaceIdxs[2]]) > tolerance )
          sharedFaceVertices = false;

        // ---------------------------------------------------------
        if (sharedFaceVertices &&
            vxs[faceIdxs[3]].pointDistance(vxs[nbFaceIdxs[1]]) > tolerance )
          sharedFaceVertices = false;

        // ---------------------------------------------------------
        if (sharedFaceVertices) {
          continue;
        }

        // ---------------------------------------------------------
        // To avoid doing this calculation for the opposite face
        int faultIdx = unNamedFaultIdx;
        if (!(isCellActive && isNeighborCellActive)) {
          faultIdx = unNamedFaultWithInactiveIdx;
        }

        // ---------------------------------------------------------
        m_faultsPrCellAcc->setFaultIdx(gcIdx, face, faultIdx);
        m_faultsPrCellAcc->setFaultIdx(neighborReservoirCellIdx,
                                       cvf::StructGridInterface::oppositeFace(face),
                                       faultIdx);

        // ---------------------------------------------------------
        // Add as fault face only if grid index is less than the neighbors
        if (static_cast<size_t>(gcIdx) < neighborReservoirCellIdx) {

          // -------------------------------------------------------
          RIFault::FaultFace ff(gcIdx,
                                cvf::StructGridInterface::FaceType(faceIdx),
                                neighborReservoirCellIdx);

          // -------------------------------------------------------
          if(isCellActive && isNeighborCellActive) {
            unNamedFault->faultFaces().push_back(ff);

          } else {

            unNamedFaultWithInactive->faultFaces().push_back(ff);
          }

          // -------------------------------------------------------
        } else {
          // Should never occur. because we flag the opposite face
          // in the faultsPrCellAcc
          //
          CVF_FAIL_MSG("Found fault with global neighbor index "
                           "less than the native index. ");
        }
      }
    }
  }

  // ---------------------------------------------------------------
  // distributeNNCsToFaults();
}

// =========================================================
void RIGrid::distributeNNCsToFaults() {

  // ---------------------------------------------------------------
  cout << FLGREEN
       << "[wic-rixx]distribNNCsToFaults (rigrid.cpp)"
       << AEND << endl;

  // ---------------------------------------------------------------
  const std::vector<RIConnection>&
      nncs = this->nncData()->connections();

  // ---------------------------------------------------------------
  for (size_t nncIdx = 0; nncIdx < nncs.size(); ++nncIdx) {
    // Find the fault for each side of the nnc
    const RIConnection& conn = nncs[nncIdx];
    int fIdx1 = RIFaultsPrCellAccumulator::NO_FAULT;
    int fIdx2 = RIFaultsPrCellAccumulator::NO_FAULT;

    // ---------------------------------------------------------------
    if (conn.m_c1Face != cvf::StructGridInterface::NO_FACE) {

      // -------------------------------------------------------------
      fIdx1 = m_faultsPrCellAcc->faultIdx(
          conn.m_c1GlobIdx,
          conn.m_c1Face);

      // -------------------------------------------------------------
      fIdx2 = m_faultsPrCellAcc->faultIdx(
          conn.m_c2GlobIdx,
          cvf::StructGridInterface::oppositeFace(conn.m_c1Face));

    }

    if (fIdx1 < 0 && fIdx2 < 0) {
      cvf::String lgrString("Same Grid");
      if (m_cells[conn.m_c1GlobIdx].hostGrid() !=
          m_cells[conn.m_c2GlobIdx].hostGrid()) {
        lgrString = "Different Grid";
      }

      //cvf::Trace::show("NNC: No Fault for NNC C1: " +
      // cvf::String((int)conn.m_c1GlobIdx) + " C2: " +
      // cvf::String((int)conn.m_c2GlobIdx) +
      // " Grid: " + lgrString);
    }

    if (fIdx1 >= 0) {
      // Add the connection to both, if they are different.
      m_faults[fIdx1]->connectionIndices().push_back(nncIdx);
    }

    if (fIdx2 != fIdx1) {
      if (fIdx2 >= 0)
      {
        m_faults[fIdx2]->connectionIndices().push_back(nncIdx);
      }
    }
  }
}

// =========================================================
// The cell is normally inverted due to Depth becoming -Z at
// import, but if (only) one of the flipX/Y is done, the cell
// is back to normal
bool RIGrid::isFaceNormalsOutwards() const {

  // ---------------------------------------------------------------
  cout << FLGREEN
       << "[wic-rixx]isFaceNormalOutward (ricasedata.cpp)"
       << AEND << endl;

  // ---------------------------------------------------------------
  for (int gcIdx = 0 ; gcIdx < static_cast<int>(m_cells.size()); ++gcIdx) {

    // -------------------------------------------------------------
    if (!m_cells[gcIdx].isInvalid()) {

      // -----------------------------------------------------------
      cvf::Vec3d cellCenter = m_cells[gcIdx].center();
      cvf::Vec3d faceCenter = m_cells[gcIdx].faceCenter(
          cvf::StructGridInterface::POS_I);

      // -----------------------------------------------------------
      cvf::Vec3d faceNormal = m_cells[gcIdx].faceNormalWithAreaLenght(
          cvf::StructGridInterface::POS_I);

      // -----------------------------------------------------------
      double typicalIJCellSize =  characteristicIJCellSize();
      double dummy, dummy2, typicalKSize;
      characteristicCellSizes(&dummy, &dummy2, &typicalKSize);

      // -----------------------------------------------------------
      if ((faceCenter - cellCenter).length() > 0.2 * typicalIJCellSize &&
          (faceNormal.length() > (0.2 * typicalIJCellSize * 0.2* typicalKSize))) {

        // ---------------------------------------------------------
        // Cell is assumed ok to use, so calculate
        // whether the normals are outwards or inwards
        if ((faceCenter - cellCenter) * faceNormal >= 0) {
          return true;
        } else {
          return false;
        }
      }
    }
  }

  // ---------------------------------------------------------------
  return false;
}

// =========================================================
const RIFault*
RIGrid::findFaultFromCellIndexAndCellFace(
    size_t reservoirCellIndex,
    cvf::StructGridInterface::FaceType face) const {

  // ---------------------------------------------------------------
  CVF_ASSERT(m_faultsPrCellAcc.notNull());

  // ---------------------------------------------------------------
  if (face == cvf::StructGridInterface::NO_FACE) return nullptr;

  // ---------------------------------------------------------------
  int faultIdx = m_faultsPrCellAcc->faultIdx(reservoirCellIndex, face);
  if (faultIdx !=  RIFaultsPrCellAccumulator::NO_FAULT ) {
    return m_faults.at(faultIdx);
  }

#if 0
  for (size_t i = 0; i < m_faults.size(); i++) {
    const RIFault* rigFault = m_faults.at(i);
    const vector<RIFault::FaultFace>& faultFaces = rigFault->faultFaces();

    for (size_t fIdx = 0; fIdx < faultFaces.size(); fIdx++) {
      if (faultFaces[fIdx].m_nativeReservoirCellIndex == reservoirCellIndex) {
        if (face == faultFaces[fIdx].m_nativeFace ) {
          return rigFault;
        }
      }

      if (faultFaces[fIdx].m_oppositeReservoirCellIndex == reservoirCellIndex) {
        if (face == cvf::StructGridInterface::oppositeFace(faultFaces[fIdx].m_nativeFace)) {
          return rigFault;
        }
      }
    }
  }
#endif
  return nullptr;
}

// =========================================================
void RIGrid::findIntersectingCells(
    const cvf::BoundingBox& inputBB,
    vector<size_t>* cellIndices) const {

  // ---------------------------------------------------------------
  QDateTime tstart = QDateTime::currentDateTime();
  string str = "Find intersecting cells.";
  // print_dbg_msg_wic_ri(__func__, str, 0.0, 1);

  // ---------------------------------------------------------------
  CVF_ASSERT(m_cellSearchTree.notNull());
  m_cellSearchTree->findIntersections(inputBB, cellIndices);

}

// =========================================================
void RIGrid::buildCellSearchTree() {

  if (m_cellSearchTree.isNull()) {
//  if (m_cellSearchTree) {

    // ---------------------------------------------------------------
    const QDateTime tstart = QDateTime::currentDateTime();
    stringstream ss;
    ss << "Building search tree. m_cells.size() = " << m_cells.size()
       << " -- m_nodes.size() = " << m_nodes.size() << " ";
    // print_dbg_msg_wic_ri(__func__, ss.str(), 0.0, 1);

    // ---------------------------------------------------------------
    size_t cellCount = m_cells.size();

    // ---------------------------------------------------------------
    vector<cvf::BoundingBox> cellBoundingBoxes;
    cellBoundingBoxes.resize(cellCount);

    // ---------------------------------------------------------------
    for (size_t cIdx = 0; cIdx < cellCount; ++cIdx) {

      // -------------------------------------------------------------
      const caf::SizeTArray8& cellIndices =
          m_cells[cIdx].cornerIndices();

      // -------------------------------------------------------------
      if (m_cells[cIdx].isInvalid()) continue;

      // -------------------------------------------------------------
      cvf::BoundingBox& cellBB = cellBoundingBoxes[cIdx];
      cellBB.add(m_nodes[cellIndices[0]]);
      cellBB.add(m_nodes[cellIndices[1]]);
      cellBB.add(m_nodes[cellIndices[2]]);
      cellBB.add(m_nodes[cellIndices[3]]);
      cellBB.add(m_nodes[cellIndices[4]]);
      cellBB.add(m_nodes[cellIndices[5]]);
      cellBB.add(m_nodes[cellIndices[6]]);
      cellBB.add(m_nodes[cellIndices[7]]);
    }

    // ---------------------------------------------------------------
    m_cellSearchTree = new cvf::BoundingBoxTree;
    m_cellSearchTree->buildTreeFromBoundingBoxes(cellBoundingBoxes,
                                                 nullptr);

    // print_dbg_msg_wic_ri(__func__, ss.str(), time_since_msecs(tstart), 2);
  }
}

// =========================================================
cvf::BoundingBox RIGrid::boundingBox() const {

  // -------------------------------------------------------
  if (m_boundingBox.isValid()) return m_boundingBox;

  // -------------------------------------------------------
  for (size_t i = 0; i < m_nodes.size(); ++i) {
    m_boundingBox.add(m_nodes[i]);
  }

  return m_boundingBox;
}

////////////////////////////////////////////////////////////
//
// Copyright (C) 2011-     Statoil ASA
// Copyright (C) 2013-     Ceetron Solutions AS
// Copyright (C) 2011-2012 Ceetron AS
//
// ResInsight is free software: you can redistribute it
// and/or modify it under the terms of the GNU General
// Public License
// as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any
// later version.
//
// ResInsight is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.
//
// See the GNU General Public License at
// <http://www.gnu.org/licenses/gpl.html>
// for more details.
//
////////////////////////////////////////////////////////////
//
// Modified by M.Bellout on 3/5/18.
//

//==========================================================
// ╦═╗  ╦  ╔═╗  ╔═╗  ╔╦╗  ╔═╗  ╔═╗  ╦    ╦    ╦  ╔╗╔  ╔═╗
// ╠╦╝  ║  ╠═╣  ║     ║   ║    ║╣   ║    ║    ║  ║║║  ╠╣
// ╩╚═  ╩  ╩ ╩  ╚═╝   ╩   ╚═╝  ╚═╝  ╩═╝  ╩═╝  ╩  ╝╚╝  ╚
//==========================================================
RIActiveCellInfo::RIActiveCellInfo()
    :   m_reservoirActiveCellCount(0),
        m_reservoirCellResultCount(0),
        m_activeCellPositionMin(cvf::Vec3d::ZERO),
        m_activeCellPositionMax(cvf::Vec3d::ZERO) {}

// =========================================================
void
RIActiveCellInfo::
setReservoirCellCount(size_t reservoirCellCount){

  // -------------------------------------------------------
  m_cellIndexToResultIndex.resize(reservoirCellCount,
                                  cvf::UNDEFINED_SIZE_T);
}

// =========================================================
size_t RIActiveCellInfo::reservoirCellCount() const {
  return m_cellIndexToResultIndex.size();
}

// =========================================================
size_t RIActiveCellInfo::reservoirCellResultCount() const {
  return m_reservoirCellResultCount;
}

// =========================================================
bool
RIActiveCellInfo::isActive(size_t reservoirCellIndex) const {

  // -------------------------------------------------------
  if (m_cellIndexToResultIndex.size() == 0) {
    return true;
  }

  // -------------------------------------------------------
  CVF_TIGHT_ASSERT(
      reservoirCellIndex < m_cellIndexToResultIndex.size());

  // -------------------------------------------------------
  return m_cellIndexToResultIndex[reservoirCellIndex]
      != cvf::UNDEFINED_SIZE_T;
}

// =========================================================
size_t
RIActiveCellInfo::
cellResultIndex(size_t reservoirCellIndex) const {

  // -------------------------------------------------------
  if (m_cellIndexToResultIndex.size() == 0) {
    return reservoirCellIndex;
  }

  // -------------------------------------------------------
  CVF_TIGHT_ASSERT(reservoirCellIndex
                       < m_cellIndexToResultIndex.size());

  // -------------------------------------------------------
  return m_cellIndexToResultIndex[reservoirCellIndex];
}

// =========================================================
void
RIActiveCellInfo::
setCellResultIndex(size_t reservoirCellIndex,
                   size_t reservoirCellResultIndex) {

  // -------------------------------------------------------
  CVF_TIGHT_ASSERT(reservoirCellResultIndex
                       < m_cellIndexToResultIndex.size());

  // -------------------------------------------------------
  m_cellIndexToResultIndex[reservoirCellIndex] =
      reservoirCellResultIndex;

  // -------------------------------------------------------
  if (reservoirCellResultIndex >= m_reservoirCellResultCount) {
    m_reservoirCellResultCount = reservoirCellResultIndex + 1;
  }
}

// =========================================================
void RIActiveCellInfo::setGridCount(size_t gridCount) {
  m_perGridActiveCellInfo.resize(gridCount);
}

// =========================================================
void
RIActiveCellInfo::
setGridActiveCellCounts(size_t gridIndex,
                        size_t activeCellCount) {

  // -------------------------------------------------------
  CVF_ASSERT(gridIndex < m_perGridActiveCellInfo.size());

  // -------------------------------------------------------
  m_perGridActiveCellInfo[gridIndex].
      setActiveCellCount(activeCellCount);
}

// =========================================================
void RIActiveCellInfo::computeDerivedData() {

  m_reservoirActiveCellCount = 0;

  // -------------------------------------------------------
  for (size_t i = 0; i < m_perGridActiveCellInfo.size(); i++) {
    m_reservoirActiveCellCount +=
        m_perGridActiveCellInfo[i].activeCellCount();
  }
}

// =========================================================
size_t RIActiveCellInfo::reservoirActiveCellCount() const {
  return m_reservoirActiveCellCount;
}

// =========================================================
void
RIActiveCellInfo::setIJKBoundingBox(const cvf::Vec3st& min,
                                    const cvf::Vec3st& max) {
  m_activeCellPositionMin = min;
  m_activeCellPositionMax = max;
}

// =========================================================
void RIActiveCellInfo::IJKBoundingBox(cvf::Vec3st& min,
                                      cvf::Vec3st& max) const {
  min = m_activeCellPositionMin;
  max = m_activeCellPositionMax;
}

// =========================================================
void
RIActiveCellInfo::
gridActiveCellCounts(size_t gridIndex,
                     size_t& activeCellCount) const {

  // -------------------------------------------------------
  activeCellCount =
      m_perGridActiveCellInfo[gridIndex].activeCellCount();
}

// =========================================================
cvf::BoundingBox
RIActiveCellInfo::geometryBoundingBox() const {
  return m_activeCellsBoundingBox;
}

// =========================================================
void
RIActiveCellInfo::
setGeometryBoundingBox(cvf::BoundingBox bb) {
  m_activeCellsBoundingBox = bb;
}

// =========================================================
void RIActiveCellInfo::clear() {

  m_perGridActiveCellInfo.clear();
  m_cellIndexToResultIndex.clear();
  m_reservoirActiveCellCount = 0;
  m_activeCellPositionMin = cvf::Vec3st(0,0,0);
  m_activeCellPositionMax = cvf::Vec3st(0,0,0);
  m_activeCellsBoundingBox.reset();
}

// =========================================================
bool RIActiveCellInfo::isCoarseningActive() const {
  return m_reservoirCellResultCount != m_reservoirActiveCellCount;
}

// =========================================================
RIActiveCellInfo::GridActiveCellCounts::GridActiveCellCounts()
    : m_activeCellCount(0) {
}

// =========================================================
size_t
RIActiveCellInfo::GridActiveCellCounts::activeCellCount() const {
  return m_activeCellCount;
}

// =========================================================
void
RIActiveCellInfo::GridActiveCellCounts::setActiveCellCount(
    size_t activeCellCount) {
  m_activeCellCount = activeCellCount;
}

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
RILocalGrid::RILocalGrid(RIGrid* mainGrid):
    RIGridBase(mainGrid),
    m_parentGrid(NULL),
    m_positionInParentGrid(cvf::UNDEFINED_SIZE_T) {
}

RILocalGrid::~RILocalGrid() {
  cout << "[wic-rixx]deleting vars.----- RILocalGrid" << endl;
}

