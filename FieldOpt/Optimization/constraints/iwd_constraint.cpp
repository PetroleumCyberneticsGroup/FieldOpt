/******************************************************************************
   Copyright (C) 2017 Mathias C. Bellout <mathias.bellout@ntnu.no>
   Created by bellout on 22/3/18.

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt. If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

// ---------------------------------------------------------
#include "iwd_constraint.h"
#include "../optimizers/SNOPTSolver.h"

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// ---------------------------------------------------------
IWDConstraint::IWDConstraint(
    Settings::Optimizer* settings,
    Model::Properties::VariablePropertyContainer *variables,
    ::Reservoir::Grid::Grid *grid,
    RICaseData *RICaseData,
    RIReaderECL *RIReaderECL,
    RIGrid *RIGrid) {

  // -------------------------------------------------------
  settings_ = settings;
  variables_ = variables;

  if(RICaseData == nullptr) {
    cout << "[iwd]RICaseData is null!.---- " << endl;
  }

  // -------------------------------------------------------
  SNOPTSolver_ =
      new Optimization::Optimizers::SNOPTSolver(settings_,
                                                current_case_,
                                                grid,
                                                RICaseData,
                                                RIReaderECL,
                                                RIGrid);

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
