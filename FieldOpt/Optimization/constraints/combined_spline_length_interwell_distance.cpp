
// ---------------------------------------------------------------
#include "combined_spline_length_interwell_distance.h"
#include <iostream>

// ---------------------------------------------------------------
namespace Optimization {
namespace Constraints {

// ---------------------------------------------------------------
CombinedSplineLengthInterwellDistance::
CombinedSplineLengthInterwellDistance(
    Settings::Optimizer::Constraint settings,
    Model::Properties::VariablePropertyContainer *variables) {

  // -------------------------------------------------------------
  max_iterations_ = settings.max_iterations;
  Settings::Optimizer::Constraint dist_constr_settings;
  dist_constr_settings.wells = settings.wells;
  dist_constr_settings.min = settings.min_distance;

  // -------------------------------------------------------------
  // Establish (single) interwell interwell distance object
  distance_constraint_ = new InterwellDistance(dist_constr_settings,
                                               variables);

  // -------------------------------------------------------------
  if (verbosity_level_>2){
    std::cout << "... ... initialized distance constraint for wells: ";
    for (QString wname : settings.wells) {
      std::cout << wname.toStdString() << ", ";
    }
    std::cout << std::endl;
  }

  // -------------------------------------------------------------
  // Establish QList of length constraint objects
  length_constraints_ = QList<WellSplineLength *>();

  // Loop through each well
  for (QString wname : settings.wells) {

    Settings::Optimizer::Constraint len_constr_settings;
    len_constr_settings.well = wname;
    len_constr_settings.min = settings.min_length;
    len_constr_settings.max = settings.max_length;

    // Fill QList with length constraints
    length_constraints_.append(new WellSplineLength(len_constr_settings,
                                                    variables));

  }
}

// ---------------------------------------------------------------
bool CombinedSplineLengthInterwellDistance::CaseSatisfiesConstraint(Case *c) {

  // -------------------------------------------------------------
  if (!distance_constraint_->CaseSatisfiesConstraint(c)){
    return false;
  }

  // -------------------------------------------------------------
  for (WellSplineLength *wsl : length_constraints_) {
    if (!wsl->CaseSatisfiesConstraint(c))
      return false;
  }

  return true;
}

// ---------------------------------------------------------------
void CombinedSplineLengthInterwellDistance::SnapCaseToConstraints(Case *c) {

  // -------------------------------------------------------------
  // Limit the sequential application to a max # of iterations
  for (int i = 0; i < max_iterations_; ++i) {

    // Check if case satisfies constraints
    if (CaseSatisfiesConstraint(c)) {
      return;
    }
    else {

      // ---------------------------------------------------------
      // Apply interwell distance constraint
      distance_constraint_->SnapCaseToConstraints(c);

      // ---------------------------------------------------------
      // Apply well length constraint to each well sequentially
      for (WellSplineLength *wsl : length_constraints_) {
        wsl->SnapCaseToConstraints(c);
      }

    } // end sequential application of iwd and length constraints

  }

}

}
}
