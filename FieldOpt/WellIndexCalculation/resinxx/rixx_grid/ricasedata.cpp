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
// Modified by M.Bellout on 3/7/18.
//

// ---------------------------------------------------------
#include "ricasedata.h"

// =========================================================
// ╦═╗  ╦  ╔═╗  ╔═╗  ╔═╗  ╔═╗  ╔╦╗  ╔═╗  ╔╦╗  ╔═╗
// ╠╦╝  ║  ║    ╠═╣  ╚═╗  ║╣    ║║  ╠═╣   ║   ╠═╣
// ╩╚═  ╩  ╚═╝  ╩ ╩  ╚═╝  ╚═╝  ═╩╝  ╩ ╩   ╩   ╩ ╩
//==========================================================
RICaseData::RICaseData(string file_path) {

  // -------------------------------------------------------
  m_mainGrid = new RIGrid(file_path);
//  m_ownerCase = ownerCase;

  // -------------------------------------------------------
  // m_matrixModelResults = new RICaseCellResultsData(this);
//  m_fractureModelResults = new RigCaseCellResultsData(this);

  // -------------------------------------------------------
  m_activeCellInfo = new RIActiveCellInfo;
  m_fractureActiveCellInfo = new RIActiveCellInfo;

  // -------------------------------------------------------
  setActiveCellInfo(PorosityModelTypeFRAC_,
                    m_activeCellInfo);

  setActiveCellInfo(PorosityModelTypeMATRIX_,
                    m_fractureActiveCellInfo);

  // m_matrixModelResults->
  //     setActiveCellInfo(m_activeCellInfo.p());
  // m_fractureModelResults->
  //     setActiveCellInfo(m_fractureActiveCellInfo.p());

  // -------------------------------------------------------
//  m_unitsType = RiaEclipseUnitTools::UNITS_METRIC;

  // -------------------------------------------------------
  PorosityModelTypeMATRIX_ = PorosityModelType::MATRIX_MODEL;
  PorosityModelTypeFRAC_ = PorosityModelType::FRACTURE_MODEL;

}

// =========================================================
RICaseData::~RICaseData() {

  // -------------------------------------------------------
#ifdef FIELDOPT_BUILD

  // -------------------------------------------------------
  /*
  if (m_mainGrid.notNull()) {

    // -----------------------------------------------------
    printf("%s%s%s", FRED, "m_mainGrid != nullptr ", AEND);
    delete m_mainGrid.p(); // This call the destructor of RIGrid
    printf("%s%s%s\n", FRED, "TRUE (~RICaseData)", AEND);

  } else {

    // -----------------------------------------------------
    printf("%s%s%s\n", FRED,
           "m_mainGrid != nullptr FALSE (~RICaseData)", AEND);

  }
  */

  // -------------------------------------------------------
  if (m_activeCellInfo != NULL) {

    // -----------------------------------------------------
    printf("%s%s%s", FRED, "m_activeCellInfo != nullptr ", AEND);
    delete m_activeCellInfo;
    printf("%s%s%s\n", FRED, "TRUE (~RICaseData)", AEND);

  } else {

    // -----------------------------------------------------
    printf("%s%s%s\n", FRED,
           "m_activeCellInfo != nullptr FALSE (~RICaseData)", AEND);

  }

  // -------------------------------------------------------
  if (m_fractureActiveCellInfo != NULL) {

    // -----------------------------------------------------
    // printf("%s%s%s", FRED, "m_fractureActiveCellInfo != nullptr ", AEND);

    // delete m_fractureActiveCellInfo; // <-- crash with seg.fault or
    // corrupted memory after about one GPS iteration --> why?

    // m_fractureActiveCellInfo->release(); // <- assertion failed
    // printf("%s%s%s\n", FRED, "TRUE (~RICaseData)", AEND);

  } else {

    // -----------------------------------------------------
    printf("%s%s%s\n", FRED,
           "m_fractureActiveCellInfo != nullptr FALSE (~RICaseData)", AEND);

  }

  // -------------------------------------------------------
  printf("%s%s%s\n", FRED, "FIELDOPT_BUILD (~RICaseData)", AEND);

#elif ADGPRS_LIB_BUILD

  printf("%s%s%s\n", FRED, "ADGPRS_BUILD (~RICaseData)", AEND);

#endif

  // -------------------------------------------------------
  cout << "[wic-rixx]deleting vars.----- RICaseData" << endl;

}

//==========================================================
RIGrid* RICaseData::mainGrid() {
  return m_mainGrid.p();
}

//==========================================================
const RIGrid* RICaseData::mainGrid() const {
  return m_mainGrid.p();
}

//==========================================================
void RICaseData::setMainGrid(RIGrid* mainGrid) {
  m_mainGrid = mainGrid;
//  m_matrixModelResults->setMainGrid(m_mainGrid);
//  m_fractureModelResults->setMainGrid(m_mainGrid);
}

//==========================================================
// Get grid by index.
// Main grid has index 0, so first lgr has idx 1.
RIGridBase* RICaseData::grid(size_t index) {
  //CVF_ASSERT(m_mainGrid.notNull());
  return m_mainGrid->gridByIndex(index);
}

// =========================================================
// Get grid by index.
// Main grid has index 0, so first lgr has idx 1.
const RIGridBase* RICaseData::grid(size_t index) const {
  //CVF_ASSERT(m_mainGrid.notNull());
  return m_mainGrid->gridByIndex(index);
}

// =========================================================
size_t RICaseData::gridCount() const {
  //CVF_ASSERT(m_mainGrid.notNull());
  return m_mainGrid->gridCount();
}

// =========================================================
// ╔═╗  ╔═╗  ╦    ╦    ╦═╗  ╔═╗  ╔╗╔  ╔═╗  ╔═╗  ╔╗   ╔╗
// ║    ║╣   ║    ║    ╠╦╝  ╠═╣  ║║║  ║ ╦  ║╣   ╠╩╗  ╠╩╗
// ╚═╝  ╚═╝  ╩═╝  ╩═╝  ╩╚═  ╩ ╩  ╝╚╝  ╚═╝  ╚═╝  ╚═╝  ╚═╝
// =========================================================
// Helper class used to find min/max range for valid and
// active cells
class CellRangeBB
{
 public:
  // -------------------------------------------------------
  CellRangeBB()
      : m_min(cvf::UNDEFINED_SIZE_T,
              cvf::UNDEFINED_SIZE_T,
              cvf::UNDEFINED_SIZE_T),
        m_max(cvf::Vec3st::ZERO) {
  }

  // -------------------------------------------------------
  void add(size_t i, size_t j, size_t k) {

    if (i < m_min.x()) m_min.x() = i;
    if (j < m_min.y()) m_min.y() = j;
    if (k < m_min.z()) m_min.z() = k;

    if (i > m_max.x()) m_max.x() = i;
    if (j > m_max.y()) m_max.y() = j;
    if (k > m_max.z()) m_max.z() = k;
  }

 public:
  // -------------------------------------------------------
  cvf::Vec3st m_min;
  cvf::Vec3st m_max;
};

