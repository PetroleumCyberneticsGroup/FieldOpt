/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "c-spln_lngth-interw_dist.h"
#include <iostream>

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// ---------------------------------------------------------
using std::cout;
using std::endl;

// =========================================================
CombinedSplineLengthInterwellDistance::
CombinedSplineLengthInterwellDistance(
    Settings::Optimizer::Constraint settings,
    Model::Properties::VariablePropertyContainer *variables) {

  // -------------------------------------------------------
  max_iterations_ = settings.max_iterations;
  Settings::Optimizer::Constraint dist_constr_settings;
  dist_constr_settings.wells = settings.wells;
  dist_constr_settings.min = settings.min_distance;

  // -------------------------------------------------------
  // Establish (single) interwell interwell distance object
  distance_constraint_ = new InterwellDistance(dist_constr_settings,
                                               variables);

  // -------------------------------------------------------
  if (settings.verb_vector_[6] > 1) { // idx:6 -> opt
    std::cout << "... ... initialized distance constraint for wells: ";
    for (QString wname : settings.wells) {
      std::cout << wname.toStdString() << ", ";
    }
    std::cout << std::endl;
  }

  // -------------------------------------------------------
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

// ---------------------------------------------------------
bool CombinedSplineLengthInterwellDistance::CaseSatisfiesConstraint(Case *c) {

  // -------------------------------------------------------
  if (!distance_constraint_->CaseSatisfiesConstraint(c)){
    return false;
  }

  // -------------------------------------------------------
  for (WellSplineLength *wsl : length_constraints_) {
    if (!wsl->CaseSatisfiesConstraint(c))
      return false;
  }

  return true;
}

// ---------------------------------------------------------
void CombinedSplineLengthInterwellDistance::SnapCaseToConstraints(Case *c) {

  // -------------------------------------------------------
  // Limit the sequential application to a max # of iterations
  for (int i = 0; i < max_iterations_; ++i) {

    // Check if case satisfies constraints
    if (CaseSatisfiesConstraint(c)) {
      return;
    }
    else {

      // ---------------------------------------------------
      // Apply interwell distance constraint
      distance_constraint_->SnapCaseToConstraints(c);

      // ---------------------------------------------------
      // Apply well length constraint to each well sequentially
      for (WellSplineLength *wsl : length_constraints_) {
        wsl->SnapCaseToConstraints(c);
      }

    } // end sequential application of iwd and length constraints

  }

}

}
}
