//
// Created by pcg1 on 12.01.18.
//

#include "DFO.h"
#include "Subproblem.h"
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

  settings_ = settings;
  varcont_ = variables;
  iterations_ = 0;


  Eigen::MatrixXd hess(2,2);
  hess.setOnes();
  Eigen::VectorXd grad(2);
  grad << 1,2;
  Subproblem mySub(settings);
  mySub.setConstant(1.2);
  mySub.setGradient(grad);
  mySub.setHessian(hess);
  vector<double> xsol;
  vector<double> fsol;
  mySub.Solve(xsol, fsol, (char*)"Maximize");

  cout << "xsol: " << endl;
  for (int j = 0; j < 2; j++) {
    cout << xsol[j] << endl;
  }
  cout << endl << "Objective values:" << endl;
  for (int j = 0; j < 2; j++) {
    cout << fsol[j] << endl;
  }

  cout << "stop " << endl;


  /*
  cout << "[DFO constructor] ..." << endl;


  //cout << settings_->verb_vector()[0] << endl;
  realvar_uuid_ = GetTentativeBestCase()->GetRealVarIdVector();


  Eigen::VectorXd vars = base_case->GetRealVarVector();

  cout<< "realvar \n" << vars << endl;


  vars[0] += 1;
  vars[1] += 1;
  vars[2] += 1;
  vars[3] += 1;
  vars[4] += 1;
  vars[5] += 1;
  QString old = base_case->GetId().toString();
  Optimization::Case *newCase = new Case(base_case);
  QString neww = newCase->GetId().toString();

  cout << "The ID of the base case is: " << old.toStdString() << endl;
  cout << "The ID of the copied case is: " << neww.toStdString() << endl;

  if (old == neww){
    cout << "WTF \n";
  }
  if (old == old)
    cout << "GOOD " << endl;
  if (neww == neww)
    cout << "GOOD 2" << endl;
  if (old.toStdString() == old.toStdString())
    cout << "GOOD 3" << endl;

  if (base_case->GetId() == base_case->GetId())
    cout << "GOOD 4\n";

  if (base_case->GetId() == newCase->GetId())
    cout << "THIS IS BAD\n";

  if (base_case != newCase)
    cout << "INteresting. comparing cases!" << endl;

  auto fval0 = base_case->objective_function_value();

  newCase->SetRealVarValues(vars);
  newCase->set_objective_function_value(std::numeric_limits<double>::max());
  //auto fval1 = newCase->objective_function_value();

  cout << fval0 << "\t \t" << "oooouups" << endl;

  case_handler_->AddNewCase(newCase);
*/

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