//==========================================================
void RICaseData::computeActiveCellIJKBBox() {

  // -------------------------------------------------------
//  if (m_mainGrid != 0
//      && m_activeCellInfo != 0
//      && m_fractureActiveCellInfo != 0) {

  // -------------------------------------------------------
  cout << FLGREEN
       << "[wic-rixx]compActCellIJKBBox- (ricasedata.cpp)"
       << AEND << endl;

  // -------------------------------------------------------
  CellRangeBB matrixModelActiveBB;
  CellRangeBB fractureModelActiveBB;

  // -------------------------------------------------------
  size_t idx;
  for (idx = 0; idx < m_mainGrid->cellCount(); idx++) {

    // -----------------------------------------------------
    size_t i, j, k;
    m_mainGrid->ijkFromCellIndex(idx, &i, &j, &k);

    // -----------------------------------------------------
    if (m_activeCellInfo->isActive(idx)) {
      matrixModelActiveBB.add(i, j, k);
    }

    // -----------------------------------------------------
    if (m_fractureActiveCellInfo->isActive(idx)) {
      fractureModelActiveBB.add(i, j, k);
    }
  }

  // -------------------------------------------------------
  m_activeCellInfo->setIJKBoundingBox(
      matrixModelActiveBB.m_min,
      matrixModelActiveBB.m_max);

  // -------------------------------------------------------
  m_fractureActiveCellInfo->setIJKBoundingBox(
      fractureModelActiveBB.m_min,
      fractureModelActiveBB.m_max);

  // } // if-statement
}

//==========================================================
void RICaseData::computeActiveCellBoundingBoxes() {
  computeActiveCellIJKBBox();
  computeActiveCellsGeometryBoundingBox();
}

//==========================================================
RIActiveCellInfo*
RICaseData::activeCellInfo(
    PorosityModelType porosityModel) {

  // -------------------------------------------------------
  if (porosityModel == MATRIX_MODEL) {
    return m_activeCellInfo;
  }

  // -------------------------------------------------------
  return m_fractureActiveCellInfo;
}

// =========================================================
const RIActiveCellInfo*
RICaseData::activeCellInfo(
    PorosityModelType porosityModel) const {

  // -------------------------------------------------------
  if (porosityModel == MATRIX_MODEL) {
    return m_activeCellInfo;
  }

  // -------------------------------------------------------
  return m_fractureActiveCellInfo;
}

// =========================================================
void
RICaseData::setActiveCellInfo(
    PorosityModelType porosityModel,
    RIActiveCellInfo* activeCellInfo) {

  // -------------------------------------------------------
  if (porosityModel == MATRIX_MODEL) {

    m_activeCellInfo = activeCellInfo;
    // m_matrixModelResults->
    //    setActiveCellInfo(m_activeCellInfo);

  } else {

    m_fractureActiveCellInfo = activeCellInfo;
    // m_fractureModelResults->
    //    setActiveCellInfo(m_fractureActiveCellInfo);
  }
}

// =========================================================
void RICaseData::computeActiveCellsGeometryBoundingBox() {

  // -------------------------------------------------------
  if (verb_vector()[3] > 3) // idx:3 -> wic
    cout << fstr("[wic-rixx]compActCellsGeoBBox.",3)
         << "[ricasedata.cpp]" << endl;

  // -------------------------------------------------------
  // MB
  // if (m_activeCellInfo != 0
  //    || m_fractureActiveCellInfo != 0) {
//    return;
//  }

  // MB
  // Importat: force computation and setting of geometry BB.
  //
  // if (m_mainGrid != 0) {
  // -------------------------------------------------------
  //  cvf::BoundingBox bb;
  //  m_activeCellInfo->setGeometryBoundingBox(bb);
  //  m_fractureActiveCellInfo->setGeometryBoundingBox(bb);
  //  cout << FLGREEN<< bb.debugString().toStdString()
  //       << AEND << endl;
  //  return;
  // }

  // -------------------------------------------------------
  RIActiveCellInfo* activeInfos[2];
  activeInfos[0] = m_fractureActiveCellInfo;
  // Last, to make this bb.min become display offset
  activeInfos[1] = m_activeCellInfo;

  // -------------------------------------------------------
  cvf::BoundingBox bb;
  // Loop through active indices for matrix and fracture grid
  for (int acIdx = 0; acIdx < 2; ++acIdx) {

    // -----------------------------------------------------
    bb.reset();
    if (m_mainGrid->nodes().size() == 0) {
      bb.add(cvf::Vec3d::ZERO);

    } else {

      // ---------------------------------------------------
      // Loop through all cells
      for (size_t i = 0; i < m_mainGrid->cellCount(); i++) {

        // -------------------------------------------------
        // Loop only over cells that are active
        if (activeInfos[acIdx]->isActive(i)) {

          // -----------------------------------------------
          // Get current cell
          const RICell& c = m_mainGrid->globalCellArray()[i];

          // -----------------------------------------------
          // Get corner indices for current cell
          const caf::SizeTArray8& indices = c.cornerIndices();

          // -----------------------------------------------
          // All all the cells nodes to the bounding-box
          size_t idx;
          for (idx = 0; idx < 8; idx++) {
            bb.add(m_mainGrid->nodes()[indices[idx]]);
          }
        }
      }
      // ---------------------------------------------------
      if (verb_vector()[3] > 3) // idx:3 -> wic
        cout << fstr("[wic-rixx]computeActiveCellsGeomBB.",3)
             << bb.debugString().toStdString() << endl;
    }

    // -----------------------------------------------------
    activeInfos[acIdx]->setGeometryBoundingBox(bb);
  }

  // -------------------------------------------------------
  m_mainGrid->setDisplayModelOffset(bb.min());
}

////////////////////////////////////////////////////////////
//
// Copyright (C) Statoil ASA
// Copyright (C) Ceetron Solutions AS
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
// Modified by M.Bellout on 3/7/18.
//

// =========================================================
// ╦═╗  ╦  ╦═╗  ╔═╗  ╔═╗  ╔╦╗  ╦═╗  ╦  ╔╗╔  ╔╦╗  ╦═╗  ╔═╗
// ╠╦╝  ║  ╠╦╝  ║╣   ╠═╣   ║║  ╠╦╝  ║  ║║║   ║   ╠╦╝  ╠╣
// ╩╚═  ╩  ╩╚═  ╚═╝  ╩ ╩  ═╩╝  ╩╚═  ╩  ╝╚╝   ╩   ╩╚═  ╚
// =========================================================

// =========================================================
//bool RIReaderInterface::isFaultImportEnabled() {
//    return readerSettings()->importFaults;
//}

// -----------------------------------------------------------------
//bool RIReaderInterface::isImportOfCompleteMswDataEnabled() {
//    return readerSettings()->importAdvancedMswData;
//}

// -----------------------------------------------------------------
//bool RIReaderInterface::isNNCsEnabled() {
////    return readerSettings()->importNNCs;
//}

// -----------------------------------------------------------------
//const QString RifReaderInterface::faultIncludeFileAbsolutePathPrefix() {
////    return readerSettings()->faultIncludeFileAbsolutePathPrefix;
//}

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
// Modified by M.Bellout on 3/7/18.
//

typedef struct ecl_grid_struct ecl_grid_type;
typedef struct ecl_file_struct ecl_file_type;
typedef struct well_conn_struct well_conn_type;

