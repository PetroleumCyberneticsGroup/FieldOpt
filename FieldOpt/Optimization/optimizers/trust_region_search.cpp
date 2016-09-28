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

        void TrustRegionSearch::iterate()
        {
            /* At the first iteration we initialze the PolyModel
             * object with the initial point
             */
            if (iteration_ == 0) {
                QList<Eigen::VectorXd> points;
                points.append(PointFromCase(tentative_best_case_));
                QList<double> fvalues;
                fvalues.append(tentative_best_case_->objective_function_value());
                polymodel_ = PolyModel(points,fvalues,3.5, points.length());
                polymodel_.complete_points();

                // The set of points has been completed.
                //TODO: Call runner to get objective function values of the set of points.
            }
            // If we found a better point we move the center of the trust region
            else if (betterCaseFoundLastEvaluation()) {
                step();
                perturb();
            }
            else {
                contract();
                perturb();
            }
            case_handler_->ClearRecentlyEvaluatedCases();
        }

        Eigen::VectorXd TrustRegionSearch::PointFromCase(Case *c) {
            Eigen::VectorXd point(c->real_variables().count());
            int i=0;
            for (QUuid id : c->real_variables().keys()){
                point[i] = c->real_variables().value(id);
            }
            return point;
        }

        Case* TrustRegionSearch::CaseFromPoint(Eigen::VectorXd point, Case *prototype){
            Case *new_case = new Case(prototype);
            int i=0;
            for (QUuid id : new_case->real_variables().keys()){
                new_case->set_real_variable_value(id, point[i]);
            }
            return new_case;
        }

        void TrustRegionSearch::UpdateModel

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
