#include <iostream>
#include "compass_search.h"
#include "gss_patterns.hpp"

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
    Matrix<double, Dynamic, Dynamic> step_lengths_test_;


    if (settings->parameters().initial_step_length_vector.count() > 0) {
    
        // Convert to eigen vector (1):
//        for( int i=0; i<settings->parameters().initial_step_length_vector.count(); ++i ) {
//            step_lengths_(i) = settings->parameters().initial_step_length_vector[i];
//        }

        // Convert to eigen vector (2):
        std::vector<double> v = settings->parameters().
            initial_step_length_vector.toVector().toStdVector();
        Map<Matrix<double, Dynamic, Dynamic>> step_lengths_test_(v.data(),v.size(),1);
    
    } else {

    }

    step_lengths_.fill(settings->parameters().initial_step_length);

    // Assert conversion to eigen vector is ok 
    for( int i=0; i<step_lengths_test_.rows(); ++i ) {
        assert(step_lengths_test_(0) == step_lengths_[0]);
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
    return QString("%1,%2,%3,%4,%5,%6,%7,%8,%9")
        .arg("Iteration")
        .arg("EvaluatedCases")
        .arg("QueuedCases")
        .arg("RecentlyEvaluatedCases")
        .arg("TentativeBestCaseID")
        .arg("TentativeBestCaseOFValue")
        .arg("StepLengthX")
        .arg("StepLengthY")
        .arg("StepLengthZ");
}

QString CompassSearch::GetStatusString() const
{
    return QString("%1,%2,%3,%4,%5,%6,%7,%8,%9")
        .arg(iteration_)
        .arg(nr_evaluated_cases())
        .arg(nr_queued_cases())
        .arg(nr_recently_evaluated_cases())
        .arg(GetTentativeBestCase()->id().toString())
        .arg(GetTentativeBestCase()->objective_function_value())
        .arg(step_lengths_[0])
        .arg(step_lengths_[1])
        .arg(step_lengths_[2]);
}

void CompassSearch::handleEvaluatedCase(Case *c) {
    if (isImprovement(c))
        updateTentativeBestCase(c);
}

bool CompassSearch::is_successful_iteration() {
    return case_handler_->RecentlyEvaluatedCases().contains(GetTentativeBestCase());
}

}}
