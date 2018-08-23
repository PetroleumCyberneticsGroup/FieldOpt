/******************************************************************************
   Copyright (C) 2015-2016 Hilmar M. Magnusson <hilmarmag@gmail.com>
   Modified by Einar J.M. Baumann (2016) <einar.baumann@gmail.com>
   Modified by Alin G. Chitu (2016-2017) <alin.chitu@tno.nl, chitu_alin@yahoo.com>
   Modified by Einar J.M. Baumann (2017) <einar.baumann@gmail.com>

   This file and the WellIndexCalculator as a whole is part of the
   FieldOpt project. However, unlike the rest of FieldOpt, the
   WellIndexCalculator is provided under the GNU Lesser General Public
   License.

   WellIndexCalculator is free software: you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation, either version 3 of
   the License, or (at your option) any later version.

   WellIndexCalculator is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with WellIndexCalculator.  If not, see
   <http://www.gnu.org/licenses/>.
******************************************************************************/


//#include <map>
//#include <iostream>
//#include <stdexcept>
//#include <limits>

#include <Utilities/colors.hpp>
#include "wellindexcalculator.h"
#include "tests/wic_debug.hpp"

//using namespace std;

namespace Reservoir {
namespace WellIndexCalculation {
WellIndexCalculator::WellIndexCalculator(Grid::Grid *grid) {

  grid_ = grid;
  auto smallest_cell = grid_->GetSmallestCell();
  smallest_grid_cell_dimension_ = 1e7;

  for (int i = 1; i < smallest_cell.corners().size(); ++i) {
    double distance_between_corners =
        (smallest_cell.corners()[i-1] - smallest_cell.corners()[i]).norm();

    if (distance_between_corners < smallest_grid_cell_dimension_) {
      smallest_grid_cell_dimension_ = distance_between_corners;
    }
  }
}

void WellIndexCalculator::ComputeWellBlocks(
    map<string, vector<IntersectedCell>> &well_indices,
    vector<WellDefinition> &wells,
    int rank) {

  //map<string, vector<IntersectedCell>> well_indices;

  // Perform well block search for each well
  for (int iWell = 0; iWell < wells.size(); ++iWell) {

    // Compute an overall bounding box per well --> speed up cell searching
    double xi, yi, zi, xf, yf, zf;
    xi = numeric_limits<double>::max();
    xf = -numeric_limits<double>::max(); //numeric_limits<double>::lowest();
    yi = numeric_limits<double>::max();
    yf = -numeric_limits<double>::max(); //numeric_limits<double>::lowest();
    zi = numeric_limits<double>::max();
    zf = -numeric_limits<double>::max(); //numeric_limits<double>::lowest();

    // Loop through all segments -> find outermost coordinates for entire well
    for (int iSegment = 0; iSegment < wells[iWell].radii.size(); ++iSegment ) {
      xi = min(xi, min(wells[iWell].heels[iSegment].x(), wells[iWell].toes[iSegment].x()));
      xf = max(xf, max(wells[iWell].heels[iSegment].x(), wells[iWell].toes[iSegment].x()));
      yi = min(yi, min(wells[iWell].heels[iSegment].y(), wells[iWell].toes[iSegment].y()));
      yf = max(yf, max(wells[iWell].heels[iSegment].y(), wells[iWell].toes[iSegment].y()));
      zi = min(zi, min(wells[iWell].heels[iSegment].z(), wells[iWell].toes[iSegment].z()));
      zf = max(zf, max(wells[iWell].heels[iSegment].z(), wells[iWell].toes[iSegment].z()));

      // Debug ------------------------------
      WICDebug::dbg_ComputeWellBlocks_bbox_i(
          dbg_mode, wells, iWell, iSegment, xi, yi, zi, xf, yf, zf, rank);
      // printf("WICDebug::dbg_ComputeWellBlocks_bbox_i.\n");

    }

    // Heuristic increase of the bounding area
    xi = xi - 0.1*(xf-xi); xf = xf + 0.1*(xf-xi);
    yi = yi - 0.1*(yf-yi); yf = yf + 0.1*(yf-yi);
    zi = zi - 0.1*(zf-zi); zf = zf + 0.1*(zf-zi);

    // Debug ------------------------------
    WICDebug::dbg_ComputeWellBlocks_bbox_f(dbg_mode, xi, yi, zi, xf, yf, zf, rank);
    // printf("WICDebug::dbg_ComputeWellBlocks_bbox_f.\n");

    // Get the list of all cells in the bounding box
    vector<int> bb_cells;
    double bb_xi, bb_yi, bb_zi, bb_xf, bb_yf, bb_zf;
    bb_cells = grid_->GetBoundingBoxCellIndices(xi, yi, zi,
                                                xf, yf, zf,
                                                bb_xi, bb_yi, bb_zi,
                                                bb_xf, bb_yf, bb_zf);

    // Debug -------------------------------
    WICDebug::dbg_GetBoundingBoxCellIndices(dbg_mode, bb_cells, rank);

    // Loop through each well segment -> find intersected cells for each segment
    vector<IntersectedCell> intersected_cells;
    for (int iSegment = 0; iSegment < wells[iWell].radii.size(); ++iSegment) {
      collect_intersected_cells(intersected_cells,
                                wells[iWell].heels[iSegment],
                                wells[iWell].toes[iSegment],
                                wells[iWell].radii[iSegment],
                                wells[iWell].skins[iSegment],
                                bb_cells,
                                bb_xi, bb_yi, bb_zi,
                                bb_xf, bb_yf, bb_zf,
                                rank);
    }

    // For all intersected cells compute well transmissibility factor
    for (int iCell = 0; iCell < intersected_cells.size(); ++iCell) {
      compute_well_index(intersected_cells, iCell, rank);
    }

    // Assign intersected cells to well
    well_indices[wells[iWell].wellname] = intersected_cells;
  }

  // Return the objects
  //return well_indices;
}


void WellIndexCalculator::collect_intersected_cells(vector<IntersectedCell> &isc_cells,
                                                    Vector3d start_pt, Vector3d end_pt,
                                                    double wb_rad, double skin_fac,
                                                    vector<int> bb_cells,
                                                    double& bb_xi, double& bb_yi, double& bb_zi,
                                                    double& bb_xf, double& bb_yf, double& bb_zf,
                                                    int rank) {

  string dbg_str;

  // ---------------------------------------------------------------------
  /* Check size of bounding box: If size of bounding box is zero,
   * i.e., if the bounding box consists of no cells, it means the
   * well segment is not inside the reservoir grid (active or
   * logical?).
   */
  bool well_is_outside = (bb_cells.size() == 0);
  if (well_is_outside) {
    WICDebug::dbg_collect_intersected_cells_well_outside_box(dbg_mode, 1, rank);
    return;
  }

  // ---------------------------------------------------------------------
  /* Additionally, check if the segment is outside the bounding box
   * of the reservoir, i.e., the logical grid. If either this or the
   * above is true, the function returns with message.
   */
  bool segment_is_outside = IsLineCompletelyOutsideBox(Vector3d(bb_xi, bb_yi, bb_zi),
                                                       Vector3d(bb_xf, bb_yf, bb_zf),
                                                       start_pt, end_pt);
  if (segment_is_outside) {
    WICDebug::dbg_collect_intersected_cells_well_outside_box(dbg_mode, 2, rank);
    return;
  }

  // ---------------------------------------------------------------------
  // Find the heel and toe cells. Return if it fails.
  Grid::Cell first_cell, last_cell;
  if (!findEndpoint(bb_cells, start_pt, end_pt, first_cell, rank) ||
      !findEndpoint(bb_cells, end_pt, start_pt, last_cell, rank)) {

    WICDebug::dbg_FindHeelToeEndPoints(dbg_mode, first_cell, last_cell, (string)"failure", rank);
    return;
  } else {
    WICDebug::dbg_FindHeelToeEndPoints(dbg_mode, first_cell, last_cell, (string)"success", rank);
  }

  // ---------------------------------------------------------------------
  /* If the first and last blocks are the same, return the block and start+end points */
  if (last_cell.global_index() == first_cell.global_index()) {

    int isc_cell_idx = IntersectedCell::GetIntersectedCellIndex(isc_cells, first_cell);
    cout << FLRED << "isc_cell_idx = " << isc_cell_idx << AEND << endl;
    isc_cells.at(isc_cell_idx).add_new_segment(start_pt, end_pt, wb_rad, skin_fac);

    return;
  }

  // ---------------------------------------------------------------------
  // First cell
  double epsilon = smallest_grid_cell_dimension_ / (1e3 * (start_pt-end_pt).norm());
  Vector3d entry_pt = start_pt;
  double step = 0.0;
  auto prev_cell = first_cell;

  int isc_cell_idx = IntersectedCell::GetIntersectedCellIndex(isc_cells, first_cell);

  // ---------------------------------------------------------------------
  // Make sure we follow line in the correct direction. (i.e. dot product positive)
  Vector3d exit_pt = find_exit_point(isc_cells, isc_cell_idx, start_pt, end_pt, start_pt);

  // Reverse direction?
  if ((end_pt - start_pt).dot(exit_pt - start_pt) <= 0.0) {
    exit_pt = find_exit_point(isc_cells, isc_cell_idx, start_pt, end_pt, exit_pt);
  }

  if (!(exit_pt - start_pt).isMuchSmallerThan(1e-6)) {

    // Add endpoint cell into intersected cell set
    isc_cells.at(isc_cell_idx).add_new_segment(start_pt, exit_pt, wb_rad, skin_fac);
  }


  // ---------------------------------------------------------------------
  // Remaining cells in the middle
  int cycle_count = 0;

  while (step <= 1.0) {

    int steps = 0;

    // -----------------------------------------------------------------
    // Move into the next cell, add it to the list and set the entry point
    step = (exit_pt - start_pt).norm() / (end_pt - start_pt).norm();
    WICDebug::dbg_TraverseLoopStep(dbg_mode, start_pt, end_pt,
                                   exit_pt, (string)"START", rank);

    Reservoir::Grid::Cell new_cell;

    do {
      step += epsilon;
      steps++;

      Vector3d old_entry_pt = entry_pt; // for dbg
      entry_pt = start_pt + step * (end_pt - start_pt);
      WICDebug::dbg_TraversingCellsA(dbg_mode, new_cell, prev_cell, old_entry_pt,
                                     entry_pt, start_pt, end_pt, step, epsilon,
                                     steps, (string)"step-into", rank);

      // OV: 20170709
      if ( !grid_->GetCellEnvelopingPoint(new_cell, entry_pt, bb_cells) ) {
        WICDebug::dbg_TraversingCellsA(dbg_mode, new_cell, prev_cell, old_entry_pt,
                                       entry_pt, start_pt, end_pt, step, epsilon,
                                       steps, (string)"check-failed", rank);
        continue;
      } else {
        WICDebug::dbg_TraversingCellsA(dbg_mode, new_cell, prev_cell, old_entry_pt,
                                       entry_pt, start_pt, end_pt, step, epsilon,
                                       steps, (string)"check-ok", rank);
      }
      // try {
      //     new_cell = grid_->GetCellEnvelopingPoint(entry_pt, bb_cells);
      // }
      // catch (const runtime_error &e) {
      //     continue;
      // }

      // If current cell idx is equal to old cell idx, then we are still in the old cell,
      // therefore, continue stepping into the new cell (if the cell is inactive, continue
      // stepping into new cell until getting into an active cell)

    } while ( (new_cell.global_index() == prev_cell.global_index() ||
        !new_cell.is_active() || !grid_->IndexIsInsideGrid(new_cell.global_index()) )
        && step <= 1.0);

    // -----------------------------------------------------------------
    if (introduces_cycle(isc_cells, new_cell)) {
      recover_from_cycle(isc_cells[isc_cells.size()-1], new_cell, bb_cells,
                         entry_pt, exit_pt, start_pt, end_pt, step, epsilon,
                         cycle_count);
      cycle_count++;
    }

    // -----------------------------------------------------------------
    // Add new cell into existing intersected cell set
    isc_cell_idx = IntersectedCell::GetIntersectedCellIndex(isc_cells, new_cell);

    // -----------------------------------------------------------------
    // If current cell is not equal to the previous cell, and we are not
    // at the last cell either, and we are still stepping into cells, i.e.,
    // step <= 1.0, find the exit point of the current cell
    if (new_cell.global_index() != prev_cell.global_index() &&
        new_cell.global_index() != last_cell.global_index() &&
        step <= 1.0) {

      Vector3d old_exit_pt = exit_pt; // dbg
      exit_pt = find_exit_point(isc_cells, isc_cell_idx, entry_pt, end_pt, exit_pt);

      WICDebug::dbg_TraversingCellsB(dbg_mode, new_cell, prev_cell, last_cell,
                                     step, isc_cells, isc_cell_idx, entry_pt,
                                     end_pt, old_exit_pt, exit_pt, rank);

      isc_cells.at(isc_cell_idx).add_new_segment(entry_pt, exit_pt, wb_rad, skin_fac);
      prev_cell = new_cell;

       start_pt = exit_pt; // TEST

      // ---------------------------------------------------------------------
      // If we've step beyond the original line, or we've already at the last
      // cell, then add the trajectory as a segment, and return
    } else if (step > 1.0 || new_cell.global_index() == last_cell.global_index()) {

      isc_cells.at(isc_cell_idx).add_new_segment(entry_pt, end_pt, wb_rad, skin_fac);

      WICDebug::dbg_TraversingCellsC(dbg_mode, new_cell, last_cell, entry_pt, end_pt,
                                     step, isc_cells, isc_cell_idx, (string)"step-into", rank);

      if (isc_cells.at(isc_cell_idx).global_index() != last_cell.global_index()) {

        WICDebug::dbg_TraversingCellsC(dbg_mode, new_cell, last_cell, entry_pt, end_pt,
                                       step, isc_cells, isc_cell_idx, (string)"check", rank);

        // isc_cells.clear(); // DEBUG!!!
      }
      return;

      // ---------------------------------------------------------------------
    } else if ( (end_pt - start_pt).isMuchSmallerThan(1e-4) ) {

      WICDebug::dbg_TraverseLoopStep(dbg_mode, start_pt, end_pt,
                                     exit_pt, (string)"LOOP", rank);
      return;

      // ---------------------------------------------------------------------
    } else if (new_cell.global_index() == prev_cell.global_index()) {
      // Did not find a new cell:
      /* Either we're still inside the old one, or we've
       * stepped into an inactive cell. Step further. */
      return;
//            continue;
    } else { // We should never end up here.
      stringstream ss;
      ss << "WIC [RANK=" << rank << "]: Something unexpected happened "
          "when trying to find the next intersected cell." << endl;
      throw runtime_error(ss.str());
    }
  } // while (step <= 1.0)
}



void WellIndexCalculator::recover_from_cycle(IntersectedCell &prev_cell,
                                             Grid::Cell &next_cell,
                                             vector<int> bb_cells,
                                             Vector3d &entry_pt,
                                             Vector3d &exit_pt,
                                             Vector3d start_pt,
                                             Vector3d end_pt,
                                             double &step,
                                             double epsilon,
                                             int cycle_count) {

  // ---------------------------------------------------------------------
  Vector3d prev_entry_point = prev_cell.get_segment_entry_point(prev_cell.num_segments()-1);
  Vector3d prev_exit_point = prev_cell.get_segment_exit_point(prev_cell.num_segments()-1);

  // ---------------------------------------------------------------------
  cout << "WIC: Recovering from cycle." << endl;
  cout << "  Old exit point: ("
       << prev_exit_point.x() << ", "
       << prev_exit_point.y() << ", "
       << prev_exit_point.z() << ")\n";
  cout << "  Old next cell: "
       << next_cell.global_index() << " "
       << next_cell.ijk_index().to_string() << endl; // I and J should be +1

  // ---------------------------------------------------------------------
  entry_pt = prev_entry_point;
  step = (prev_entry_point - start_pt).norm() / (end_pt - start_pt).norm();

  // ---------------------------------------------------------------------
  /* Traverse through previous cell to find new exit point; at the
   * same time, find the next cell and its entry point.*/
  do {
    prev_exit_point = entry_pt;
    step += pow(10,cycle_count/6)*epsilon;
    entry_pt = start_pt + step * (end_pt - start_pt);

    // OV: 20170709
    if ( !grid_->GetCellEnvelopingPoint(next_cell, entry_pt, bb_cells) ) {
      continue;
    }
    // try {
    //     next_cell = grid_->GetCellEnvelopingPoint(entry_pt, bb_cells);
    // }
    // catch (const runtime_error &e) {
    /*{
        cout << "WIC: Something unexpected occured when recovering "
             << "from cycle (finding next cell)." << endl;
        throw runtime_error("Error recovering from cycle in WIC.");
    }*/
  } while ((next_cell.global_index() == prev_cell.global_index() ||
      !next_cell.is_active() || !grid_->IndexIsInsideGrid(next_cell.global_index()) )
      && step <= 1.0);

  // ---------------------------------------------------------------------
  /* Update the exit point in the previous cell. */
  prev_cell.update_last_segment_exit_point(prev_exit_point);

  cout << "WIC: New exit point: ("
       << prev_exit_point.x() << ", "
       << prev_exit_point.y() << ", "
       << prev_exit_point.z() << ")\n";
  cout << "WIC: New next cell: "
       << next_cell.global_index() << " "
       << next_cell.ijk_index().to_string() << endl;
}


bool WellIndexCalculator::findEndpoint(const vector<int> &bb_cells,
                                       Vector3d &start_pt,
                                       Vector3d end_point,
                                       Grid::Cell &cell,
                                       int rank) const {

  // ---------------------------------------------------------------------
  // First, traverse the segment until we're inside an active cell.
  double step = 0.0;
  Vector3d org_start_pt = start_pt;
  Vector3d old_start_pt;
  // Set step size to half of the smallest dimension of the smallest grid block
  double epsilon = smallest_grid_cell_dimension_ / (2.0 * (start_pt-end_point).norm());

  // ---------------------------------------------------------------------
  // OV: 20170709
  while (step <= 1.0) {
    if ( grid_->GetCellEnvelopingPoint(cell, start_pt, bb_cells) ) {
      if ( cell.is_active() ) {
        WICDebug::dbg_FindEndPointA(dbg_mode, start_pt, step, cell, bb_cells, rank);
        break; // break while loop
      }
    }

    step += epsilon;
    old_start_pt = start_pt; // for dbg
    start_pt = org_start_pt * (1 - step) + end_point * step;
    WICDebug::dbg_FindEndPointB(dbg_mode, old_start_pt, start_pt, step, (string)"inwards", rank);
  }
  // while (step <= 1.0) {
  //     try
  //     {
  //         cell = grid_->GetCellEnvelopingPoint(start_pt, bb_cells);
  //         if (!cell.is_active())
  //             throw runtime_error("WIC: The cell is inactive.");
  //         break;
  //     }
  //     catch (const runtime_error &e)
  //     {
  //         step += epsilon;
  //         start_pt = org_start_pt * (1 - step) + end_point * step;
  //     }
  // }

  // ---------------------------------------------------------------------
  if (step > 1.0) {
    return false; // Return if we failed to step into the reservoir
  } else if (step == 0.0) {
    return true; // Return if we didn't have to move
  }

  // ---------------------------------------------------------------------
  // Then, traverse back with a smaller step size until we're outside again.
  epsilon = epsilon / 1e2;

  // ---------------------------------------------------------------------
  // OV: 20170709
  while (true) {
    step -= epsilon;
    old_start_pt = start_pt; // for dbg
    start_pt = org_start_pt * (1 - step) + end_point * step;
    WICDebug::dbg_FindEndPointB(dbg_mode, old_start_pt, start_pt, step, "outwards", rank);

    if ( grid_->GetCellEnvelopingPoint(cell, start_pt, bb_cells) ) {
      if ( !cell.is_active() ) {
        break; // break while loop
      }
    } else {
      break; // break while loop
    }
  }
  return true;

  // while (true) {
  //     try
  //     {
  //         step -= epsilon;
  //         start_pt = org_start_pt * (1 - step) + end_point * step;
  //         cell = grid_->GetCellEnvelopingPoint(start_pt, bb_cells);
  //         if (!cell.is_active())
  //             throw runtime_error("WIC: The cell is inactive.");
  //     }
  //     catch (const runtime_error &e)
  //     {
  //         break;
  //     }
  // }
  // return true;
}


Vector3d WellIndexCalculator::find_exit_point(vector<IntersectedCell> &cells, int cell_index,
                                              Vector3d &entry_point, Vector3d &end_point,
                                              Vector3d &exception_point) {
  Vector3d line = end_point - entry_point;

  // ---------------------------------------------------------------------
  // Loop through the cell faces until we find one that the line intersects
  for (Grid::Cell::Face face : cells.at(cell_index).faces()) {
    if (face.normal_vector.dot(line) != 0) {
      // Check that the line and face are not parallel.
      auto intersect_point = face.intersection_with_line(entry_point, end_point);

      // Check that the intersection point is on the
      // correct side of all faces (i.e. inside the cell)
      bool feasible_point = true;
      for (auto p : cells.at(cell_index).faces()) {
        if (!p.point_on_same_side(intersect_point, 10e-6)) {
          feasible_point = false;
          break;
        }
      }

      // Return the point if it is:
      if (feasible_point // deemed feasible:
          && (exception_point - intersect_point).norm() > 10e-4 // not identical to exception point
          && (entry_point - intersect_point).norm() > 10e-4 // not identical to entry point
          && (end_point - entry_point).dot(end_point - intersect_point) >= 0) { // going in the correct direction
        return intersect_point;
      }
    }
  }
  // If all fails, the line intersects the cell in a
  // single point (corner or edge) -> return entry_point
  return entry_point;
}

bool WellIndexCalculator::introduces_cycle(vector<IntersectedCell> cells,
                                           Grid::Cell grdcell) {
  if (cells.size() > 1) {
    if (cells[cells.size()-2].global_index() == grdcell.global_index()) {
      return true;
    } else {
      return false;
    }
  } else return false;
}

//bool WellIndexCalculator::GetIntersection(double fDst1, double fDst2,
//                                          Vector3d P1, Vector3d P2,
//                                          Vector3d &Hit) {
//    if ( (fDst1 * fDst2) >= 0.0f ) {
//        return false;
//    };
//
//    if ( fDst1 == fDst2 ) {
//        return false;
//    };
//
//    Hit = P1 + (P2-P1) * ( -fDst1/(fDst2-fDst1) );
//    return true;
//}

//bool WellIndexCalculator::InBox( Vector3d Hit, Vector3d B1, Vector3d B2, const int Axis)
//{
//    if ( Axis==1 && Hit.z() > B1.z() && Hit.z() < B2.z() && Hit.y() > B1.y() && Hit.y() < B2.y()) return true;
//    if ( Axis==2 && Hit.z() > B1.z() && Hit.z() < B2.z() && Hit.x() > B1.x() && Hit.x() < B2.x()) return true;
//    if ( Axis==3 && Hit.x() > B1.x() && Hit.x() < B2.x() && Hit.y() > B1.y() && Hit.y() < B2.y()) return true;
//    return false;
//}


//// Returns true if line (L1, L2) intersects with the box (B1, B2)
//// Returns intersection point in Hit
//bool WellIndexCalculator::CheckLineBox( Vector3d B1, Vector3d B2,
//                                        Vector3d L1, Vector3d L2,
//                                        Vector3d &Hit)
//{
//    if (L2.x() < B1.x() && L1.x() < B1.x()) return false;
//    if (L2.x() > B2.x() && L1.x() > B2.x()) return false;
//    if (L2.y() < B1.y() && L1.y() < B1.y()) return false;
//    if (L2.y() > B2.y() && L1.y() > B2.y()) return false;
//    if (L2.z() < B1.z() && L1.z() < B1.z()) return false;
//    if (L2.z() > B2.z() && L1.z() > B2.z()) return false;
//
//    if (L1.x() > B1.x() && L1.x() < B2.x() &&
//        L1.y() > B1.y() && L1.y() < B2.y() &&
//        L1.z() > B1.z() && L1.z() < B2.z())
//    {
//        Hit = L1;
//        return true;
//    }
//
//    if ((GetIntersection( L1.x() - B1.x(), L2.x() - B1.x(), L1, L2, Hit) && InBox( Hit, B1, B2, 1 )) ||
//        (GetIntersection( L1.y() - B1.y(), L2.y() - B1.y(), L1, L2, Hit) && InBox( Hit, B1, B2, 2 )) ||
//        (GetIntersection( L1.z() - B1.z(), L2.z() - B1.z(), L1, L2, Hit) && InBox( Hit, B1, B2, 3 )) ||
//        (GetIntersection( L1.x() - B2.x(), L2.x() - B2.x(), L1, L2, Hit) && InBox( Hit, B1, B2, 1 )) ||
//        (GetIntersection( L1.y() - B2.y(), L2.y() - B2.y(), L1, L2, Hit) && InBox( Hit, B1, B2, 2 )) ||
//        (GetIntersection( L1.z() - B2.z(), L2.z() - B2.z(), L1, L2, Hit) && InBox( Hit, B1, B2, 3 ))) {
//        return true;
//    }
//
//    return false;
//}


bool WellIndexCalculator::IsLineCompletelyOutsideBox(Vector3d B1,
                                                     Vector3d B2,
                                                     Vector3d L1,
                                                     Vector3d L2) {
  if (L2.x() < B1.x() && L1.x() < B1.x()) return true;
  if (L2.x() > B2.x() && L1.x() > B2.x()) return true;
  if (L2.y() < B1.y() && L1.y() < B1.y()) return true;
  if (L2.y() > B2.y() && L1.y() > B2.y()) return true;
  if (L2.z() < B1.z() && L1.z() < B1.z()) return true;
  if (L2.z() > B2.z() && L1.z() > B2.z()) return true;
}

void WellIndexCalculator::compute_well_index(vector<IntersectedCell> &cells,
                                             int cell_index, int rank) {
  double well_index_x_matrix = 0;
  double well_index_y_matrix = 0;
  double well_index_z_matrix = 0;

  double well_index_x_fracture = 0;
  double well_index_y_fracture = 0;
  double well_index_z_fracture = 0;

  IntersectedCell &icell = cells.at(cell_index);
  int num_grids = icell.permx().size();

//  icell.global_index()
  icell.set_dx(icell.dxdydz()[0]);
  icell.set_dy(icell.dxdydz()[1]);
  icell.set_dz(icell.dxdydz()[2]);

  if (grid_->IndexIsInsideGrid( icell.global_index() )) {

    for (int iSegment = 0; iSegment < icell.num_segments(); iSegment++) {

      // Compute vector from segment
      Vector3d current_vec = icell.get_segment_exit_point(iSegment) - icell.get_segment_entry_point(iSegment);
      WICDebug::dbg_compute_well_index_check_cell(dbg_mode, cell_index, icell, iSegment, current_vec, "", rank);

      /* Projects segment vector to directional spanning vectors and determines the length.
       * of the projections. Note that we only care about the length of the projection,
       * not the spatial position. Also adds the lengths of previous segments in case there
       * is more than one segment within the well.
       */

      // (*** WRONG FOR Z ***) CURRENT_VECTOR(cv) -- PROJECTED ONTO -- XVEC(xv) / YVEC(yv) / ZVEC(zv) CELL_SIDES
      double current_Lx__cv_onto_xv = (icell.xvec() * icell.xvec().dot(current_vec) / icell.xvec().dot(icell.xvec())).norm();
      double current_Ly__cv_onto_yv = (icell.yvec() * icell.yvec().dot(current_vec) / icell.yvec().dot(icell.yvec())).norm();
      double current_Lz__cv_onto_zv = (icell.zvec() * icell.zvec().dot(current_vec) / icell.zvec().dot(icell.zvec())).norm();

      // XVEC(xv) / YVEC(yv) / ZVEC(zv) CELL_SIDES -- PROJECTED ONTO -- CURRENT_VECTOR(cv)
      double current_Lx__xv_onto_cv = (current_vec * icell.xvec().dot(current_vec) / current_vec.dot(current_vec)).norm();
      double current_Ly__yv_onto_cv = (current_vec * icell.yvec().dot(current_vec) / current_vec.dot(current_vec)).norm();
      double current_Lz__zv_onto_cv = (current_vec * icell.zvec().dot(current_vec) / current_vec.dot(current_vec)).norm();

      // CORRECTION
      double current_Lx = current_Lx__xv_onto_cv;
      double current_Ly = current_Ly__yv_onto_cv;
      double current_Lz = current_Lz__zv_onto_cv;

      // Compute Well Index from formula provided by Shu (\todo Introduce ref/year) per Segment
      // (Note that this has a glich since segments from the same well could have different radius (e.g. radial well))
      vector<double> current_wx;
      vector<double> current_wy;
      vector<double> current_wz;

      for(int igrid = 0; igrid < num_grids; igrid++)
      {
        current_wx.push_back(dir_well_index(current_Lx, icell.dy(), icell.dz(), icell.permy()[igrid], icell.permz()[igrid], icell.get_segment_radius(iSegment), icell.get_segment_skin(iSegment)));
        current_wy.push_back(dir_well_index(current_Ly, icell.dx(), icell.dz(), icell.permx()[igrid], icell.permz()[igrid], icell.get_segment_radius(iSegment), icell.get_segment_skin(iSegment)));
        current_wz.push_back(dir_well_index(current_Lz, icell.dx(), icell.dy(), icell.permx()[igrid], icell.permy()[igrid], icell.get_segment_radius(iSegment), icell.get_segment_skin(iSegment)));
      }

      // Store data for later use
      icell.set_segment_calculation_data(iSegment, "dx", icell.dx());
      icell.set_segment_calculation_data(iSegment, "dy", icell.dy());
      icell.set_segment_calculation_data(iSegment, "dz", icell.dz());

      icell.set_segment_calculation_data(iSegment, "Lx", current_Lx);
      icell.set_segment_calculation_data(iSegment, "Ly", current_Ly);
      icell.set_segment_calculation_data(iSegment, "Lz", current_Lz);

      icell.set_segment_calculation_data(iSegment, "current_Lx__cv_onto_xv", current_Lx__cv_onto_xv);
      icell.set_segment_calculation_data(iSegment, "current_Ly__cv_onto_yv", current_Ly__cv_onto_yv);
      icell.set_segment_calculation_data(iSegment, "current_Lz__cv_onto_zv", current_Lz__cv_onto_zv);

      icell.set_segment_calculation_data(iSegment, "current_Lx__xv_onto_cv", current_Lx__xv_onto_cv);
      icell.set_segment_calculation_data(iSegment, "current_Ly__yv_onto_cv", current_Ly__yv_onto_cv);
      icell.set_segment_calculation_data(iSegment, "current_Lz__zv_onto_cv", current_Lz__zv_onto_cv);

      icell.set_segment_calculation_data_3d(iSegment, "xvec", icell.xvec());
      icell.set_segment_calculation_data_3d(iSegment, "yvec", icell.yvec());
      icell.set_segment_calculation_data_3d(iSegment, "zvec", icell.zvec());
      icell.set_segment_calculation_data_3d(iSegment, "cvec", current_vec);

      if (icell.is_active_matrix())
      {
        icell.set_segment_calculation_data(iSegment, "permx_m", icell.permx()[0]);
        icell.set_segment_calculation_data(iSegment, "permy_m", icell.permy()[0]);
        icell.set_segment_calculation_data(iSegment, "permz_m", icell.permz()[0]);

        icell.set_segment_calculation_data(iSegment, "wx_m", current_wx[0]);
        icell.set_segment_calculation_data(iSegment, "wy_m", current_wy[0]);
        icell.set_segment_calculation_data(iSegment, "wz_m", current_wz[0]);
      }
      if (icell.is_active_fracture())
      {
        int ind = 0;
        if (icell.is_active_matrix()) ind = 1;

        icell.set_segment_calculation_data(iSegment, "permx_f", icell.permx()[ind]);
        icell.set_segment_calculation_data(iSegment, "permy_f", icell.permy()[ind]);
        icell.set_segment_calculation_data(iSegment, "permz_f", icell.permz()[ind]);

        icell.set_segment_calculation_data(iSegment, "wx_f", current_wx[ind]);
        icell.set_segment_calculation_data(iSegment, "wy_f", current_wy[ind]);
        icell.set_segment_calculation_data(iSegment, "wz_f", current_wz[ind]);
      }

      // Compute the sum of well index for each direction.
      // For segments with equal radius this will in the end calculate
      // the well index based on the Shu formula in its original formulation
      if (icell.is_active_matrix())
      {
        well_index_x_matrix += current_wx[0];
        well_index_y_matrix += current_wy[0];
        well_index_z_matrix += current_wz[0];
      }
      if (icell.is_active_fracture())
      {
        int ind = 0;
        if (icell.is_active_matrix()) ind = 1;
        well_index_x_fracture += current_wx[ind];
        well_index_y_fracture += current_wy[ind];
        well_index_z_fracture += current_wz[ind];
      }
    }

    // Compute the combined well index as the Sum the segment
    // Compute Well Index from formula provided by Shu for the
    // entire combined projections (this is the original formulation)
    if (icell.is_active_matrix())
    {
      icell.set_cell_well_index_matrix(sqrt(
          well_index_x_matrix * well_index_x_matrix +
              well_index_y_matrix * well_index_y_matrix +
              well_index_z_matrix * well_index_z_matrix));
    }

    if (icell.is_active_fracture())
    {
      icell.set_cell_well_index_fracture(sqrt(
          well_index_x_fracture * well_index_x_fracture +
              well_index_y_fracture * well_index_y_fracture +
              well_index_z_fracture * well_index_z_fracture));
    }

  }

}

double WellIndexCalculator::dir_well_index(double Lx,
                                           double dy, double dz,
                                           double ky, double kz, double
                                           wellbore_radius, double skin_factor) {
  double silly_eclipse_factor = 0.008527;
  double well_index_i = silly_eclipse_factor * (2 * M_PI * sqrt(ky * kz) * Lx) /
      (log(dir_wellblock_radius(dy, dz, ky, kz) / wellbore_radius) + skin_factor);
  return well_index_i;
}

double WellIndexCalculator::dir_wellblock_radius(double dx, double dy,
                                                 double kx, double ky) {
  double r = 0.28 * sqrt((dx * dx) * sqrt(ky / kx) + (dy * dy) * sqrt(kx / ky)) /
      (sqrt(sqrt(kx / ky)) + sqrt(sqrt(ky / kx)));
  return r;
}

void WellDefinition::ReadWellsFromFile(string file_path, vector<WellDefinition>& wells)
{
  ifstream infile(file_path);
  string previous_well_name = "";
  string well_name;
  double hx, hy, hz, tx, ty, tz;
  double radius;
  double skin_factor;
  //try
//    {
  while (infile >> well_name >> hx >> hy >> hz >> tx >> ty >> tz >> radius >> skin_factor) {
    if (previous_well_name != well_name) {
      wells.push_back(WellDefinition());
      wells.back().wellname = well_name;

      previous_well_name = well_name;
    }

    wells.back().heels.push_back(Eigen::Vector3d(hx,hy,hz));
    wells.back().toes.push_back(Eigen::Vector3d(tx,ty,tz));
    wells.back().radii.push_back(radius);
    wells.back().skins.push_back(skin_factor);
  }
//    }
}
}
}
