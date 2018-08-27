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
  wicalc_rixx(Grid::Grid *grid,
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

 protected:
  // ---------------------------------------------------------------
  // size_t grid_count_;
  // size_t cell_count_;
  // size_t gcellarray_sz_;

};

}
}

#endif //FIELDOPT_WICALC_RIXX_H
