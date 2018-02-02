//
// Created by pcg1 on 12.01.18.
//

#include "DFO.h"

namespace Optimization {
namespace Optimizers {

DFO::DFO(Settings::Optimizer *settings,
         Optimization::Case *base_case,
         Model::Properties::VariablePropertyContainer *variables,
         Reservoir::Grid::Grid *grid,
         Logger *logger) : Optimizer(settings, base_case, variables, grid, logger) {
    // Set paramaters and stuff here
    if (settings->parameters().trust_radius > 0.0)
        trust_radius_ = settings->parameters().trust_radius;
    else
        trust_radius_ = 1.0;
}
Optimization::Optimizer::TerminationCondition DFO::IsFinished() {
    return TerminationCondition::NOT_FINISHED;
}
void DFO::handleEvaluatedCase(Optimization::Case *c) {

}
void DFO::iterate() {

}

}
}