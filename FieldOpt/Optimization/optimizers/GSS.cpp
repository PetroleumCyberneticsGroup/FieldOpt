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

  settings_ = settings;
  if (settings->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Init. Abs.Class GSS.-----" << endl;

  int numRvars = (int)base_case->GetRealVarVector().size();
  int numIvars = (int)base_case->GetIntegerVarVector().size();
  num_vars_ = numRvars + numIvars;
  if (numRvars > 0 && numIvars > 0)
    cout << ("WARNING: Compass search does not handle both "
        "continuous and discrete variables at the same time");

  contr_fac_ = settings->parameters().contraction_factor;
  assert(contr_fac_ < 1.0);
  cout << fixed << setprecision(8);
  if (settings->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Contraction factor:------ " << contr_fac_ << endl;

  expan_fac_ = settings->parameters().expansion_factor;
  assert(expan_fac_ >= 1.0);
  if (settings->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Expansion factor:-------- " << expan_fac_ << endl;
  cout << fixed << setprecision(1);

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

  if (settings_->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Generating trial points.-" << endl;

  auto trial_points = QList<Case *>();
  if (dirs[0] == -1)
    dirs = range(0, (int)directions_.size(), 1);

  VectorXi int_base = GetTentativeBestCase()->GetIntegerVarVector();
  VectorXd rea_base = GetTentativeBestCase()->GetRealVarVector();

  QList<QUuid> idv = GetTentativeBestCase()->GetRealVarIdVector();
  GetTentativeBestCase()->real_variables();
  GetTentativeBestCase()->GetValues()

  if (settings_->verb_vector()[6] >= 1) { // idx:6 -> opt (Optimization)
    cout << "[opt]GetRealVarVector:--------" << endl;
    IOFormat CleanFmt(1, 0, "", "", "", "", "[", "]");
    cout << setw(10) << rea_base.format(CleanFmt) << endl;

    for (int i = 0; i < idv.length(); ++i) {
      cout << "UUID[" << i << "]: " << idv.at(i).toString().toStdString() << endl;
    }

    cout << fixed << setprecision(8);
  }

  if (settings_->verb_vector()[6] >= 2) // idx:6 -> opt (Optimization)
    cout << "[opt]Perturbations:-----------" << endl;

  for (int dir : dirs) {
    auto trial_point = new Case(GetTentativeBestCase());

    if (int_base.size() > 0) {
      trial_point->SetIntegerVarValues(perturb(int_base, dir));
    }
    else if (rea_base.size() > 0) {
      trial_point->SetRealVarValues(perturb(rea_base, dir));
    }

    trial_point->set_origin_data(GetTentativeBestCase(),
                                 dir, step_lengths_(dir));
    trial_points.append(trial_point);
  }

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
    IOFormat CleanFmt(1, 0, "", "", "", "", "[", "]");
    cout << step_tol_.format(CleanFmt) << endl;
    cout << fixed << setprecision(8);
  }

  for (int i = 0; i < step_lengths_.size(); ++i) {
    if (step_lengths_(i) >= step_tol_(i))
      return false;
  }
  return true;
}

void GSS::set_step_lengths(double len) {
  step_lengths_.fill(len);
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
