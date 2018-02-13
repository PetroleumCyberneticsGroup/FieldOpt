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
#include <iostream>
#include <iomanip>
#include "GSS.h"
#include "Utilities/math.hpp"

using std::cout;
using std::endl;

namespace Optimization {
namespace Optimizers {

GSS::GSS(Settings::Optimizer *settings,
         Case *base_case,
         Model::Properties::VariablePropertyContainer *variables,
         Reservoir::Grid::Grid *grid,
         Logger *logger
)
    : Optimizer(settings, base_case, variables, grid, logger) {

  // Initalize GSS
  settings_ = settings;
  realvar_uuid_ = GetTentativeBestCase()->GetRealVarIdVector();
  varcont_ = variables;

  if (settings->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Init. Abs.Class GSS.-----" << endl;

  set_num_vars(base_case);
  set_contraction_factor();
  set_expansion_factor();
}

Optimizer::TerminationCondition GSS::IsFinished()
{
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

void GSS::expand(vector<int> dirs) {
  if (dirs[0] == -1) {
    step_lengths_ = step_lengths_ * expan_fac_;
  }
  else {
    for (int dir : dirs)
      step_lengths_(dir) = step_lengths_(dir) * expan_fac_;
  }
}

void GSS::contract(vector<int> dirs) {
  if (dirs[0] == -1) {
    step_lengths_ = step_lengths_ * contr_fac_;
  }
  else {
    for (int dir : dirs)
      step_lengths_(dir) = step_lengths_(dir) * contr_fac_;
  }
}

QList<Case *> GSS::generate_trial_points(vector<int> dirs) {

  // Generating sample points
  auto trial_points = QList<Case *>();
  if (dirs[0] == -1)
    dirs = range(0, (int)directions_.size(), 1);
  if (settings_->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Generating trial points.-" << endl;

  // Get base vectors
  VectorXi int_base = GetTentativeBestCase()->GetIntegerVarVector();
  VectorXd rea_base = GetTentativeBestCase()->GetRealVarVector();
  if (settings_->verb_vector()[6] >= 1) { // idx:6 -> opt (Optimization)
    cout << "[opt]GetRealVarVector:--------" << endl;
    IOFormat CleanFmt(1, 0, "", "", "", "", "[", "]");
    cout << setw(10) << rea_base.format(CleanFmt) << endl;
  }

  // Develop sampling points using base vector
  if (settings_->verb_vector()[6] >= 2) // idx:6 -> opt (Optimization)
    cout << "[opt]Perturbations:-----------" << endl;
  
  for (int dir : dirs) {
    auto trial_point = new Case(GetTentativeBestCase());
    if (int_base.size() > 0)
      trial_point->SetIntegerVarValues(perturb(int_base, dir));
    else if (rea_base.size() > 0)
      trial_point->SetRealVarValues(perturb(rea_base, dir));

    // Append to trial points
    trial_point->set_origin_data(GetTentativeBestCase(),
                                 dir, step_lengths_(dir));
    trial_points.append(trial_point);
  }

  // Apply constraints
  for (Case *c : trial_points)
    constraint_handler_->SnapCaseToConstraints(c);

  return trial_points;
}

template<typename T>
Matrix<T, Dynamic, 1> GSS::perturb(Matrix<T, Dynamic, 1> base, int dir) {

  Matrix<T, Dynamic, 1> dirc = directions_[dir].cast<T>();
  T sl = step_lengths_(dir);
  Matrix<T, Dynamic, 1> perturbation = base + dirc * sl;

  if (settings_->verb_vector()[6] >= 2) { // idx:6 -> opt (Optimization)
    IOFormat CleanFmt(1, 0, "", "", "", "", "[", "]");
    cout << setw(10) << perturbation.format(CleanFmt) << endl;
    cout << fixed << setprecision(8);
  }

  return perturbation;
}

bool GSS::is_converged() {

  if (settings_->verb_vector()[6] >= 1) { // idx:6 -> opt (Optimization)
    cout << "[opt]Step tol vector:---------" << endl;
    cout << fixed << setprecision(1);
    IOFormat CleanFmt(1, 0, "", "", "", "", "[", "]");
    cout << setw(6) << step_tol_.format(CleanFmt) << endl;
    cout << fixed << setprecision(8);
  }

  for (int i = 0; i < step_lengths_.size(); ++i) {
    if (step_lengths_(i) >= step_tol_(i))
      return false;
  }
  return true;
}

void GSS::set_num_vars(Case* base_case) {
  int numRvars = (int)base_case->GetRealVarVector().size();
  int numIvars = (int)base_case->GetIntegerVarVector().size();
  num_vars_ = numRvars + numIvars;
  if (numRvars > 0 && numIvars > 0)
    cout << ("WARNING: Compass search does not handle both "
        "continuous and discrete variables at the same time");
}

void GSS::set_contraction_factor(){
  contr_fac_ = settings_->parameters().contraction_factor;
  assert(contr_fac_ < 1.0);
  cout << fixed << setprecision(8);
  if (settings_->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Contraction factor:------ " << contr_fac_ << endl;
}

void GSS::set_expansion_factor(){
  expan_fac_ = settings_->parameters().expansion_factor;
  assert(expan_fac_ >= 1.0);
  if (settings_->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Expansion factor:-------- " << expan_fac_ << endl;
  cout << fixed << setprecision(1);
}

void GSS::set_step_lengths(double len) {
  step_lengths_.fill(len);
}

void GSS::set_step_lengths() {
  step_lengths_ = Eigen::VectorXd(directions_.size());
  GSS::set_step_vector(settings_->parameters().initial_step_length,
                       settings_->parameters().initial_step_length_xyz,
                       step_lengths_);
}

void GSS::set_step_tolerances() {
  step_tol_ = Eigen::VectorXd(directions_.size());
  GSS::set_step_vector(settings_->parameters().minimum_step_length,
                       settings_->parameters().minimum_step_length_xyz,
                       step_tol_);
}

void GSS::set_step_vector(double isval,
                          QList<double> vecxyz,
                          Eigen::VectorXd& st_vec) {

  QList<double> sval; sval << isval;
  if (settings_->verb_vector()[6] >= 2) { // idx:6 -> opt (Optimization)
    cout << "[opt]Sz step_length/step_tol_: " << st_vec.rows() << endl;
    cout << "[opt]Sz init.step l_xyz/t_xyz: " << vecxyz.size() << endl;
    cout << "[opt]Sz init.step lenght/tol:- " << sval.size() << endl;
  }

  if(vecxyz.length() > 0) { // differentiate b/e xys coords and other continuous vars
    int nvar = realvar_uuid_.length();
    for (int i = 0; i < realvar_uuid_.length(); ++i) {
      auto prop = varcont_->GetContinousVariable(realvar_uuid_[i]);
      if (prop->propertyInfo().spline_end == Model::Properties::Property::SplineEnd::Heel ||
          prop->propertyInfo().spline_end == Model::Properties::Property::SplineEnd::Toe) {
        switch (prop->propertyInfo().coord) {
          case Model::Properties::Property::Coordinate::x:
            st_vec[i] = vecxyz[0];
            st_vec[i+nvar] = vecxyz[0]; break;// x
          case Model::Properties::Property::Coordinate::y:
            st_vec[i] = vecxyz[1];
            st_vec[i+nvar] = vecxyz[1]; break;// y
          case Model::Properties::Property::Coordinate::z:
            st_vec[i] = vecxyz[2];
            st_vec[i+nvar] = vecxyz[2]; break;// z
        }
      } else if (sval.length() == 1){
        st_vec[i] = sval[0];
        st_vec[i+nvar] = sval[0];
      }
    }
  } else {
    st_vec.fill(sval[0]);
  }

  if (settings_->verb_vector()[6] >= 2) { // idx:6 -> opt (Optimization)
    cout << "[opt]Step lenght/tol vector:--";
    cout << fixed << setprecision(1);
    IOFormat CleanFmt(1, 0, "", "", "", "", "[", "]");
    cout << setw(6) << st_vec.format(CleanFmt) << endl;
    cout << fixed << setprecision(8);
  }

}

Case * GSS::dequeue_case_with_worst_origin() {
  auto queued_cases = case_handler_->QueuedCases();
  std::sort(queued_cases.begin(), queued_cases.end(),
            [this](const Case *c1, const Case *c2) -> bool {
              return isBetter(c1, c2);
            });
  case_handler_->DequeueCase(queued_cases.last()->id());
  return queued_cases.last();
}

}
}