// ╦═╗  ╦  ╦═╗  ╔═╗  ╔═╗  ╔╦╗  ╔═╗  ╦═╗  ╔═╗  ╔═╗  ╦
// ╠╦╝  ║  ╠╦╝  ║╣   ╠═╣   ║║  ║╣   ╠╦╝  ║╣   ║    ║
// ╩╚═  ╩  ╩╚═  ╚═╝  ╩ ╩  ═╩╝  ╚═╝  ╩╚═  ╚═╝  ╚═╝  ╩═╝
// =================================================================

// -----------------------------------------------------------------
///     ECLIPSE cell numbering layout:
///        Lower layer:   Upper layer
///        Low Depth      High Depth
///        Low K          High K
///        Shallow        Deep
///       2---3           6---7
///       |   |           |   |
///       0---1           4---5
///
///
///
// -----------------------------------------------------------------

// The indexing conventions for vertices in ECLIPSE
//
//      2-------------3
//     /|            /|
//    / |           / |               /j
//   /  |          /  |              /
//  0-------------1   |             *---i
//  |   |         |   |             |
//  |   6---------|---7             |
//  |  /          |  /              |k
//  | /           | /
//  |/            |/
//  4-------------5
//  vertex indices
//
// The indexing conventions for vertices in ResInsight
//
//      7-------------6             |k
//     /|            /|             | /j
//    / |           / |             |/
//   /  |          /  |             *---i
//  4-------------5   |
//  |   |         |   |
//  |   3---------|---2
//  |  /          |  /
//  | /           | /
//  |/            |/
//  0-------------1
//  vertex indices
//

static const size_t cellMappingECLRi[8] = { 0, 1, 3, 2, 4, 5, 7, 6 };

// -----------------------------------------------------------------
// Static functions
//******************************************************************

bool transferGridCellData(RIGrid* mainGrid,
                          RIActiveCellInfo* activeCellInfo,
                          RIActiveCellInfo* fractureActiveCellInfo,
                          RIGridBase* localGrid,
                          const ecl_grid_type* localEclGrid,
                          size_t matrixActiveStartIndex,
                          size_t fractureActiveStartIndex) {

  CVF_ASSERT(activeCellInfo && fractureActiveCellInfo);

  // ---------------------------------------------------------------
  int cellCount = ecl_grid_get_global_size(localEclGrid);
  size_t cellStartIndex = mainGrid->globalCellArray().size();
  size_t nodeStartIndex = mainGrid->nodes().size();

  // ---------------------------------------------------------------
  RICell defaultCell;
  defaultCell.setHostGrid(localGrid);
  mainGrid->globalCellArray().resize(cellStartIndex + cellCount, defaultCell);

  // ---------------------------------------------------------------
  mainGrid->nodes().resize(nodeStartIndex + cellCount*8, cvf::Vec3d(0,0,0));

  // ---------------------------------------------------------------
  // int progTicks = 100;
  // double cellsPrProgressTick = cellCount/(float)progTicks;
//  caf::ProgressInfo progInfo(progTicks, "");

  // ---------------------------------------------------------------
  size_t computedCellCount = 0;
  // Loop over cells and fill them with data

  // ---------------------------------------------------------------
#pragma omp parallel for
  for (int gridLocalCellIndex = 0; gridLocalCellIndex < cellCount; ++gridLocalCellIndex) {

    RICell& cell = mainGrid->globalCellArray()[cellStartIndex + gridLocalCellIndex];

    cell.setGridLocalCellIndex(gridLocalCellIndex);

    // Active cell index
    int matrixActiveIndex = ecl_grid_get_active_index1(localEclGrid,
                                                       gridLocalCellIndex);
    if (matrixActiveIndex != -1) {
      activeCellInfo->setCellResultIndex(cellStartIndex + gridLocalCellIndex,
                                         matrixActiveStartIndex + matrixActiveIndex);
    }

    int fractureActiveIndex = ecl_grid_get_active_fracture_index1(localEclGrid,
                                                                  gridLocalCellIndex);
    if (fractureActiveIndex != -1) {
      fractureActiveCellInfo->setCellResultIndex(cellStartIndex + gridLocalCellIndex,
                                                 fractureActiveStartIndex + fractureActiveIndex);
    }

    // Parent cell index
    int parentCellIndex = ecl_grid_get_parent_cell1(localEclGrid, gridLocalCellIndex);
    if (parentCellIndex == -1) {
      cell.setParentCellIndex(cvf::UNDEFINED_SIZE_T);
    } else {
      cell.setParentCellIndex(parentCellIndex);
    }

    // Corner coordinates
    int cIdx;
    for (cIdx = 0; cIdx < 8; ++cIdx) {

      double * point = mainGrid->nodes()
      [nodeStartIndex + gridLocalCellIndex * 8 + cellMappingECLRi[cIdx]].ptr();

      ecl_grid_get_cell_corner_xyz1(localEclGrid, gridLocalCellIndex, cIdx,
                                    &(point[0]), &(point[1]), &(point[2]));

      point[2] = -point[2]; // Flipping Z making depth become negative z values
      cell.cornerIndices()[cIdx] = nodeStartIndex + gridLocalCellIndex*8 + cIdx;
    }

    // Sub grid in cell
    const ecl_grid_type* subGrid = ecl_grid_get_cell_lgr1(localEclGrid,
                                                          gridLocalCellIndex);

    if (subGrid != NULL) {
      int subGridId = ecl_grid_get_lgr_nr(subGrid);
      CVF_ASSERT(subGridId > 0);
      cell.setSubGrid(static_cast<RILocalGrid*>(mainGrid->gridById(subGridId)));
    }

    // Mark inactive long pyramid looking cells as invalid
    // Forslag
    //if (!invalid && (cell.isInCoarseCell()
    // || (!cell.isActiveInMatrixModel() && !cell.isActiveInFractureModel()) ) )
    cell.setInvalid(cell.isLongPyramidCell());

#pragma omp atomic
    computedCellCount++;

//    progInfo.setProgress((int)(computedCellCount/cellsPrProgressTick));
  }
  return true;
}

// -----------------------------------------------------------------
RIReaderECL::RIReaderECL() {
  m_fileName.clear();
  m_filesWithSameBaseName.clear();

  m_eclipseCase = nullptr;

  m_ecl_init_file = nullptr;
  m_dynamicResultsAccess = nullptr;
}

// -----------------------------------------------------------------
RIReaderECL::~RIReaderECL() {
  if (m_ecl_init_file) {
    ecl_file_close(m_ecl_init_file);
  }
  m_ecl_init_file = nullptr;

  if (m_dynamicResultsAccess.notNull()) {
    m_dynamicResultsAccess->close();
  }

}

