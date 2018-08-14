/******************************************************************************
   Created by einar on 6/7/17.
   Copyright (C) 2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#ifndef FIELDOPT_EGO_H
#define FIELDOPT_EGO_H

#include "Optimization/optimizer.h"
#include "gp/gp.h"
#include "AcquisitionFunction.h"
#include "af_optimizers/AFPSO.h"

namespace Optimization {
namespace Optimizers {
namespace BayesianOptimization {

/*!
 * @brief This class is an implementation of Efficient Global Optimization (EGO),
 * i.e. Bayesian Optimization using Gaussian Process models applied to derivative-
 * free optimization.
 *
 * \todo Hyperparameter optimization: after N cases, optimize the GP hyperparameters.
 * \todo Convergence criterion: total squared error in model.
 * \todo Convergence criterion: Combination of highest expected value ans total squared uncertainty?
 */
class EGO : public Optimizer {
 public:
  TerminationCondition IsFinished() override;
  EGO(Settings::Optimizer *settings,
      Case *base_case,
      Model::Properties::VariablePropertyContainer *variables,
      Reservoir::Grid::Grid *grid,
      Logger *logger,
      CaseHandler *case_handler=0,
      Constraints::ConstraintHandler *constraint_handler=0
  );

 protected:
  void handleEvaluatedCase(Case *c) override;
  void iterate() override;

 private:
  VectorXd lb_, ub_; //!< Upper and lower bounds
  int n_initial_guesses_; //!< Number of random cases to be generated initially.
  libgp::GaussianProcess *gp_; //!< The gaussian process to be used throughout the optimization run.
  BayesianOptimization::AcquisitionFunction af_; //!< Acquisition function to be used throughout the optimization run.
  BayesianOptimization::AFOptimizers::AFPSO af_opt_; //!< Aquisition function optimizer to be used throughout the optimization run.

  long int time_af_opt_;
  long int time_fitting_;

  class ConfigurationSummary : public Loggable {
   public:
    ConfigurationSummary(EGO *opt) { opt_ = opt; }
    LogTarget GetLogTarget() override;
    map<string, string> GetState() override;
    QUuid GetId() override;
    map<string, vector<double>> GetValues() override;
   private:
    EGO *opt_;
  };
};

}
}
}

#endif //FIELDOPT_EGO_H
