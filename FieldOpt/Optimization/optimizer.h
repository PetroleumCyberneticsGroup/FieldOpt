/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "Settings/optimizer.h"
#include "case.h"
#include "case_handler.h"
#include "constraints/constraint_handler.h"
#include "optimization_exceptions.h"
#include "Model/properties/variable_property_container.h"
#include "Runner/loggable.hpp"
#include "Runner/logger.h"
#include "normalizer.h"
#include "Utilities/verbosity.h"
#include "Utilities/printer.hpp"

class Logger;

namespace Optimization {

class HybridOptimizer;

/*!
 * \brief The Optimizer class is the abstract parent class for all optimizers. It is primarily
 * designed to support direct search optimization algorithms.
 */
class Optimizer : public Loggable
{
  friend class HybridOptimizer;

 public:
  Optimizer() = delete;

  /*!
   * \brief GetCaseForEvaluation Get a new, unevaluated case for evaluation.
   *
   * If no unevaluated cases are currently available in the CaseHandler, the iterate()
   * method is called to generate new cases.
   * \return Pointer to a new, unevaluated case.
   */
  Case *GetCaseForEvaluation();

  /*!
   * \brief SubmitEvaluatedCase Submit an already evaluated case to the optimizer.
   *
   * The submitted case is marked as recently evaluated in the CaseHandler.
   * \param c Case to submit.
   */
  void SubmitEvaluatedCase(Case *c);

  /*!
   * \brief GetTentativeBestCase Get the best case found so far.
   * \return
   */
  Case *GetTentativeBestCase() const;

  /*!
   * \brief case_handler Get the case handler. Used by the bookkeeper in the runner lib.
   */
  CaseHandler *case_handler() const { return case_handler_; }

  // Status related methods
  int nr_evaluated_cases() const { return case_handler_->EvaluatedCases().size(); }
  int nr_queued_cases() const { return case_handler_->QueuedCases().size(); }
  int nr_recently_evaluated_cases() const { return case_handler_->RecentlyEvaluatedCases().size(); }

  /*!
   * The TerminationCondition enum enumerates the reasons why an optimization run is deemed
   * finished. It is returned by the IsFinished method
   */
  enum TerminationCondition : int {NOT_FINISHED=0,
    MAX_EVALS_REACHED=1, MINIMUM_STEP_LENGTH_REACHED=2,
    MAX_ITERATIONS_REACHED=3
  };

  /*!
   * \brief IsFinished Check whether the optimization is finished, i.e. if the the optimizer has
   * reached some  termination condition.
   *
   * This method should be called before attempting to get a new case for evaluation.
   * \return NOT_FINISHED (0, =false) if the optimization has not finished, otherwise the non-zero reason
   * for termination.
   */
  virtual TerminationCondition IsFinished() = 0;

  virtual QString GetStatusStringHeader() const; //!< Get the CSV header for the status string.
  virtual QString GetStatusString() const; //!< Get a CSV string describing the current state of the optimizer.
  void EnableConstraintLogging(QString output_directory_path); //!< Enable writing a text log for the constraint operations.
  void SetVerbosityLevel(int level);
  bool IsAsync() const { return is_async_; } //!< Check if the optimizer is asynchronous.

  /*!
   * @brief Get the simulation duration in seconds for a case.
   * @param c Case to get simulation duration for.
   * @return Simulation duration in seconds. -1 if the case has not been successfully simulated.
   */
  int GetSimulationDuration(Case *c);

  int iteration() const { return iteration_; }

