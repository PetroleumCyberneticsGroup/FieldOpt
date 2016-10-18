#include "trust_region_search.h"

namespace Optimization {
    namespace Optimizers {

        TrustRegionSearch::TrustRegionSearch(::Settings::Optimizer *settings, Case *base_case,
                                     Model::Properties::VariablePropertyContainer *variables,
                                     Reservoir::Grid::Grid *grid)
                : Optimizer(settings, base_case, variables, grid)
        {
            radius_ = settings->parameters().initial_step_length;
            minimum_radius_ = settings->parameters().minimum_step_length;
        }

        void TrustRegionSearch::step()
        {
            applyNewTentativeBestCase();
        }

        void TrustRegionSearch::contract()
        {
            radius_ = radius_/2.0;
        }

        void TrustRegionSearch::expand()
        {
            radius_ = radius_*2.0;
        }

        void TrustRegionSearch::perturb()
        {
            QList<Case *> perturbations = QList<Case *>();
            for (QUuid id : tentative_best_case_->integer_variables().keys())
                perturbations.append(tentative_best_case_->Perturb(id, Case::SIGN::PLUSMINUS, radius_));
            for (QUuid id : tentative_best_case_->real_variables().keys())
                perturbations.append(tentative_best_case_->Perturb(id, Case::SIGN::PLUSMINUS, radius_));
            for (Case *c : perturbations) {
                constraint_handler_->SnapCaseToConstraints(c);
            }
            case_handler_->AddNewCases(perturbations);
        }

        Optimizer::TerminationCondition TrustRegionSearch::IsFinished()
        {
            if (case_handler_->EvaluatedCases().size() >= max_evaluations_)
                return MAX_EVALS_REACHED;
            else if (radius_ < minimum_radius_)
                return MINIMUM_STEP_LENGTH_REACHED;
            else return NOT_FINISHED; // The value of not finished is 0, which evaluates to false.
        }

        void TrustRegionSearch::UpdateModel()
        {
            /* Everytime we update model we must first have a PolyModel object,
             * then we must complete the set of points in the model, then the
             * objective values of all cases must be calculated, and lastly we
             * can return the PolyModel coefficients. This functions checks these
             * steps, starting with the first one. We start at the first incomplete
             * step and perform all the steps that come after it.
             */

            // At the first iteration we initialze the PolyModel with base_case
            if (iteration_ == 0) {
                polymodel_ = PolyModel(tentative_best_case_, radius_);
                polymodel_.complete_points();
                // The set of points has been completed.
                //TODO: Call runner to get objective function values of the set of points.
                case_handler_->AddNewCases(polymodel_.get_cases_not_eval());
                current_model_ = polymodel_.get_model_coeffs();
            }

            // If set of points is not ready
            else if (polymodel_.ModelNeedsSetOfPoints()) {
                polymodel_.complete_points();
                case_handler_->AddNewCases(polymodel_.get_cases_not_eval());
                polymodel_.ClearCasesNotEval();
                current_model_ = polymodel_.get_model_coeffs();
            }
            // If there are unevaluated cases send them to case_handler
            else if (polymodel_.get_needs_evals()) {
                case_handler_->AddNewCases(polymodel_.get_cases_not_eval());
                polymodel_.ClearCasesNotEval();
                current_model_ = polymodel_.get_model_coeffs();
            }
            // Model is already done, just get coefficients.
            else {
                current_model_ = polymodel_.get_model_coeffs();
            }

            case_handler_->ClearRecentlyEvaluatedCases();

            /* TODO Here we can use current_model to do an optimization step, which
             * should include finding a new (improved) point/case. Next use this
             * point as the new center_point in model and maybe reduce radius, then
             * redo the updateModel thing
             */
        }

        void TrustRegionSearch::iterate() {}

        QString TrustRegionSearch::GetStatusStringHeader() const
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

        QString TrustRegionSearch::GetStatusString() const
        {
            return QString("%1,%2,%3,%4,%5,%6,%7")
                    .arg(iteration_)
                    .arg(nr_evaluated_cases())
                    .arg(nr_queued_cases())
                    .arg(nr_recently_evaluated_cases())
                    .arg(tentative_best_case_->id().toString())
                    .arg(tentative_best_case_->objective_function_value())
                    .arg(radius_);
        }

    }}