// -----------------------------------------------------------------
// Read geometry from file given by name into given reservoir object
bool RIReaderECL::transferGeometry(const ecl_grid_type* mainEclGrid,
                                   RICaseData* eclipseCase) {
  CVF_ASSERT(eclipseCase);

  // ---------------------------------------------------------------
  cout << FLGREEN
       << "[wic-rixx]Reading geometry--- (ricasedata.cpp)"
       << AEND << endl;

  // ---------------------------------------------------------------
  if (!mainEclGrid) {
    // Some error
    return false;
  }

  // ---------------------------------------------------------------
  RIActiveCellInfo* activeCellInfo =
      eclipseCase->activeCellInfo(MATRIX_MODEL);

  // ---------------------------------------------------------------
  RIActiveCellInfo* fractureActiveCellInfo =
      eclipseCase->activeCellInfo(FRACTURE_MODEL);

  CVF_ASSERT(activeCellInfo && fractureActiveCellInfo);

  // ---------------------------------------------------------------
  RIGrid* mainGrid = eclipseCase->mainGrid();
  CVF_ASSERT(mainGrid);

  // ---------------------------------------------------------------
  {
    cvf::Vec3st  gridPointDim(0,0,0);
    gridPointDim.x() = ecl_grid_get_nx(mainEclGrid) + 1;
    gridPointDim.y() = ecl_grid_get_ny(mainEclGrid) + 1;
    gridPointDim.z() = ecl_grid_get_nz(mainEclGrid) + 1;
    mainGrid->setGridPointDimensions(gridPointDim);
  }

  // ---------------------------------------------------------------
  // string mainGridName = ecl_grid_get_name(mainEclGrid);
  // ERT returns file path to grid file as name for main grid
  mainGrid->setGridName("Main grid");

  // ---------------------------------------------------------------
  // Get and set grid and lgr metadata
  size_t totalCellCount =
      static_cast<size_t>(ecl_grid_get_global_size(mainEclGrid));

  // ---------------------------------------------------------------
  int numLGRs = ecl_grid_get_num_lgr(mainEclGrid);
  int lgrIdx;

  // ---------------------------------------------------------------
  for (lgrIdx = 0; lgrIdx < numLGRs; ++lgrIdx) {
    ecl_grid_type* localEclGrid = ecl_grid_iget_lgr(mainEclGrid, lgrIdx);

    string lgrName = ecl_grid_get_name(localEclGrid);
    int lgrId = ecl_grid_get_lgr_nr(localEclGrid);

    // -------------------------------------------------------------
    cvf::Vec3st  gridPointDim(0,0,0);
    gridPointDim.x() = ecl_grid_get_nx(localEclGrid) + 1;
    gridPointDim.y() = ecl_grid_get_ny(localEclGrid) + 1;
    gridPointDim.z() = ecl_grid_get_nz(localEclGrid) + 1;

    // -------------------------------------------------------------
    RILocalGrid* localGrid = new RILocalGrid(mainGrid);
    localGrid->setGridId(lgrId);
    mainGrid->addLocalGrid(localGrid);

    // -------------------------------------------------------------
    localGrid->setIndexToStartOfCells(totalCellCount);
    localGrid->setGridName(lgrName);
    localGrid->setGridPointDimensions(gridPointDim);
    totalCellCount += ecl_grid_get_global_size(localEclGrid);
  }

  // ---------------------------------------------------------------
  activeCellInfo->setReservoirCellCount(totalCellCount);
  fractureActiveCellInfo->setReservoirCellCount(totalCellCount);

  // ---------------------------------------------------------------
  // Reserve room for the cells and nodes and fill them with data
  mainGrid->globalCellArray().reserve(totalCellCount);
  mainGrid->nodes().reserve(8*totalCellCount);

//  caf::ProgressInfo progInfo(3 + numLGRs, "");
//  progInfo.setProgressDescription("Main Grid");
//  progInfo.setNextProgressIncrement(3);

  // ---------------------------------------------------------------
  transferGridCellData(mainGrid, activeCellInfo,
                       fractureActiveCellInfo,
                       mainGrid, mainEclGrid, 0, 0);

//  progInfo.setProgress(3);

  // ---------------------------------------------------------------
  size_t globalMatrixActiveSize = ecl_grid_get_nactive(mainEclGrid);
  size_t globalFractureActiveSize = ecl_grid_get_nactive_fracture(mainEclGrid);

  // ---------------------------------------------------------------
  activeCellInfo->setGridCount(1 + numLGRs);
  fractureActiveCellInfo->setGridCount(1 + numLGRs);

  // ---------------------------------------------------------------
  activeCellInfo->setGridActiveCellCounts(0, globalMatrixActiveSize);
  fractureActiveCellInfo->setGridActiveCellCounts(0, globalFractureActiveSize);

  transferCoarseningInfo(mainEclGrid, mainGrid);

  // ---------------------------------------------------------------
  for (lgrIdx = 0; lgrIdx < numLGRs; ++lgrIdx) {
//    progInfo.setProgressDescription("LGR number " + QString::number(lgrIdx+1));

    // ---------------------------------------------------------------
    ecl_grid_type* localEclGrid = ecl_grid_iget_lgr(mainEclGrid, lgrIdx);
    RILocalGrid* localGrid = static_cast<RILocalGrid*>(mainGrid->gridByIndex(lgrIdx+1));

    transferGridCellData(mainGrid,
                         activeCellInfo,
                         fractureActiveCellInfo,
                         localGrid,
                         localEclGrid,
                         globalMatrixActiveSize,
                         globalFractureActiveSize);

    // ---------------------------------------------------------------
    int matrixActiveCellCount = ecl_grid_get_nactive(localEclGrid);
    globalMatrixActiveSize += matrixActiveCellCount;

    // ---------------------------------------------------------------
    int fractureActiveCellCount = ecl_grid_get_nactive_fracture(localEclGrid);
    globalFractureActiveSize += fractureActiveCellCount;

    // ---------------------------------------------------------------
    activeCellInfo->setGridActiveCellCounts(lgrIdx + 1, matrixActiveCellCount);
    fractureActiveCellInfo->setGridActiveCellCounts(lgrIdx + 1, fractureActiveCellCount);

    transferCoarseningInfo(localEclGrid, localGrid);

//    progInfo.setProgress(3 + lgrIdx);
  }

  mainGrid->initAllSubGridsParentGridPointer();
  activeCellInfo->computeDerivedData();
  fractureActiveCellInfo->computeDerivedData();

  return true;
}


