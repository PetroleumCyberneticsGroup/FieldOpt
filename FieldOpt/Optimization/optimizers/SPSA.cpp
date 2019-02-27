#include "Optimization/optimizers/SPSA.h"
#include <cmath>

namespace Optimization {
namespace Optimizers {


SPSA::SPSA(Settings::Optimizer *settings,
     Case *base_case,
     Model::Properties::VariablePropertyContainer *variables,
     Reservoir::Grid::Grid *grid,
     Logger *logger,
     CaseHandler *case_handler,
     Constraints::ConstraintHandler *constraint_handler)
    : Optimizer(settings, base_case, variables, grid, logger, case_handler, constraint_handler)
{
  D_ = base_case->GetRealVarVector().size();
  gen_ = get_random_generator(settings->parameters().rng_seed);

  auto params = settings->parameters();
  max_iterations_ = params.spsa_max_iterations;
  init_step_magnitude_ = params.spsa_init_step_magnitude;
  alpha_ = params.spsa_alpha;
  gamma_ = params.spsa_gamma;
  A_ = params.spsa_A;
  a_ = params.spsa_a;
  c_ = params.spsa_c;
  assert(a_ != 0.0 || init_step_magnitude_ != 0.0);

  estimate_ = new Case(base_case);
  perturbations_evaluated_ = false;
  perturbations_valid_ = false;
}

Optimization::Optimizer::TerminationCondition SPSA::IsFinished()
{
  if (case_handler_->CasesBeingEvaluated().size() > 0 || case_handler_->QueuedCases().size() > 0) {
    return NOT_FINISHED;
  }
  if (iteration_ > max_iterations_) {
    Printer::ext_info("Max iterations reached. Terminating", "Optimization", "SPSA");
    return MAX_ITERATIONS_REACHED;
  }
  else {
    return NOT_FINISHED;
  }
}

void SPSA::handleEvaluatedCase(Case *c)
{
  if (isImprovement(c)) {
    updateTentativeBestCase(c);
    if (VERB_OPT >= 2) {
      Printer::ext_info("Found new tentative best case in iteration " + Printer::num2str(iteration_)
          + ": " + Printer::num2str(tentative_best_case_->objective_function_value()),
            "Optimization", "SPSA");
    }
  }
  if (perturbations_.first->state.eval == Optimization::Case::CaseState::E_DONE
      && perturbations_.second->state.eval == Optimization::Case::CaseState::E_DONE)
  {
    perturbations_evaluated_ = true;
    if (VERB_OPT >= 3) {
      Printer::ext_info("Both perturbations evaluated in iteration " + Printer::num2str(iteration_), "Optimization", "SPSA");
    }
    updateGradient();
    if (iteration_ == 1 && init_step_magnitude_ != 0.0) {
      if (VERB_OPT >= 3) { Printer::ext_info("First iteration done. Computing a.", "Optimization", "SPSA"); }
      compute_a();
    }
    update_a_k();
    updateEstimate();
  }

}

void SPSA::iterate()
{
  if (iteration_ > max_iterations_) {
    return;
  }
  logger_->AddEntry(this);
  if (iteration_ == 0 || perturbations_evaluated_ == true) {
    iteration_++;
    if (VERB_OPT >= 3) { Printer::ext_info("Starting iteration " + Printer::num2str(iteration_), "Optimization", "SPSA"); }

    update_c_k();

    int max_attempts = 20;
    int attempt = 1;
    perturbations_valid_ = false;
    while (perturbations_valid_ == false && attempt <= max_attempts) {
      updateSPVector();
      perturbations_ = createPerturbations();
      attempt++;
    }
    if (perturbations_valid_ == false) {
      Printer::ext_warn("Failed to generate a valid pair of perturbations after "
          + Printer::num2str(max_attempts) + "attempts. Skipping iteration.",
          "Optimization", "SPSA");
      iterate();
    }
    else {
      case_handler_->AddNewCase(perturbations_.first);
      case_handler_->AddNewCase(perturbations_.second);
      perturbations_evaluated_ = false;
      return;
    }
  }
  else {
    return;
  }
}

void SPSA::update_a_k()
{
  a_k_ = a_ / pow((A_ + iteration_), alpha_);
  if (VERB_OPT >= 3) {
    Printer::ext_info("Updated gain sequence a_k: " + Printer::num2str(a_k_), "Optimization", "SPSA");
  }
}

void SPSA::update_c_k()
{
  c_k_ = c_ / pow(iteration_, gamma_);
  if (VERB_OPT >= 3) {
    Printer::ext_info("Updated gain sequence c_k: " + Printer::num2str(c_k_), "Optimization", "SPSA");
  }
}

void SPSA::updateSPVector()
{
  if (VERB_OPT >= 3) Printer::ext_info("Updating SP vector.", "Optimization", "SPSA");
  delta_k_ = random_symmetric_bernoulli_eigen(gen_, D_);
}

std::pair<Case *, Case *> SPSA::createPerturbations()
{
  if (VERB_OPT >= 3) Printer::ext_info("Creating perturbations.", "Optimization", "SPSA");
  Case *first = new Case(estimate_);
  Case *second = new Case(estimate_);
  first->SetRealVarValues( estimate_->GetRealVarVector() + c_k_ * delta_k_ );
  second->SetRealVarValues(estimate_->GetRealVarVector() - c_k_ * delta_k_ );
  if (!constraint_handler_->CaseSatisfiesConstraints(first)
      || !constraint_handler_->CaseSatisfiesConstraints(second)) {
    perturbations_valid_ = false;
  }
  else {
    perturbations_valid_ = true;
  }
  return std::pair<Case *, Case *>(first, second);
}

void SPSA::updateGradient()
{
  if (VERB_OPT >= 3) Printer::ext_info("Updating gradient.", "Optimization", "SPSA");
  Eigen::VectorXd inv_delta = Eigen::VectorXd(D_);
  for (int i = 0; i < D_; i++) {
    inv_delta[i] = 1.0 / delta_k_[i];
  }
  double yplus = perturbations_.first->objective_function_value();
  double yminus = perturbations_.second->objective_function_value();
  if (mode_ == Settings::Optimizer::OptimizerMode::Maximize) {
    yplus = yplus * -1;
    yminus = yminus * -1;
  }
  g_k_ = (yplus - yminus) / (2 * c_k_) * (inv_delta);
}

void SPSA::updateEstimate() {
  if (VERB_OPT >= 3) Printer::ext_info("Updating estimate.", "Optimization", "SPSA");

  if ((iteration_ - tentative_best_case_iteration_) > 0.2*max_iterations_) {
    Printer::ext_warn("No new best case found in the last (0.2*max_iterations) iterations. "
        "Setting estimate to tentative best case.", "Optimization", "SPSA");
    estimate_->SetRealVarValues(tentative_best_case_->GetRealVarVector());
    tentative_best_case_iteration_ = iteration_;
  }
  else {
    estimate_->SetRealVarValues(estimate_->GetRealVarVector() - a_k_ * g_k_);
    constraint_handler_->SnapCaseToConstraints(estimate_);
  }

}

void SPSA::compute_a()
{
  a_ = init_step_magnitude_ / abs(g_k_.mean()) * pow(A_ + 1, alpha_);
  if (a_ < 0.0) a_ = 0.0;
  if (a_ > 1.0) a_ = 1.0;
  if (VERB_OPT >= 2) {
    Printer::ext_info("Computed parameter a: " + Printer::num2str(a_), "Optimization", "SPSA");
  }
}

}
}
