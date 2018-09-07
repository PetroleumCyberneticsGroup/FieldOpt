/***********************************************************
 Copyright (C) 2017
 Mathias C. Bellout <mathias.bellout@ntnu.no>

 Created by bellout on 5/6/18.

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it
 and/or modify it under the terms of the GNU General
 Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at
 your option) any later version.

 FieldOpt is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#ifndef FIELDOPT_WICALC_RIXX_H
#define FIELDOPT_WICALC_RIXX_H

// ---------------------------------------------------------
// FieldOpt::RESINXX
#include "resinxx/well_path.h"
#include "WellDefinition.h"

// ---------------------------------------------------------
namespace Reservoir {
namespace WellIndexCalculation {

// ---------------------------------------------------------
using std::cout;
using std::endl;
using std::string;
using std::vector;

//==========================================================
class wicalc_rixx
{
 public:
  // -------------------------------------------------------
  wicalc_rixx(Grid::Grid *grid = nullptr,
              RICaseData *ricasedata = nullptr);

  // -------------------------------------------------------
  ~wicalc_rixx();

  // -------------------------------------------------------
  Settings::Model::Well well_settings_;
  vector<double> intersections_;
  Grid::Grid* grid_;

  // -------------------------------------------------------
  // OV
  cvf::ref<RICaseData> ricasedata_;
  // RICaseData* ricasedata_; // old

  // RIReaderECL rireaderecl_;
  // RIReaderECL* rireaderecl_; // old

  // Experimental
  // cvf::ref<RICaseData> ricasedatac_;
  // cvf::ref<RICaseData> ricasedatap_;
  // RIGrid* rigrid_;

  // ---------------------------------------------------------------
  // WellPath *wellPath_;
  const RIActiveCellInfo* activeCellInfo_;
  //const RIActiveCellInfo* fractureActiveCellInfo_;

  // ---------------------------------------------------------------
  void collectIntersectedCells(vector<IntersectedCell> &isc_cells,
                               vector<WellPathCellIntersectionInfo> isc_info,
                               WellDefinition well,
                               WellPath& wellPath);

  // ---------------------------------------------------------------
  void calculateWellPathIntersections(const WellPath& wellPath,
                                      vector<double> &isc_values);

  // ---------------------------------------------------------------
  void ComputeWellBlocks(vector<IntersectedCell> &well_indices,
                         WellDefinition &well);

  /*!
   * @brief Check if a grid has been read into an RICaseData object.
   * @param path Grid path to check.
   */
  bool HasGrid(string path);

  /*!
   * @brief Create a new RICaseData object for a grid and save it in the grids_ member.
   * @param grid Grid to add.
   */
  void AddGrid(Grid::Grid *grid);

  /*!
   * @brief Set a grid as the currently active one (i.e. set the ricasedata_ and grid_ variables)
   */
  void SetGridActive(Grid::Grid *grid);

 protected:
  // ---------------------------------------------------------------
  // size_t grid_count_;
  // size_t cell_count_;
  // size_t gcellarray_sz_;

 private:
  map<string, cvf::ref<RICaseData>> dict_casedata_;
  map<string, Grid::Grid*> dict_grids_;
  map<string, vector<double>> dict_intersections_;

};

}
}

#endif //FIELDOPT_WICALC_RIXX_H