// -----------------------------------------------------------------
// Open file and read geometry into given reservoir object
bool RIReaderECL::open(const QString& fileName,
                       RICaseData* eclipseCase) {

  CVF_ASSERT(eclipseCase);

  // ---------------------------------------------------------------
  cout << FLGREEN << "[wic-rixx]Reading Grid------- (ricasedata.cpp)" << AEND;
  QStringList fileSet;

  // ---------------------------------------------------------------
  if (!RIECLFileTools::
  findSiblingFilesWithSameBaseName(fileName, &fileSet)) {
    cout << FLGREEN << "findSiblingFilesWithSameBaseName FAILED!" << AEND << endl;
    return false;
  } else {
    cout << FLGREEN << " - " << fileName.toStdString() << AEND;
  }
  m_fileName = fileName;
  m_filesWithSameBaseName = fileSet;

  // ---------------------------------------------------------------
  // Todo: Needs to check existence of file before calling ert, else it will abort
  ecl_grid_type * mainEclGrid = ecl_grid_alloc(fileName.toLatin1().data());
  cout << FLGREEN << " - mem alloc " << AEND << endl;

  // ---------------------------------------------------------------
  if (!transferGeometry(mainEclGrid, eclipseCase)) {
    cout << FLGREEN << "transferGeometry FAILED!" << AEND << endl;
    return false;
  }

  // ---------------------------------------------------------------
  // cout << "Reading faults" << endl;
  // cvf::Collection<RIFault> faults;
  // importFaults(fileSet, &faults);

  RIGrid* mainGrid = eclipseCase->mainGrid();
  // mainGrid->setFaults(faults);
  m_eclipseCase = eclipseCase;

  // ---------------------------------------------------------------
  // cout << "Build results meta data" << endl;
  // buildMetaData();

  // ---------------------------------------------------------------
  // Needs:
  //#include "ert/ecl/ecl_nnc_data.h"
  //#include "ert/ecl/ecl_nnc_geometry.h"

  // cout << "Reading NNC data" << endl;
  // transferStaticNNCData(mainEclGrid, m_ecl_init_file, eclipseCase->mainGrid());

  // This test should probably be improved to test more directly for presence of NNC data
  // if (m_eclipseCase->results(MATRIX_MODEL)->hasFlowDiagUsableFluxes()) {
  // transferDynamicNNCData(mainEclGrid, eclipseCase->mainGrid());
  // }

  // ---------------------------------------------------------------
//   cout << "Processing NNC data" << endl;
//   eclipseCase->mainGrid()->nncData()->processConnections( *(eclipseCase->mainGrid()));

  // ---------------------------------------------------------------
  // cout << "Reading Well information" << endl;
  // readWellCells(mainEclGrid, true);

  // ---------------------------------------------------------------
  cout << FLGREEN
       << "[wic-rixx]Releasing memory--- (ricasedata.cpp)"
       << AEND << endl;
  ecl_grid_free( mainEclGrid );

  return true;
}


// -----------------------------------------------------------------
void
RIReaderECL::setFileDataAccess(RIECLRestartDataAccess* restartDataAccess) {
  m_dynamicResultsAccess = restartDataAccess;
}

// Needs:
//#include "ert/ecl/ecl_nnc_data.h"
//#include "ert/ecl/ecl_nnc_geometry.h"
//
// -----------------------------------------------------------------
//void RIReaderECL::transferStaticNNCData(
//    const ecl_grid_type* mainEclGrid,
//    ecl_file_type* init_file,
//    RIGrid* mainGrid) {
//
//  if (!m_ecl_init_file ) { return; }
//
//  CVF_ASSERT(mainEclGrid && mainGrid);
//
//  // ---------------------------------------------------------------
//  // Get the data from ERT
//  ecl_nnc_geometry_type* nnc_geo =
//      ecl_nnc_geometry_alloc(mainEclGrid);
//
//  if (nnc_geo) {
//    ecl_nnc_data_type* tran_data =
//        ecl_nnc_data_alloc_tran(mainEclGrid,
//                                nnc_geo,
//                                ecl_file_get_global_view(init_file));
//
//    // -------------------------------------------------------------
//    if (tran_data) {
//      int numNNC = ecl_nnc_data_get_size(tran_data);
//      int geometrySize = ecl_nnc_geometry_size(nnc_geo);
//      CVF_ASSERT(numNNC == geometrySize);
//
//      if (numNNC > 0) {
//        // Transform to our own data structures
//        mainGrid->nncData()->connections().resize(numNNC);
//
//        vector<double>& transmissibilityValues =
//            mainGrid->nncData()->makeStaticConnectionScalarResult(
//                RINNCData::propertyNameCombTrans());
//
//        const double* transValues = ecl_nnc_data_get_values(tran_data);
//
//        for (int nIdx = 0; nIdx < numNNC; ++nIdx) {
//          const ecl_nnc_pair_type* geometry_pair = ecl_nnc_geometry_iget(nnc_geo, nIdx);
//
//          RIGridBase* grid1 =  mainGrid->gridByIndex(geometry_pair->grid_nr1);
//
//          mainGrid->nncData()->connections()[nIdx].m_c1GlobIdx =
//              grid1->reservoirCellIndex(geometry_pair->global_index1);
//
//          RIGridBase* grid2 =  mainGrid->gridByIndex(geometry_pair->grid_nr2);
//
//          mainGrid->nncData()->connections()[nIdx].m_c2GlobIdx =
//              grid2->reservoirCellIndex(geometry_pair->global_index2);
//
//          transmissibilityValues[nIdx] = transValues[nIdx];
//        }
//      }
//
//      ecl_nnc_data_free(tran_data);
//    }
//
//    ecl_nnc_geometry_free(nnc_geo);
//  }
//}

// -----------------------------------------------------------------
void RIReaderECL::transferDynamicNNCData(
    const ecl_grid_type* mainEclGrid, RIGrid* mainGrid)
{
  CVF_ASSERT(mainEclGrid && mainGrid);

  if (m_dynamicResultsAccess.isNull()) return;

  size_t timeStepCount = m_dynamicResultsAccess->timeStepCount();

  vector< vector<double> >& waterFluxData =
      mainGrid->nncData()->makeDynamicConnectionScalarResult(
          RINNCData::propertyNameFluxWat(), timeStepCount);

  vector< vector<double> >& oilFluxData =
      mainGrid->nncData()->makeDynamicConnectionScalarResult(
          RINNCData::propertyNameFluxOil(), timeStepCount);

  vector< vector<double> >& gasFluxData =
      mainGrid->nncData()->makeDynamicConnectionScalarResult(
          RINNCData::propertyNameFluxGas(), timeStepCount);

  for (size_t timeStep = 0; timeStep < timeStepCount; ++timeStep)
  {
    m_dynamicResultsAccess->dynamicNNCResults(mainEclGrid, timeStep,
                                              &waterFluxData[timeStep],
                                              &oilFluxData[timeStep],
                                              &gasFluxData[timeStep]);
  }
}

// -----------------------------------------------------------------
bool
RIReaderECL::openAndReadActiveCellData(const QString& fileName,
                                       const vector<QDateTime>& mainCaseTimeSteps,
                                       RICaseData* eclipseCase) {
  CVF_ASSERT(eclipseCase);

  // It is required to have a main grid before reading active cell data
  if (!eclipseCase->mainGrid()) {
    return false;
  }

  // Get set of files
  QStringList fileSet;
  if (!RIECLFileTools::findSiblingFilesWithSameBaseName(fileName,
                                                        &fileSet))
    return false;

  // Keep the set of files of interest
  m_filesWithSameBaseName = fileSet;
  m_eclipseCase = eclipseCase;


  if (!readActiveCellInfo()) {
    return false;
  }

  ensureDynamicResultAccessIsPresent();
  if (m_dynamicResultsAccess.notNull()) {
    m_dynamicResultsAccess->setTimeSteps(mainCaseTimeSteps);
  }

  return true;
}


