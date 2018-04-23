/******************************************************************************
   Created by einar on 11/3/16.
   Copyright (C) 2016 Einar J.M. Baumann <einar.baumann@gmail.com>

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
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

// ---------------------------------------------------------
#include <iostream>
#include <iomanip>
#include "GSS.h"
#include "Utilities/math.hpp"

// ---------------------------------------------------------
using std::cout;
using std::endl;

// ---------------------------------------------------------
namespace Optimization {
namespace Optimizers {

// ---------------------------------------------------------
GSS::GSS(Settings::Optimizer *settings,
         Case *base_case,
         Model::Properties::VariablePropertyContainer *variables,
         Reservoir::Grid::Grid *grid,
         Logger *logger
)
    : Optimizer(settings, base_case, variables, grid, logger) {

  // -------------------------------------------------------
  // Initalize GSS
  settings_ = settings;
  realvar_uuid_ = GetTentativeBestCase()->GetRealVarIdVector();
  varcont_ = variables;

  print_dbg_msg_d("[opt]Init. Abs.Class GSS.---- ", 1);

//  // -------------------------------------------------------------
//  // Override constraint_handler_ from optimizer.cpp
//  constraint_handler_ = new Constraints::ConstraintHandler(
//      settings_->constraints(), variables, grid, settings,
//      base_case, logger);

  // -------------------------------------------------------
  set_num_vars(base_case);
  set_contraction_factor();
  set_expansion_factor();
}

// ---------------------------------------------------------
Optimizer::TerminationCondition GSS::IsFinished() {

  TerminationCondition tc = NOT_FINISHED;
  if (case_handler_->CasesBeingEvaluated().size() > 0)
    return tc;
  if (case_handler_->EvaluatedCases().size() >= max_evaluations_)
    tc = MAX_EVALS_REACHED;
  else if (is_converged())
    tc = MINIMUM_STEP_LENGTH_REACHED;

  if (tc != NOT_FINISHED) {
    logger_->AddEntry(this);
    logger_->AddEntry(new Summary(this, tc));
  }
  return tc;
}

// ---------------------------------------------------------
void GSS::expand(vector<int> dirs) {
  print_dbg_msg_d("[opt]GGS: expanding.--------- ", 2);

  // -------------------------------------------------------
  if (dirs[0] == -1) {
    step_lengths_ = step_lengths_ * expan_fac_;

  } else {
    for (int dir : dirs)
      step_lengths_(dir) = step_lengths_(dir) * expan_fac_;
  }
}

// ---------------------------------------------------------
void GSS::contract(vector<int> dirs) {
  print_dbg_msg_d("[opt]GGS: contracting.------- ", 2);

  // -------------------------------------------------------
  if (dirs[0] == -1) {
    step_lengths_ = step_lengths_ * contr_fac_;

  } else {
    for (int dir : dirs)
      step_lengths_(dir) = step_lengths_(dir) * contr_fac_;
  }
}

// ---------------------------------------------------------
QList<Case *> GSS::generate_trial_points(vector<int> dirs) {

  // -------------------------------------------------------
  // Generating sample points
  auto trial_points = QList<Case *>();

  // -------------------------------------------------------
  if (dirs[0] == -1)
    dirs = range(0, (int)directions_.size(), 1);
  print_dbg_msg_d("[opt]Generating trial points. ", 1);

  // -------------------------------------------------------
  // Get base vectors (real)
  VectorXd rea_base = GetTentativeBestCase()->GetRealVarVector();
  print_dbg_msg_d("[opt]GetRealVarVector:------- ", 1, rea_base);

  // Get base vectors (int)
  VectorXi int_base = GetTentativeBestCase()->GetIntegerVarVector();
  print_dbg_msg_i("[opt]GetIntVarVector:-------- ", 1, int_base);

  // -------------------------------------------------------
  // Develop sampling points using base vector
  print_dbg_msg_d("[opt]Perturbations:---------- ", 2);

  // -------------------------------------------------------
  // Set up in settings:
  // Upper-level Optimization section: includes Optimizers
  // + an overall configuration subsection with the following
  // fields:

  for (int dir : dirs) {
    auto trial_point = new Case(GetTentativeBestCase());

    // TEMP SHUT OFF
//    if (int_base.size() > 0) {
//      trial_point->SetIntegerVarValues(perturb(int_base, dir));
//
//    } else if (rea_base.size() > 0) {
      trial_point->SetRealVarValues(perturb(rea_base, dir));
//    }

    // -----------------------------------------------------
    trial_point->set_origin_data(GetTentativeBestCase(),
                                 dir, step_lengths_(dir));

    // -----------------------------------------------------
    // Append to trial points
    trial_points.append(trial_point);
  }

  // -------------------------------------------------------
  // Apply constraints
  print_dbg_msg_d("[opt]Applying constraints.--- ", 1);
  for (Case *c : trial_points)
    constraint_handler_->SnapCaseToConstraints(c);

  return trial_points;
}

// ---------------------------------------------------------
template<typename T> Matrix<T, Dynamic, 1>
GSS::perturb(Matrix<T, Dynamic, 1> base, int dir) {

  // -------------------------------------------------------
  Matrix<T, Dynamic, 1> dirc = directions_[dir].cast<T>();
  T sl = step_lengths_(dir);
  Matrix<T, Dynamic, 1> perturbation = base + dirc * sl;

  // -------------------------------------------------------
  VectorXd vec = perturbation.template cast<double>();
  print_dbg_msg_d("[opt]Perturbations.all:------ ", 2, vec);
  return perturbation;
}

// ---------------------------------------------------------
bool GSS::is_converged() {

  // -------------------------------------------------------
  print_dbg_msg_d("[opt]Conv.check_tol.vector:-- ", 2, step_tol_);
  for (int i = 0; i < step_lengths_.size(); ++i) {
    if (step_lengths_(i) >= step_tol_(i))
      return false;
  }
  return true;
}

// ---------------------------------------------------------
void GSS::set_num_vars(Case* base_case) {

  int numRvars = (int)base_case->GetRealVarVector().size();
  int numIvars = (int)base_case->GetIntegerVarVector().size();
  num_vars_ = numRvars + numIvars;
  if (numRvars > 0 && numIvars > 0)
    cout << ("WARNING: Compass search does not handle both "
        "continuous and discrete variables at the same time\n");
}

// ---------------------------------------------------------
void GSS::set_contraction_factor(){

  contr_fac_ = settings_->parameters().contraction_factor;
  assert(contr_fac_ < 1.0);

  VectorXd vec(1); vec << contr_fac_;
  print_dbg_msg_d("[opt]Contraction factor:----- ", 1, vec);
}

// ---------------------------------------------------------
void GSS::set_expansion_factor(){

  expan_fac_ = settings_->parameters().expansion_factor;
  assert(expan_fac_ >= 1.0);

  VectorXd vec(1); vec << expan_fac_;
  print_dbg_msg_d("[opt]Expansion factor:------- ", 1, vec);
}

// ---------------------------------------------------------
void GSS::set_step_lengths(double len) {
  step_lengths_.fill(len);
}

// ---------------------------------------------------------
void GSS::set_step_lengths() {
  step_lengths_ = Eigen::VectorXd(directions_.size());
  GSS::set_step_vector(settings_->parameters().initial_step_length,
                       settings_->parameters().initial_step_length_xyz,
                       step_lengths_);
}

// ---------------------------------------------------------
void GSS::set_step_tolerances() {
  step_tol_ = Eigen::VectorXd(directions_.size());
  GSS::set_step_vector(settings_->parameters().minimum_step_length,
                       settings_->parameters().minimum_step_length_xyz,
                       step_tol_);
}

// ---------------------------------------------------------
void GSS::set_step_vector(double isval,
                          QList<double> vecxyz,
                          Eigen::VectorXd& st_vec) {

  QList<double> sval; sval << isval;

  // -------------------------------------------------------
  VectorXd vec(3); vec[0] = (double)st_vec.rows();
  vec[1] = (double)vecxyz.size(); vec[2] = (double)sval.size();
  print_dbg_msg_d("[opt]Sz step_length/step_tol: ", 2, vec);

  // -------------------------------------------------------
  // Differentiate positioning of xyz components in var vector
  // based on current base case variable information
  if(vecxyz.length() > 0) {
    int nvar = realvar_uuid_.length();

    // -----------------------------------------------------
    for (int i = 0; i < realvar_uuid_.length(); ++i) {
      auto prop = varcont_->GetContinousVariable(realvar_uuid_[i]);

      // ---------------------------------------------------
      if (prop->propertyInfo().spline_end ==
          Model::Properties::Property::SplineEnd::Heel
          || prop->propertyInfo().spline_end ==
              Model::Properties::Property::SplineEnd::Toe) {

        // -------------------------------------------------
        switch (prop->propertyInfo().coord) {

          // -----------------------------------------------
          case Model::Properties::Property::Coordinate::x:
            st_vec[i] = vecxyz[0];
            st_vec[i+nvar] = vecxyz[0]; break;// x

            // ---------------------------------------------
          case Model::Properties::Property::Coordinate::y:
            st_vec[i] = vecxyz[1];
            st_vec[i+nvar] = vecxyz[1]; break;// y

            // ---------------------------------------------
          case Model::Properties::Property::Coordinate::z:
            st_vec[i] = vecxyz[2];
            st_vec[i+nvar] = vecxyz[2]; break;// z
        }

      } else if (sval.length() == 1) {
        st_vec[i] = sval[0];
        st_vec[i+nvar] = sval[0];
      }
    }

  } else {
    st_vec.fill(sval[0]);
  }

  print_dbg_msg_d("[opt]Step length/tol vector:- ", 2, st_vec);
}

// ---------------------------------------------------------
Case * GSS::dequeue_case_with_worst_origin() {

  auto queued_cases = case_handler_->QueuedCases();
  std::sort(queued_cases.begin(), queued_cases.end(),
            [this](const Case *c1, const Case *c2) -> bool {
              return isBetter(c1, c2);
            });
  case_handler_->DequeueCase(queued_cases.last()->id());
  return queued_cases.last();
}

// ---------------------------------------------------------
void
GSS::print_dbg_msg_i(string dbg_str, int vlevel, VectorXi eigvec) {

  print_dbg_msg_d(dbg_str, vlevel, eigvec.cast<double> ());

}

// ---------------------------------------------------------
void
GSS::print_dbg_msg_d(string dbg_str, int vlevel, VectorXd eigvec) {

  if (settings_->verb_vector()[6] >= vlevel) { // idx:6 -> opt

    // -----------------------------------------------------
    if (   dbg_str == "[opt]Init. Abs.Class GSS.---- "
        || dbg_str == "[opt]Generating trial points. "
        || dbg_str == "[opt]Init. CompassSearch.---- "
        || dbg_str == "[opt]Launching opt.iteration. "
        || dbg_str == "[opt]GGS: expanding.--------- "
        || dbg_str == "[opt]GGS: contracting.------- "
        || dbg_str == "[opt]Perturbations:---------- "
        ) {
      cout << dbg_str << endl;

      // ---------------------------------------------------
    } else if (dbg_str == "[opt]GetRealVarVector:------- "
        || dbg_str == "[opt]GetIntVarVector:-------- ") {
      cout << dbg_str << endl;
      IOFormat CleanFmt(1, 0, "", "", "", "", "[", "]");
      cout << setw(10) << eigvec.format(CleanFmt) << endl;

      // ---------------------------------------------------
    } else if (dbg_str == "[opt]Perturbations.all:------ ") {
      IOFormat CleanFmt(1, 0, "", "", "", "", "[", "]");
      cout << setw(10) << eigvec.format(CleanFmt) << endl;
      cout << fixed << setprecision(8);

      // ---------------------------------------------------
    } else if (dbg_str == "[opt]Conv.check_tol.vector:-- ") {
      cout << dbg_str << endl;
      cout << fixed << setprecision(1);
      IOFormat CleanFmt(1, 0, "", "", "", "", "[", "]");
      int r = (int)eigvec.rows();
      cout << setw(10) << eigvec.block(  0,0,r/2,1).format(CleanFmt) << endl;
      cout << setw(10) << eigvec.block(r/2,0,r/2,1).format(CleanFmt) << endl;
      cout << fixed << setprecision(8);

      // ---------------------------------------------------
    } else if (dbg_str == "[opt]Contraction factor:----- ") {
      cout << fixed << setprecision(8);
      cout << dbg_str << eigvec << endl;
      cout << fixed << setprecision(1);

      // ---------------------------------------------------
    } else if (dbg_str == "[opt]Expansion factor:------- ") {
      cout << fixed << setprecision(8);
      cout << dbg_str << eigvec << endl;
      cout << fixed << setprecision(1);

      // ---------------------------------------------------
    } else if (dbg_str == "[opt]Sz step_length/step_tol: ") {
      cout << "[opt]Sz step_length/step_tol: " << eigvec[0] << endl;
      cout << "[opt]Sz init.stp l_xyz/t_xyz: " << eigvec[1] << endl;
      cout << "[opt]Sz init.stp lenght/tol:- " << eigvec[2] << endl;

      // ---------------------------------------------------
    } else if (dbg_str == "[opt]Step length/tol vector:- ") {
      cout << dbg_str << endl;
      cout << fixed << setprecision(1);
      IOFormat CleanFmt(1, 0, "", "", "", "", "[", "]");
      int r = (int)eigvec.rows();
      cout << setw(10) << eigvec.block(  0,0,r/2,1).format(CleanFmt) << endl;
      cout << setw(10) << eigvec.block(r/2,0,r/2,1).format(CleanFmt) << endl;
      cout << fixed << setprecision(8);

      // ---------------------------------------------------
    } else {
      cout << "[opt]****WRONG DBG.SMG:****** " << dbg_str << endl;
    };

  }

}

}
}
