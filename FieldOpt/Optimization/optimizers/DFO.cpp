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
         Logger *logger)
    : Optimizer(settings, base_case, variables, grid, logger) {
    // Set paramaters and stuff here
  if (settings->parameters().initial_trust_region_radius > 0.0)
    trust_radius_ = settings->parameters().initial_trust_region_radius;
  else
    trust_radius_ = 1.0;

  cout << "[DFO constructor] ..." << endl;
  settings_ = settings;

  cout << settings_->verb_vector()[0] << endl;
  realvar_uuid_ = GetTentativeBestCase()->GetRealVarIdVector();


  Eigen::VectorXd vars = base_case->GetRealVarVector();

  cout<< "realvar \n" << vars << endl;


  vars[0] += 100;
  vars[1] += 100;
  vars[2] += 100;
  vars[3] += 100;
  vars[4] += 100;
  vars[5] += 100;
  Optimization::Case *newCase = new Case(base_case);
  newCase->SetRealVarValues(vars);
  newCase->set_objective_function_value(std::numeric_limits<double>::max());
  case_handler_->AddNewCase(newCase);
}
/*
Optimization::Optimizer::TerminationCondition DFO::IsFinished() {
    return TerminationCondition::NOT_FINISHED;
}
void DFO::handleEvaluatedCase(Optimization::Case *c) {

}
void DFO::iterate() {

}
*/
}
}