// -----------------------------------------------------------------
// See also RigStatistics::computeActiveCellUnion()
bool RIReaderECL::readActiveCellInfo() {

  // ---------------------------------------------------------------
  CVF_ASSERT(m_eclipseCase);
  CVF_ASSERT(m_eclipseCase->mainGrid());

  // ---------------------------------------------------------------
  QString egridFileName =
      RIECLFileTools::firstFileNameOfType(m_filesWithSameBaseName,
                                          ECL_EGRID_FILE);

  // ---------------------------------------------------------------
  if (egridFileName.size() > 0) {

    // -------------------------------------------------------------
    ecl_file_type* ecl_file = ecl_file_open(egridFileName.toLatin1().data(),
                                            ECL_FILE_CLOSE_STREAM);
    if (!ecl_file) { return false; }

    // -------------------------------------------------------------
    int actnumKeywordCount = ecl_file_get_num_named_kw(ecl_file, ACTNUM_KW);

    if (actnumKeywordCount > 0) {
      vector<vector<int> > actnumValuesPerGrid;
      actnumValuesPerGrid.resize(actnumKeywordCount);

      size_t reservoirCellCount = 0;
      for (size_t gridIdx = 0;
           gridIdx < static_cast<size_t>(actnumKeywordCount); gridIdx++) {

        RIECLFileTools::keywordData(
            ecl_file, ACTNUM_KW, gridIdx, &actnumValuesPerGrid[gridIdx]);
        reservoirCellCount += actnumValuesPerGrid[gridIdx].size();
      }

      // -----------------------------------------------------------
      // Check if number of cells is matching
      if (m_eclipseCase->mainGrid()->globalCellArray().size()
          != reservoirCellCount) {
        return false;
      }

      // -----------------------------------------------------------
      RIActiveCellInfo* activeCellInfo =
          m_eclipseCase->activeCellInfo(MATRIX_MODEL);
      RIActiveCellInfo* fractureActiveCellInfo =
          m_eclipseCase->activeCellInfo(FRACTURE_MODEL);

      // -----------------------------------------------------------
      activeCellInfo->setReservoirCellCount(reservoirCellCount);
      fractureActiveCellInfo->setReservoirCellCount(reservoirCellCount);

      activeCellInfo->setGridCount(actnumKeywordCount);
      fractureActiveCellInfo->setGridCount(actnumKeywordCount);

      // -----------------------------------------------------------
      size_t cellIdx = 0;
      size_t globalActiveMatrixIndex = 0;
      size_t globalActiveFractureIndex = 0;
      for (size_t gridIdx = 0;
           gridIdx < static_cast<size_t>(actnumKeywordCount); gridIdx++) {
        size_t activeMatrixIndex = 0;
        size_t activeFractureIndex = 0;

        // ---------------------------------------------------------
        vector<int>& actnumValues = actnumValuesPerGrid[gridIdx];

        for (size_t i = 0; i < actnumValues.size(); i++) {

          // -------------------------------------------------------
          if (actnumValues[i] == 1 || actnumValues[i] == 3) {
            activeCellInfo->
                setCellResultIndex(cellIdx, globalActiveMatrixIndex++);
            activeMatrixIndex++;
          }

          // -------------------------------------------------------
          if (actnumValues[i] == 2 || actnumValues[i] == 3) {
            fractureActiveCellInfo->
                setCellResultIndex(cellIdx, globalActiveFractureIndex++);
            activeFractureIndex++;
          }
          cellIdx++;
        }

        // ---------------------------------------------------------
        activeCellInfo->
            setGridActiveCellCounts(gridIdx, activeMatrixIndex);
        fractureActiveCellInfo->
            setGridActiveCellCounts(gridIdx, activeFractureIndex);
      }

      // -----------------------------------------------------------
      activeCellInfo->computeDerivedData();
      fractureActiveCellInfo->computeDerivedData();
    }

    ecl_file_close(ecl_file);

    return true;
  }

  return false;
}


// -----------------------------------------------------------------
// Create results access object (.UNRST or .X0001 ... .XNNNN)
void RIReaderECL::ensureDynamicResultAccessIsPresent() {
  if (m_dynamicResultsAccess.isNull()) {
    auto m_dynamicResultsAccessv =
        RIECLFileTools::createDynamicResultAccess(m_fileName);
    m_dynamicResultsAccess = m_dynamicResultsAccessv;
  }
}

// -----------------------------------------------------------------
// vector<QDateTime> RIReaderECL::allTimeSteps() const
// {
//   vector<QDateTime> steps;
//   if (m_dynamicResultsAccess.notNull())
//   {
//     vector<double> dymmy;
//     m_dynamicResultsAccess->timeSteps(&steps, &dymmy);
//   }

//   return steps;
// }

// -----------------------------------------------------------------
// Get dynamic result at given step index. Will concatenate
// values for the main grid and all sub grids.
// -----------------------------------------------------------------
// bool RIReaderECL::dynamicResult(const QString& result,
//                                            PorosityModelType matrixOrFracture,
//                                            size_t stepIndex,
//                                            vector<double>* values)
// {
//   ensureDynamicResultAccessIsPresent();

//   if (m_dynamicResultsAccess.notNull()) {
//     size_t indexOnFile = timeStepIndexOnFile(stepIndex);

//     vector<double> fileValues;
//     if (!m_dynamicResultsAccess->results(result,
//                                          indexOnFile,
//                                          m_eclipseCase->mainGrid()->gridCount(),
//                                          &fileValues)) {
//       return false;
//     }

//     extractResultValuesBasedOnPorosityModel(matrixOrFracture,
//                                             values, fileValues);
//   }

//   return true;
// }

// -----------------------------------------------------------------
// Get all values of a given static result as doubles
bool
RIReaderECL::staticResult(const QString& result,
                          PorosityModelType matrixOrFracture,
                          vector<double>* values) {
  CVF_ASSERT(values);

  // ---------------------------------------------------------------
  if (result.compare("ACTNUM", Qt::CaseInsensitive) == 0) {
    RIActiveCellInfo* activeCellInfo =
        m_eclipseCase->activeCellInfo(matrixOrFracture);
    values->resize(activeCellInfo->reservoirActiveCellCount(), 1.0);

    return true;
  }

  // ---------------------------------------------------------------
  openInitFile();

  if(m_ecl_init_file) {
    vector<double> fileValues;

    // -------------------------------------------------------------
    size_t numOccurrences = ecl_file_get_num_named_kw(m_ecl_init_file,
                                                      result.toLatin1().data());

    // -------------------------------------------------------------
    size_t i;
    for (i = 0; i < numOccurrences; i++) {

      vector<double> partValues;
      RIECLFileTools::keywordData(m_ecl_init_file,
                                  result, i,
                                  &partValues);
      fileValues.insert(fileValues.end(),
                        partValues.begin(),
                        partValues.end());
    }

    extractResultValuesBasedOnPorosityModel(MATRIX_MODEL,
                                            values, fileValues);
  }

  return true;
}

