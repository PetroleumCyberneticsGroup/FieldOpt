//
// Created by bellout on 3/22/18.
//

// ---------------------------------------------------------
#include "iwd_constraint.h"

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// ---------------------------------------------------------
IWDConstraint::IWDConstraint(
    Settings::Optimizer* settings,
    Model::Properties::VariablePropertyContainer *variables) {

  // -------------------------------------------------------
  settings_ = settings;
  variables_ = variables;

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


      // -------------------------------------------------------
      SNOPTSolver_ =
          new Optimization::Optimizers::SNOPTSolver(
              settings_,
              current_case_,
              variables_,
              grid_,
              logger_);

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
