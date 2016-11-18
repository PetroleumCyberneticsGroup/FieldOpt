#include <iostream>
#include "compass_search.h"

namespace Optimization {
    namespace Optimizers {

        CompassSearch::CompassSearch(Settings::Optimizer *settings, Case *base_case,
                                     Model::Properties::VariablePropertyContainer *variables,
                                     Reservoir::Grid::Grid *grid)
                : GSS(settings, base_case, variables, grid)
        {
            step_length_ = settings->parameters().initial_step_length;
            minimum_step_length_ = settings->parameters().minimum_step_length;

            contr_fac_ = settings->parameters().minimum_step_length;
            expan_fac_ = 1.0;

            int numRvars = base_case->GetRealVarVector().size();
            int numIvars = base_case->GetIntegerVarVector().size();
            int num_vars = numRvars + numIvars;
            if (numRvars > 0 && numIvars > 0)
                std::cout << "WARNING: Compass search may behave strangely when using both continuous and discrete variables." << std::endl;

            // Generate set of direction vectors (all coordinate directions)
            directions_ = std::vector<Eigen::VectorXi>(2*num_vars);
            for (int i = 0; i < num_vars; ++i) {
                Eigen::VectorXi dir = Eigen::VectorXi::Zero(num_vars);
                dir(i) = 1;
                directions_[i] = dir;
                directions_[i+num_vars] = (-1) * dir;
            }

            // Generate list of step lengths
            step_lengths_ = Eigen::VectorXd(num_vars);
            step_lengths_.fill(settings->parameters().initial_step_length);
        }

        void CompassSearch::step()
        {
            applyNewTentativeBestCase();
        }

        void CompassSearch::contract()
        {
            step_length_ = step_length_/2.0;
        }

        void CompassSearch::perturb()
        {
            QList<Case *> perturbations = QList<Case *>();
            for (QUuid id : tentative_best_case_->integer_variables().keys())
                perturbations.append(tentative_best_case_->Perturb(id, Case::SIGN::PLUSMINUS, step_length_));
            for (QUuid id : tentative_best_case_->real_variables().keys())
                perturbations.append(tentative_best_case_->Perturb(id, Case::SIGN::PLUSMINUS, step_length_));
            for (Case *c : perturbations) {
                constraint_handler_->SnapCaseToConstraints(c);
            }
            case_handler_->AddNewCases(perturbations);
        }

        Optimizer::TerminationCondition CompassSearch::IsFinished()
        {
            if (case_handler_->EvaluatedCases().size() >= max_evaluations_)
                return MAX_EVALS_REACHED;
            else if (step_length_ < minimum_step_length_)
                return MINIMUM_STEP_LENGTH_REACHED;
            else return NOT_FINISHED; // The value of not finished is 0, which evaluates to false.
        }

        void CompassSearch::iterate()
        {
            if (iteration_ == 0) {
                perturb();
            }
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
                    .arg(tentative_best_case_->id().toString())
                    .arg(tentative_best_case_->objective_function_value())
                    .arg(step_length_);
        }

        void CompassSearch::handleEvaluatedCase(Case *c) {

        }

    }}