 protected:
  /*!
   * \brief Base constructor for optimizers. Initializes constraints and sets some member values.
   * \param settings Settings for the optimizer.
   * \param base_case The base case for optimizer. Must already have been evaluated (i.e. have an objective function value).
   * \param variables The variable property container from the Model (needed for initialization of constriants).
   * \param grid The grid to be used (needed for initializtion of some constraints).
   * \param logger Logger object passed from runner.
   * \param case_handler CaseHandler object. This is passed from the HybridOptimizer; defaults to 0, in which case a new one will be created.
   * \param constraint_handler ConstraintHandler object. This is passed from the HybridOptimizer; defaults to 0, in which case a new one will be created.
   */
  Optimizer(::Settings::Optimizer *settings,
            Case *base_case,
            Model::Properties::VariablePropertyContainer *variables,
            Reservoir::Grid::Grid *grid,
            Logger *logger,
            CaseHandler *case_handler=0,
            Constraints::ConstraintHandler *constraint_handler=0
  );

  /*!
   * @brief Handle an incomming evaluated case. This is called at the end of the SubmitEvaluatedCase method.
   * @param c
   */
  virtual void handleEvaluatedCase(Case *c) = 0;

  /*!
   * @brief Check whether the Case c is an improvement on the tentative best case.
   * @param c Case to be checked.
   * @return True if improvement; otherwise false.
   */
  bool isImprovement(const Case* c);

  /*!
   * @brief Check if Case c1 is better than Case c2, taking into account if we're maximizing or minimizing.
   */
  bool isBetter(const Case* c1, const Case *c2) const;

  /*!
   * \brief iterate Performs an iteration, generating new cases and adding them to the case_handler.
   */
  virtual void iterate() = 0;

  LogTarget GetLogTarget() override;
  map<string, string> GetState() override;
  QUuid GetId() override;
  map<string, vector<double>> GetValues() override;

 protected:
  void updateTentativeBestCase(Case *c);
  CaseHandler *case_handler_; //!< All cases (base case, unevaluated cases and evaluated cases) passed to or generated by the optimizer.
  Constraints::ConstraintHandler *constraint_handler_; //!< All constraints defined for the optimization.
  int evaluated_cases_; //!< Number of evaluated cases.
  int max_evaluations_; //!< Maximum number of objective function evaluations allowed before terminating.
  int iteration_; //!< The current iteration.
  int verbosity_level_; //!< The verbosity level for runtime console logging.
  ::Settings::Optimizer::OptimizerMode mode_; //!< The optimization mode, i.e. whether the objective function should be maximized or minimized.
  bool is_async_; //!< Inidcates whether or not the optimizer is asynchronous. Defaults to false.
  Logger *logger_;
  bool enable_logging_; //!< Whether logging should be performed. This should be set to false when the optimizer is a component in HybridOptimizer.
  void DisableLogging(); //!< Disable logging for this optimizer. This is called by HybridOptimizer.
  bool penalize_; //!< Switch for whether or not to use penalty function to account for constraints.
  Case *tentative_best_case_; //!< The best case encountered thus far.
  int tentative_best_case_iteration_; //!< The iteration in which the current tentative best case was found.
  bool is_hybrid_component_; //!< Indicates that this object is a hybrid optimization component.

  Normalizer normalizer_ofv_; //!< Normalizer for objective function values.

  void initializeNormalizers(); //!< Initialize all normalization parameters.

  class Summary : public Loggable {
   public:
    Summary(Optimizer *opt, TerminationCondition cond,
            map<string, string> ext_state=map<string, string>()) { opt_ = opt; cond_ = cond; ext_state_ = ext_state; }
    LogTarget GetLogTarget() override;
    map<string, string> GetState() override;
    QUuid GetId() override;
    map<string, vector<double>> GetValues() override;
   private:
    map<string, string> ext_state_;
    Optimizer *opt_;
    Optimizer::TerminationCondition cond_;
  };

  /*!
   * @brief Calculate the penalized objective function value for a case.
   * @param c Case to calculate the penalized objective function value for.
   * @return The penalized objective function value.
   */
  double PenalizedOFV(Case *c);

 private:
  QDateTime start_time_;
  int seconds_spent_in_iterate_; //!< The number of seconds spent in the iterate() method.

  /*!
   * @brief Initialize the OFV normalizer, setting the parameters for it
   * from the cases that have been evaluated so far.
   */
  void initializeOfvNormalizer();
};

}

#endif // OPTIMIZER_H
