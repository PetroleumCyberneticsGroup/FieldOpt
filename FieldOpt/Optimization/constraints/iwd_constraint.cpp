/***********************************************************
 Copyright (C) 2017
 Mathias C. Bellout <mathias.bellout@ntnu.no>

 Created by bellout on 22/3/18.

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
#include "iwd_constraint.h"
#include "../optimizers/SNOPTSolver.h"

#include <FieldOpt-WellIndexCalculator/resinxx/rixx_prj_viz/RivIntersectionGeometryGenerator.h>

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// =========================================================
IWDConstraint::IWDConstraint(
    Settings::Optimizer* settings,
    Model::Properties::VariablePropertyContainer *variables,
    ::Reservoir::Grid::Grid *grid,
    RICaseData *ricasedata) {

  // -------------------------------------------------------
  settings_ = settings;
  variables_ = variables;

  // -------------------------------------------------------
  if(ricasedata == nullptr) {
    cout << "[iwd]RICaseData is null!.---- " << endl;
  }

  // -------------------------------------------------------
  // Force compute geometric bb
  ricasedata_ = ricasedata;
  ricasedata_->computeActiveCellBoundingBoxes();

  // -------------------------------------------------------
  // Use RIGrid from now on
  rigrid_ = ricasedata_->mainGrid();
  rigrid_->computeCachedData();
  rigrid_->calculateFaults(
      ricasedata_->activeCellInfo(MATRIX_MODEL));

  // -------------------------------------------------------
  bbgrid_ = ricasedata_->activeCellInfo(
      MATRIX_MODEL)->geometryBoundingBox();

  // -------------------------------------------------------
  // Dbg
  if (settings_->verb_vector()[5] > 3) { // idx:5 -> mod

    // -----------------------------------------------------
    cout << endl << fstr("[mod]bbgrid_.debugString()",5)
         << bbgrid_.debugString().toStdString() << endl;
    cout << fstr("abb.extent():")
         << show_Ved3d("", bbgrid_.extent()) << endl;

    // -----------------------------------------------------
    stringstream istr;
    cvf::Vec3d bbgrid_cornerVerts[8];
    bbgrid_.cornerVertices(bbgrid_cornerVerts);

    // -----------------------------------------------------
    if (settings_->verb_vector()[5] > 4) { // idx:5 -> mod
      for (int j = 0; j < 8; j++) {
        istr << "bbgrid_.cornerVertices[" << j << "]:";
        cout << fstr(istr.str(), 5)
             << show_Ved3d("", bbgrid_cornerVerts[j], false)
             << endl;
        istr.str("");
      }
    }
  }

  // -------------------------------------------------------
  SNOPTSolver_ =
      new Optimization::Optimizers::SNOPTSolver(settings_,
                                                current_case_,
                                                grid,
                                                ricasedata);


  if(0) {

    // -------------------------------------------------------------
    std::vector<cvf::Vec3d> ccv, ccc;
    size_t idx;
//  for (idx = 0; idx < ricasedata_->mainGrid()->cellCount(); idx++) {
    for (idx = 0; idx < 4; idx++) {

      size_t i, j, k;
      rigrid_->ijkFromCellIndex(idx, &i, &j, &k);
      // ricasedata_->mainGrid()->ijkFromCellIndex(idx, &i, &j, &k);
      cout << "i:" << i << " j:" << j << " k:" << k << endl;
      //cout << ricasedata_->mainGrid()->cellCentroid(idx).x();

      std::array<cvf::Vec3d, 8> hc;
      rigrid_->cellCornerVertices(idx, hc.data());
      // ricasedata_->mainGrid()->cellCornerVertices(idx, hc.data());
      cout << "hc_x:" << hc[0].x() << " hc_y:" << hc[0].y() << " hc_z:" << hc[0].z() << endl;
      ccc.push_back(hc[0]);

      cvf::Vec3d cc = rigrid_->cell(idx).center();
      // cvf::Vec3d cc = ricasedata_->mainGrid()->cell(idx).center();
      cout << "cc_x:" << cc.x() << " cc_y:" << cc.y() << " cc_z:" << cc.z() << endl;
      ccv.push_back(cc);

    }

    std::array<cvf::Vec3d, 8> hc;
    rigrid_->cellCornerVertices(0, hc.data());
    // ricasedata_->mainGrid()->cellCornerVertices(0, hc.data());
    ccc.push_back(hc[0]);





    // -------------------------------------------------------
    rimintersection_ = new RimIntersection(rigrid_,
                                           ricasedata_,
                                           settings);

//   95 25 -2005
//   5 55 -2005
//  cvf::Vec3d p1 = cvf::Vec3d(95, 25, -2005);
//  cvf::Vec3d p2 = cvf::Vec3d(5, 55, -2005);
//  rimintersection_->appendPointToPolyLine(p1);
//  rimintersection_->appendPointToPolyLine(p2);

//  95 35 -2005
//  35 5 -2005
//  5 35 -2005
//  35 95 -2005
//  95 35 -2005

    cvf::Vec3d p1 = cvf::Vec3d(95, 35, -2005);
    cvf::Vec3d p2 = cvf::Vec3d(35, 5, -2005);
    cvf::Vec3d p3 = cvf::Vec3d(5, 35, -2005);
    cvf::Vec3d p4 = cvf::Vec3d(35, 95, -2005);
    cvf::Vec3d p5 = cvf::Vec3d(95, 35, -2005);
    rimintersection_->appendPointToPolyLine(p1);
    rimintersection_->appendPointToPolyLine(p2);
    rimintersection_->appendPointToPolyLine(p3);
    rimintersection_->appendPointToPolyLine(p4);
    rimintersection_->appendPointToPolyLine(p5);

//  for (int ii=0; ii < 2; ++ii) {
//    cout << "Polypoint " << ii <<  ": ";
//    cout << ccc[ii].x() << " " << ccc[ii].y() << " " << ccc[ii].z() << endl;
//    // cout << "appendPointToPolyLine(ccc[" << ii << "])" << endl;
//    rimintersection_->appendPointToPolyLine(ccc[ii]);
//  }

    RivIntersectionPartMgr *imgr =
        rimintersection_->intersectionPartMgr();

    RivIntersectionGeometryGenerator *
        icsec = imgr->getCrossSectionGenerator();

    size_t vx_count = icsec->m_cellBorderLineVxes.p()->size();

    for (size_t ivx = 0; ivx < vx_count; ivx++) {
      auto vx_x = icsec->m_cellBorderLineVxes.p()->val(ivx).x();
      auto vx_y = icsec->m_cellBorderLineVxes.p()->val(ivx).y();
      auto vx_z = icsec->m_cellBorderLineVxes.p()->val(ivx).z();
      cout << "vx_x: " << vx_x << " "
           << "vx_y: " << vx_y << " "
           << "vx_z: " << vx_z << endl;
    }

    print_ri_hck_vec3f("", "", "", cvf::Vec3f::ZERO, true, false);
    for (size_t ivx = 0; ivx < vx_count; ivx++) {
      print_ri_hck_vec3f("", "", "",
                         icsec->m_cellBorderLineVxes.p()->val(ivx));
    }


    // -------------------------------------------------------
    if (settings->verb_vector()[5] > 1) // idx:5 -> mod (Model)
      cout << fstr("[mod]Init RIGrid_.", 5) << "RICell& cell" << endl;

    // size_t cellcount = rigrid_->cellCount();


  }




//  const RICell& cell = rigrid_->globalCellArray()[cellcount];
//  // size_t cellcount = ricasedata_->mainGrid()->cellCount();
//  // const RICell& cell = ricasedata_->mainGrid()->globalCellArray()[cellcount];
//
//  cout << "volume:" << cell.volume() << " count:" << cellcount<< endl;
//
//  cvf::Vec3d startp = rigrid_->cell(0).center();
//  cvf::Vec3d endp = rigrid_->cell(cellcount-1).center();
//  // cvf::Vec3d startp = ricasedata_->grid(0)->cell(0).center();
//  // cvf::Vec3d endp = ricasedata_->grid(0)->cell(cellcount-1).center();
//
//  cout << "x:" << startp.x() << " y:" << startp.y() << " z:" << startp.z() << endl;
//  cout << "x:" << endp.x() << " y:" << endp.y() << " z:" << endp.z() << endl;
//


  // -------------------------------------------------------
  // BOUNDING BOX EXPERIMENTS

  // ....................................................
//  cvf::BoundingBox bb;
//  bb.add(startp);
//  bb.add(endp);
//
//  cout << endl << fstr("[mod]bb.debugString()",5)
//       << bb.debugString().toStdString() << endl;
//  cout << fstr("bb.extent():")
//       << show_Ved3d("", bb.extent()) << endl;
//
//  // cvf::BoundingBox bbg = ricasedata_->mainGrid()->boundingBox();
//  cvf::BoundingBox bbg = rigrid_->boundingBox();
//
//  cout << fstr("[mod]bbg.debugString()",5)
//       << bbg.debugString().toStdString() << endl;
//
//  cout << fstr("bbg.extent():")
//       << show_Ved3d("", bbg.extent()) << endl;


  // ....................................................
  // FIND TIGHT GRID BB
  // Find grid extremities
//  auto gmincoord = rigrid_->minCoordinate();
//  auto gmaxcoord = rigrid_->maxCoordinate();
//
//  cvf::BoundingBox bbminmax = rigridbase_->boundingBox();
////  bbminmax.add(gmincoord);
////  bbminmax.add(gmaxcoord);
//
////  cout << fstr("[mod]rigrid_->maxCoordinate()",5)
////       << show_Ved3d("", gmaxcoord) << endl;
////
////  cout << fstr("[mod]rigrid_->minCoordinate()",5)
////       << show_Ved3d("", gmincoord) << endl;
//
//  cout << endl << fstr("[mod]bbminmax.debugString()",5)
//       << bbminmax.debugString().toStdString() << endl;
//  cout << fstr("bbminmax.extent():")
//       << show_Ved3d("", bbminmax.extent()) << endl;
//
////  for (size_t ii=0; ii < cellcount; ++ii) {
////
////  }














}

// ---------------------------------------------------------
bool IWDConstraint::CaseSatisfiesConstraint(Case *current_case) {

//  // -------------------------------------------------------
//  if (!distance_constraint_->CaseSatisfiesConstraint(c)){
//    return false;
//  }
//
//  // -------------------------------------------------------
//  for (WellSplineLength *wsl : length_constraints_) {
//    if (!wsl->CaseSatisfiesConstraint(c))
//      return false;
//  }
//
  return true;
}

// ---------------------------------------------------------
void IWDConstraint::SnapCaseToConstraints(Case *current_case) {

  // -------------------------------------------------------
  // Limit the sequential application to a max # of iterations
//  for (int i = 0; i < max_iterations_; ++i) {
//
//    // Check if case satisfies constraints
//    if (CaseSatisfiesConstraint(current_case)) {
//      return;
//    }
//    else {




  // ---------------------------------------------------
  // Apply interwell distance constraint
//      distance_constraint_->SnapCaseToConstraints(c);

  // ---------------------------------------------------
  // Apply well length constraint to each well sequentially
//      for (WellSplineLength *wsl : length_constraints_) {
//        wsl->SnapCaseToConstraints(c);
//      }

//    } // end sequential application of iwd and length constraints

//  }

}

// ---------------------------------------------------------

}
}
