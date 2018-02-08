#include <iostream>
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
  directions_ = GSSPatterns::Compass(num_vars_);
  step_lengths_ = Eigen::VectorXd(directions_.size());
  if (settings->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Init. CompassSearch.-----" << endl;

  auto islv = settings->parameters().initial_step_length_vector;

  if(islv.length() > 0) {
    assert(step_lengths_.rows() == 2 * islv.length());
    for (int i = 0; i < islv.length(); ++i) {
      step_lengths_(i) = settings->parameters().initial_step_length_vector[i];
      step_lengths_(i + islv.length()) = settings->parameters().initial_step_length_vector[i];
    }
  } else {
    step_lengths_.fill(settings->parameters().initial_step_length);
  }

  if (settings->verb_vector()[6] >= 1) { // idx:6 -> opt (Optimization)
    cout << "[opt]Step length vector:------" << endl;
    IOFormat CleanFmt(4, 0, "", "", "", "", "[", "]");
    cout << step_lengths_.format(CleanFmt) << endl;
  }

  assert(step_lengths_.size() == directions_.size());
  step_tol_ = Eigen::VectorXd(directions_.size());
  auto stolv = settings->parameters().minimum_step_length_vector;
  assert(step_tol_.size() == 2 * stolv.size());

  if(stolv.length() > 0) {
    for (int i = 0; i < stolv.length(); ++i) {
      step_tol_(i) = stolv[i];
      step_tol_(i + stolv.length()) = stolv[i];
    }
  } else {
    step_tol_.fill(settings->parameters().minimum_step_length);
  }

  if (settings_->verb_vector()[6] >= 1) { // idx:6 -> opt (Optimization)
    cout << "[opt]Step tol vector:---------" << endl;
    IOFormat CleanFmt(4, 0, "", "", "", "", "[", "]");
    cout << step_tol_.format(CleanFmt) << endl;
  }

}

void CompassSearch::iterate()
{
  if (!is_successful_iteration() && iteration_ != 0)
    contract();
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