//------------------------------------------------------------------
void RIReaderECL::extractResultValuesBasedOnPorosityModel(
    PorosityModelType matrixOrFracture,
    vector<double>* destinationResultValues,
    const vector<double>& sourceResultValues) {

  if (sourceResultValues.size() == 0) return;

  RIActiveCellInfo* fracActCellInfo = m_eclipseCase->activeCellInfo(FRACTURE_MODEL);


  if (matrixOrFracture == MATRIX_MODEL &&
      fracActCellInfo->reservoirActiveCellCount() == 0) {
    destinationResultValues->insert(destinationResultValues->end(),
                                    sourceResultValues.begin(),
                                    sourceResultValues.end());
  } else {
    RIActiveCellInfo* actCellInfo = m_eclipseCase->activeCellInfo(MATRIX_MODEL);

    size_t sourceStartPosition = 0;

    for (size_t i = 0; i < m_eclipseCase->mainGrid()->gridCount(); i++) {
      size_t matrixActiveCellCount = 0;
      size_t fractureActiveCellCount = 0;

      actCellInfo->gridActiveCellCounts(i, matrixActiveCellCount);
      fracActCellInfo->gridActiveCellCounts(i, fractureActiveCellCount);

      if (matrixOrFracture == MATRIX_MODEL) {

        destinationResultValues->insert(
            destinationResultValues->end(),
            sourceResultValues.begin() + sourceStartPosition,
            sourceResultValues.begin() + sourceStartPosition + matrixActiveCellCount);

      } else {

        destinationResultValues->insert(
            destinationResultValues->end(),
            sourceResultValues.begin() + sourceStartPosition + matrixActiveCellCount,
            sourceResultValues.begin() + sourceStartPosition + matrixActiveCellCount + fractureActiveCellCount);

      }

      sourceStartPosition += (matrixActiveCellCount + fractureActiveCellCount);
    }
  }
}


// -----------------------------------------------------------------
void RIReaderECL::openInitFile() {
  if (m_ecl_init_file) {
    return;
  }

  // ---------------------------------------------------------------
  QString initFileName =
      RIECLFileTools::firstFileNameOfType(m_filesWithSameBaseName,
                                          ECL_INIT_FILE);

  // ---------------------------------------------------------------
  if (initFileName.size() > 0) {
    m_ecl_init_file = ecl_file_open(initFileName.toLatin1().data(),
                                    ECL_FILE_CLOSE_STREAM);
  }
}

// -----------------------------------------------------------------
void RIReaderECL::transferCoarseningInfo(const ecl_grid_type* eclGrid,
                                         RIGridBase* grid) {

  // ---------------------------------------------------------------
  int coarseGroupCount = ecl_grid_get_num_coarse_groups(eclGrid);

  // ---------------------------------------------------------------
  for (int i = 0; i < coarseGroupCount; i++) {

    // -------------------------------------------------------------
    ecl_coarse_cell_type* coarse_cell = ecl_grid_iget_coarse_group(eclGrid, i);
    CVF_ASSERT(coarse_cell);

    // -------------------------------------------------------------
    size_t i1 = static_cast<size_t>(ecl_coarse_cell_get_i1(coarse_cell));
    size_t i2 = static_cast<size_t>(ecl_coarse_cell_get_i2(coarse_cell));
    size_t j1 = static_cast<size_t>(ecl_coarse_cell_get_j1(coarse_cell));
    size_t j2 = static_cast<size_t>(ecl_coarse_cell_get_j2(coarse_cell));
    size_t k1 = static_cast<size_t>(ecl_coarse_cell_get_k1(coarse_cell));
    size_t k2 = static_cast<size_t>(ecl_coarse_cell_get_k2(coarse_cell));

    grid->addCoarseningBox(i1, i2, j1, j2, k1, k2);
  }
}

//------------------------------------------------------------------
string RIReaderECL::ertGridName(size_t gridNr)
{
  string gridName;
  if (gridNr == 0) {
    gridName = ECL_GRID_GLOBAL_GRID;
  } else {
    CVF_ASSERT(m_eclipseCase);
    CVF_ASSERT(m_eclipseCase->gridCount() > gridNr);
    gridName = m_eclipseCase->grid(gridNr)->gridName();
  }

  return gridName;
}

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
// Modified by M.Bellout on 3/7/18.
//

//╦═╗  ╦  ╔═╗  ╔═╗  ╦    ╔═╗  ╦  ╦    ╔═╗  ╔╦╗  ╔═╗  ╔═╗  ╦    ╔═╗
//╠╦╝  ║  ║╣   ║    ║    ╠╣   ║  ║    ║╣    ║   ║ ║  ║ ║  ║    ╚═╗
//╩╚═  ╩  ╚═╝  ╚═╝  ╩═╝  ╚    ╩  ╩═╝  ╚═╝   ╩   ╚═╝  ╚═╝  ╩═╝  ╚═╝
// =================================================================
RIECLFileTools::RIECLFileTools() {}

// -----------------------------------------------------------------
RIECLFileTools::~RIECLFileTools() { }

// -----------------------------------------------------------------
// Get first occurrence of file of given type in given list of
// filenames, as filename or NULL if not found
QString RIECLFileTools::firstFileNameOfType(const QStringList& fileSet,
                                            ecl_file_enum fileType)
{
  int i;
  for (i = 0; i < fileSet.count(); i++) {
    bool formatted = false;
    int reportNumber = -1;
    if (ecl_util_get_file_type(fileSet.at(i).toLatin1().data(),
                               &formatted,
                               &reportNumber) == fileType)
    {
      return fileSet.at(i);
    }
  }

  return QString::null;
}

// -----------------------------------------------------------------
// Get set of Eclipse files based on an input file and its path
bool RIECLFileTools::findSiblingFilesWithSameBaseName(
    const QString& fullPathFileName, QStringList* baseNameFiles) {

  CVF_ASSERT(baseNameFiles);
  baseNameFiles->clear();

  QString filePath = QFileInfo(fullPathFileName).absoluteFilePath();
  filePath = QFileInfo(filePath).path();
  QString fileNameBase = QFileInfo(fullPathFileName).baseName();

  stringlist_type* eclipseFiles = stringlist_alloc_new();
  ecl_util_select_filelist(filePath.toLatin1().data(),
                           fileNameBase.toLatin1().data(),
                           ECL_OTHER_FILE, false, eclipseFiles);

  int i;
  for (i = 0; i < stringlist_get_size(eclipseFiles); i++) {
    baseNameFiles->append(stringlist_safe_iget(eclipseFiles, i));
  }

  stringlist_free(eclipseFiles);

  return baseNameFiles->count() > 0;
}

// -----------------------------------------------------------------
bool RIECLFileTools::keywordData(ecl_file_type* ecl_file,
                                 const QString& keyword,
                                 size_t fileKeywordOccurrence,
                                 vector<double>* values) {

  ecl_kw_type* kwData =
      ecl_file_iget_named_kw(ecl_file,
                             keyword.toLatin1().data(),
                             static_cast<int>(fileKeywordOccurrence));
  if (kwData) {

    size_t numValues = ecl_kw_get_size(kwData);
    vector<double> doubleData;
    doubleData.resize(numValues);

    ecl_kw_get_data_as_double(kwData, doubleData.data());
    values->insert(values->end(), doubleData.begin(), doubleData.end());

    return true;
  }

  return false;
}

//--------------------------------------------------------------------
bool RIECLFileTools::keywordData(ecl_file_type* ecl_file,
                                 const QString& keyword,
                                 size_t fileKeywordOccurrence,
                                 vector<int>* values) {

  ecl_kw_type* kwData =
      ecl_file_iget_named_kw(ecl_file,
                             keyword.toLatin1().data(),
                             static_cast<int>(fileKeywordOccurrence));
  if (kwData) {
    size_t numValues = ecl_kw_get_size(kwData);

    vector<int> integerData;
    integerData.resize(numValues);

    ecl_kw_get_memcpy_int_data(kwData, integerData.data());
    values->insert(values->end(), integerData.begin(), integerData.end());

    return true;
  }

  return false;
}

