#include <iostream>
#include <iomanip>
#include "compass_search.h"
#include "gss_patterns.hpp"

using std::cout;
using std::endl;

namespace Optimization {
namespace Optimizers {

CompassSearch::CompassSearch(Settings::Optimizer *settings,
                             Case *base_case,
                             Model::Properties::VariablePropertyContainer *variables,
                             Reservoir::Grid::Grid *grid,
                             Logger *logger
)
    : GSS(settings, base_case, variables, grid, logger)
{

  // Initiate CompassSearch
  directions_ = GSSPatterns::Compass(num_vars_);
  if (settings->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Init. CompassSearch.-----" << endl;

  // Set step lengths
  step_lengths_ = Eigen::VectorXd(directions_.size());
  this->set_step_lengths();
  assert(step_lengths_.size() == directions_.size());
  if (settings->verb_vector()[6] >= 1) { // idx:6 -> opt (Optimization)
    cout << fixed << setw(6) << setprecision(1);
    cout << "[opt]Step length vector:------" << endl;
    IOFormat CleanFmt(1, 0, "", "", "", "", "[", "]");
    cout << step_lengths_.format(CleanFmt) << endl;
  }

  // Set step tolerances
  step_tol_ = Eigen::VectorXd(directions_.size());
  this->set_step_tolerances();
  assert(step_tol_.size() == step_lengths_.size());
  if (settings_->verb_vector()[6] >= 1) { // idx:6 -> opt (Optimization)
    cout << "[opt]Step tol vector:---------" << endl;
    IOFormat CleanFmt(1, 0, "", "", "", "", "[", "]");
    cout << step_tol_.format(CleanFmt) << endl;
    cout << fixed << setprecision(8);
  }

}

void CompassSearch::iterate()
{
  if (!is_successful_iteration() && iteration_ != 0)
    contract();
  if (settings_->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Launching opt.iteration.-" << endl;
  case_handler_->AddNewCases(generate_trial_points());
  case_handler_->ClearRecentlyEvaluatedCases();
  iteration_++;
}

QString CompassSearch::GetStatusStringHeader() const
{
  return QString("%1,%2,%3,%4,%5,%6,%7")
      .arg("Iteration")
      .arg("EvaluatedCases")
      .arg("QueuedCases")
      .arg("RecentlyEvaluatedCases")
      .arg("TentativeBestCaseID")
      .arg("TentativeBestCaseOFValue")
      .arg("StepLength");
}

QString CompassSearch::GetStatusString() const
{
  return QString("%1,%2,%3,%4,%5,%6,%7")
      .arg(iteration_)
      .arg(nr_evaluated_cases())
      .arg(nr_queued_cases())
      .arg(nr_recently_evaluated_cases())
      .arg(GetTentativeBestCase()->id().toString())
      .arg(GetTentativeBestCase()->objective_function_value())
      .arg(step_lengths_[0]);
}

void CompassSearch::handleEvaluatedCase(Case *c) {
  if (isImprovement(c))
    updateTentativeBestCase(c);
}

bool CompassSearch::is_successful_iteration() {
  return case_handler_->RecentlyEvaluatedCases().contains(GetTentativeBestCase());
}

}}
