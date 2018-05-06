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
#include <iostream>
#include <iomanip>
#include "compass_search.h"
#include "gss_patterns.hpp"

// ---------------------------------------------------------
using std::cout;
using std::endl;

// ---------------------------------------------------------
namespace Optimization {
namespace Optimizers {

// =========================================================
CompassSearch::CompassSearch(
    Settings::Optimizer *settings,
    Case *base_case,
    Model::Properties::VariablePropertyContainer *variables,
    Reservoir::Grid::Grid *grid,
    Logger *logger,
    RICaseData *ricasedata)
    : GSS(settings, base_case, variables, grid, logger, ricasedata) {

  // -------------------------------------------------------
  directions_ = GSSPatterns::Compass(num_vars_);
  GSS::print_dbg_msg_d("[opt]Init. CompassSearch.---- ", 1);

  // -------------------------------------------------------
  set_step_lengths();
  assert(step_lengths_.size() == directions_.size());

  // -------------------------------------------------------
  set_step_tolerances();
  assert(step_tol_.size() == step_lengths_.size());

}

// =========================================================
void CompassSearch::iterate() {

  // -------------------------------------------------------
  GSS::print_dbg_msg_d("[opt]Launching opt.iteration. ", 1);
  if (!is_successful_iteration() && iteration_ != 0) {
    contract();
  }

  // -------------------------------------------------------
  case_handler_->AddNewCases(generate_trial_points());
  case_handler_->ClearRecentlyEvaluatedCases();
  iteration_++;
}

// =========================================================
QString CompassSearch::GetStatusStringHeader() const {

  // -------------------------------------------------------
  if(settings_->parameters().
      initial_step_length_xyz.length() > 0) {

    // -----------------------------------------------------
    return QString("%1,%2,%3,%4,%5,%6,%7,%8,%9")
        .arg("Iteration")
        .arg("EvaluatedCases")
        .arg("QueuedCases")
        .arg("RecentlyEvaluatedCases")
        .arg("TentativeBestCaseID")
        .arg("TentativeBestCaseOFValue")
        .arg("StepLengthX")
        .arg("StepLengthY")
        .arg("StepLengthZ");

  } else {

    // -----------------------------------------------------
    return QString("%1,%2,%3,%4,%5,%6,%7")
        .arg("Iteration")
        .arg("EvaluatedCases")
        .arg("QueuedCases")
        .arg("RecentlyEvaluatedCases")
        .arg("TentativeBestCaseID")
        .arg("TentativeBestCaseOFValue")
        .arg("StepLength");

  }
}

// =========================================================
QString CompassSearch::GetStatusString() const {

  // -------------------------------------------------------
  if(settings_->parameters().initial_step_length_xyz.length() > 0) {

    vector<double> stepx, stepy, stepz;

    // -----------------------------------------------------
    for (int i = 0; i < realvar_uuid_.length(); ++i) {

      auto prop = varcont_->GetContinousVariable(realvar_uuid_[i]);

      // ---------------------------------------------------
      if (prop->propertyInfo().spline_end ==
          Model::Properties::Property::SplineEnd::Heel ||
          prop->propertyInfo().spline_end ==
              Model::Properties::Property::SplineEnd::Toe) {

        // -------------------------------------------------
        switch (prop->propertyInfo().coord) {

          // -----------------------------------------------
          case Model::Properties::Property::Coordinate::x:
            stepx.push_back(step_lengths_[i]);
            break; // x

            // ---------------------------------------------
          case Model::Properties::Property::Coordinate::y:
            stepy.push_back(step_lengths_[i]);
            break; // y

            // ---------------------------------------------
          case Model::Properties::Property::Coordinate::z:
            stepz.push_back(step_lengths_[i]);
            break; // z
        }
      }
    }

    // -----------------------------------------------------
    return QString("%1,%2,%3,%4,%5,%6,%7,%8,%9")
        .arg(iteration_)
        .arg(nr_evaluated_cases())
        .arg(nr_queued_cases())
        .arg(nr_recently_evaluated_cases())
        .arg(GetTentativeBestCase()->id().toString())
        .arg(GetTentativeBestCase()->objective_function_value())
        .arg(stepx[0])
        .arg(stepy[0])
        .arg(stepz[0]);

  } else {

    // -----------------------------------------------------
    return QString("%1,%2,%3,%4,%5,%6,%7")
        .arg(iteration_)
        .arg(nr_evaluated_cases())
        .arg(nr_queued_cases())
        .arg(nr_recently_evaluated_cases())
        .arg(GetTentativeBestCase()->id().toString())
        .arg(GetTentativeBestCase()->objective_function_value())
        .arg(step_lengths_[0]);
  }
}

// =========================================================
void CompassSearch::handleEvaluatedCase(Case *c) {
  if (isImprovement(c))
    updateTentativeBestCase(c);
}

// =========================================================
bool CompassSearch::is_successful_iteration() {
  return case_handler_->
      RecentlyEvaluatedCases().contains(GetTentativeBestCase());
}

}
}