// -----------------------------------------------------------------
cvf::ref<RIECLRestartDataAccess>
RIECLFileTools::createDynamicResultAccess(const QString& fileName) {

  QStringList filesWithSameBaseName;
  RIECLFileTools::findSiblingFilesWithSameBaseName(fileName,
                                                   &filesWithSameBaseName);

  cvf::ref<RIECLRestartDataAccess> resultsAccess;

  // Look for unified restart file
  QString unrstFileName = RIECLFileTools::firstFileNameOfType(
      filesWithSameBaseName, ECL_UNIFIED_RESTART_FILE);

//  if (unrstFileName.size() > 0) {
//    resultsAccess = new RIECLUnifiedRestartFileAccess();
//    resultsAccess->setRestartFiles(QStringList(unrstFileName));
//
//  } else {
//    // Look for set of restart files (one file per time step)
//    QStringList restartFiles =
//        RIECLFileTools::filterFileNamesOfType(filesWithSameBaseName,
//                                              ECL_RESTART_FILE);
//    if (restartFiles.size() > 0) {
//      resultsAccess = new RIECLRestartFilesetAccess();
//      resultsAccess->setRestartFiles(restartFiles);
//    }
//  }

  return resultsAccess;
}

// -----------------------------------------------------------------
// Get all files of the given type from the provided list of filenames
QStringList RIECLFileTools::filterFileNamesOfType(
    const QStringList& fileSet, ecl_file_enum fileType) {

  QStringList fileNames;

  int i;
  for (i = 0; i < fileSet.count(); i++) {

    bool formatted = false;
    int reportNumber = -1;
    if (ecl_util_get_file_type(fileSet.at(i).toLatin1().data(),
                               &formatted,
                               &reportNumber) == fileType) {
      fileNames.append(fileSet.at(i));
    }
  }

  return fileNames;
}

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
// Modified by M.Bellout on 3/8/18.
//

// ╦═╗  ╦  ╔═╗  ╔═╗  ╦    ╦═╗  ╔═╗  ╔╦╗  ╦═╗  ╔╦╗  ╔╦╗  ╔═╗  ═╗ ╦
// ╠╦╝  ║  ║╣   ║    ║    ╠╦╝  ╚═╗   ║   ╠╦╝   ║    ║║  ╠═╣  ╔╩╦╝
// ╩╚═  ╩  ╚═╝  ╚═╝  ╩═╝  ╩╚═  ╚═╝   ╩   ╩╚═   ╩   ═╩╝  ╩ ╩  ╩ ╚═
// =================================================================
// Abstract class for results access
RIECLRestartDataAccess::RIECLRestartDataAccess() {
}

//--------------------------------------------------------------------
RIECLRestartDataAccess::~RIECLRestartDataAccess() {
}

//--------------------------------------------------------------------
RIRestartReportKeywords::RIRestartReportKeywords() {
}

//--------------------------------------------------------------------
void RIRestartReportKeywords::appendKeyword(const string& keyword,
                                            size_t itemCount,
                                            int globalIndex) {
  m_keywordNameAndItemCount.push_back(RIKeywordLocation(keyword,
                                                        itemCount,
                                                        globalIndex));
}

//--------------------------------------------------------------------
vector<string>
RIRestartReportKeywords::keywordsWithItemCountFactorOf(
    const vector<size_t>& factorCandidates) {

  vector<string> tmp;

  for (auto uni : uniqueKeywords()) {

    size_t sum = 0;
    for (auto loc : objectsForKeyword(uni)) {
      sum += loc.itemCount();
    }

    bool foundMatch = false;
    size_t i = 0;

    while (i < factorCandidates.size() && !foundMatch) {

      if (sum > 0 && (sum % factorCandidates[i]) == 0) {
        foundMatch = true;
        tmp.push_back(uni);
      }

      i++;
    }
  }

  return tmp;
}

//--------------------------------------------------------------------
vector<pair<string, size_t> >
RIRestartReportKeywords::keywordsWithAggregatedItemCount() {

  vector<pair<string, size_t> > tmp;

  for (auto uni : uniqueKeywords()) {
    size_t sum = 0;

    for (auto loc : objectsForKeyword(uni)) {
      sum += loc.itemCount();
    }

    tmp.push_back(make_pair(uni, sum));
  }

  return tmp;
}

//--------------------------------------------------------------------
vector<RIKeywordLocation>
RIRestartReportKeywords::objectsForKeyword(const string& keyword) {

  vector<RIKeywordLocation> tmp;

  for (auto a : m_keywordNameAndItemCount) {
    if (a.keyword() == keyword) {
      tmp.push_back(a);
    }
  }

  return tmp;
}

//--------------------------------------------------------------------
set<string> RIRestartReportKeywords::uniqueKeywords() {
  set<string> unique;

  for (auto a : m_keywordNameAndItemCount){
    unique.insert(a.keyword());
  }

  return unique;
}

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
// Modified by M.Bellout on 3/8/18.
//

// ╦═╗  ╦  ╔═╗  ╔═╗  ╦    ╦ ╦  ╔╗╔  ╦═╗  ╔═╗  ╔╦╗  ╦═╗  ╔╦╗  ╔═╗  ╦  ╦    ╔═╗  ╔═╗  ═╗ ╦
// ╠╦╝  ║  ║╣   ║    ║    ║ ║  ║║║  ╠╦╝  ╚═╗   ║   ╠╦╝   ║   ╠╣   ║  ║    ║╣   ╠═╣  ╔╩╦╝
// ╩╚═  ╩  ╚═╝  ╚═╝  ╩═╝  ╚═╝  ╝╚╝  ╩╚═  ╚═╝   ╩   ╩╚═   ╩   ╚    ╩  ╩═╝  ╚═╝  ╩ ╩  ╩ ╚═
// =================================================================
// Class for access to results from a unified restart file
// -----------------------------------------------------------------
RIECLUnifiedRestartFileAccess::RIECLUnifiedRestartFileAccess()
    : RIECLRestartDataAccess() {
  m_ecl_file = NULL;
}

RIECLUnifiedRestartFileAccess::~RIECLUnifiedRestartFileAccess() {
  if (m_ecl_file) {
    ecl_file_close(m_ecl_file);
  }
}





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
// Modified by M.Bellout on 3/8/18.
//

// ╦═╗  ╦  ╔═╗  ╔═╗  ╦    ╦═╗  ╔═╗  ╔╦╗  ╦═╗  ╔╦╗  ╔═╗  ╦  ╦    ╔═╗  ╔═╗  ╔═╗  ╔╦╗  ╔═╗  ═╗ ╦
// ╠╦╝  ║  ║╣   ║    ║    ╠╦╝  ╚═╗   ║   ╠╦╝   ║   ╠╣   ║  ║    ║╣   ╚═╗  ║╣    ║   ╠═╣  ╔╩╦╝
// ╩╚═  ╩  ╚═╝  ╚═╝  ╩═╝  ╩╚═  ╚═╝   ╩   ╩╚═   ╩   ╚    ╩  ╩═╝  ╚═╝  ╚═╝  ╚═╝   ╩   ╩ ╩  ╩ ╚═
// =================================================================
// Class for access to results from a set of restart files

