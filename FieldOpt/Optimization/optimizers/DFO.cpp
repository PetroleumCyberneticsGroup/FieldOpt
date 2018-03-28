//
// Created by pcg1 on 12.01.18.
//

#include "DFO.h"
#define MODEL_IMPROVEMENT_ALGORITHM 0
#define FOUND_NEW_OPTIMUM_CANDIDATE 1


/// Test functions - start
double matyasFunction(Eigen::VectorXd x){
  double val = 0.26*(x(0)*x(0) + x(1)*x(1)) - 0.46*x(0)*x(1);
  return val;
}

double sphere(Eigen::VectorXd x){
  double val = 0;
  for (int i = 0; i < x.rows(); i++){
    val += x(i)*x(i);
  }
  return val;
}
/// Test functions - end

namespace Optimization {
namespace Optimizers {

DFO::DFO(Settings::Optimizer *settings,
         Optimization::Case *base_case,
         Model::Properties::VariablePropertyContainer *variables,
         Reservoir::Grid::Grid *grid,
         Logger *logger)
    : Optimizer(settings, base_case, variables, grid, logger) {
    // Set parameters and stuff here
  if (settings->parameters().initial_trust_region_radius > 0.0)
    initial_trust_region_radius_ = settings->parameters().initial_trust_region_radius;
  else
    initial_trust_region_radius_ = 600;

  if (settings->parameters().number_of_interpolation_points > 0)
    number_of_interpolation_points_ = settings->parameters().number_of_interpolation_points;
  else
    number_of_interpolation_points_ = 21;

  if (settings->parameters().number_of_variables > 0)
    number_of_variables_ = settings->parameters().number_of_variables;
  else
    number_of_variables_ = 10;

  if (settings->parameters().required_poisedness > 0)
    required_poisedness_ = settings->parameters().required_poisedness;
  else
    required_poisedness_ = 5;


  settings_ = settings;
  varcont_ = variables;
  iterations_ = 0;
  previous_iterate_type_ = 0;
  base_case_ = new Case(base_case);
  last_action_ = -1;


  // Set up the DFO model.
  Eigen::VectorXd initialStartPoint = base_case->GetRealVarVector();
  DFO_model_ = DFO_Model(number_of_interpolation_points_, number_of_variables_, initialStartPoint, initial_trust_region_radius_, required_poisedness_,settings_);




  Subproblem mySub(settings);
/*
  vector<double> xsol;
  vector<double> fsol;
  //mySub.Solve(xsol, fsol, (char*)"Maximize");
  mySub.Solve(xsol, fsol, (char*)"Minimize");
  int numVars = 10;
  cout << "xsol: " << endl;
  for (int j = 0; j < numVars; j++) {
    cout << xsol[j] << endl;
  }
  cout << endl << "Objective values:" << endl;
  for (int j = 0; j < 2; j++) {
    cout << fsol[j] << endl;
  }
  cout << "stop " << endl;
*/
 /*
  for (int p = 0; p < 10; p++){
  static std::random_device rd;
  static std::mt19937 gen(p);
  static std::uniform_real_distribution<> dis(-30, 30);
  // Testing with 20 (wells) * 6 (variables pr wells) = 120 variables.
  int numVars = 10;
  Eigen::MatrixXd H(numVars,numVars);
  int k = 0;
  for (int j = 0; j < numVars; ++j) {
    for (int i = 0; i < j; ++i){
      H(i,j) = dis(gen);
      if (i != j){
        H(j,i) = H(i,j);
      }
    }
  }


  Eigen::VectorXd g(numVars);
  g.setRandom();
  mySub.setConstant(1.2);
  mySub.setGradient(g);
  mySub.setHessian(H);
  vector<double> xsol;
  vector<double> fsol;
  //mySub.Solve(xsol, fsol, (char*)"Maximize");
  mySub.Solve(xsol, fsol, (char*)"Minimize");

  cout << "xsol: " << endl;
  for (int j = 0; j < numVars; j++) {
    cout << xsol[j] << endl;
  }
  cout << endl << "Objective values:" << endl;
  for (int j = 0; j < 2; j++) {
    cout << fsol[j] << endl;
  }
  }
  cout << "stop " << endl;
*/

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

 */
void DFO::handleEvaluatedCase(Optimization::Case *c) {

}


void DFO::iterate() {

  /// Extract information from cases
  QList<Case *> newCases =  case_handler_->RecentlyEvaluatedCases();
  /*
  Need the fval... the rest is already known.
  */

  // This loop is used for testing. Instead of running simulations, a simple analytical function is used.
  // Most of the content in this loop should be in "iterate()", when the testing is done.
  bool notConverged = true;
  while(notConverged){
    Eigen::MatrixXd new_points;


    if (iterations_ == 0){
      new_points = DFO_model_.findFirstSetOfInterpolationPoints();
    }
    else if(DFO_model_.isInitialInterpolationPointsFound() == false) {
      new_points = DFO_model_.findLastSetOfInterpolationPoints();
    }
    else if(DFO_model_.isModelInitialized()==false){
      DFO_model_.initializeModel();
    }
    else if (last_action_ == MODEL_IMPROVEMENT_ALGORITHM){
      //We are in the middle of trying to increase the poisedness of the interpolation set.
      //Meaning that we must update the model with the new point and keep on checking the poisedness of the interpolation set.
      //DFO_model_.update(d, funcVal, yk, DFO_Model::IMPROVE_POISEDNESS);
    }
    else if (last_action_ == FOUND_NEW_OPTIMUM_CANDIDATE){

    }


    // QList<Case *> new_cases = ConvertPointsToCases(new_points);
    // case_handler_->AddNewCases(new_cases);

    iterations_++;
  }

}
QList<Case *> DFO::ConvertPointsToCases(Eigen::MatrixXd points) {

  QList<Case *> new_cases = QList<Case *>();
  for (int i = 0; i < points.cols(); ++i){
    // First add the center of the model such that we get absolute coordinates and not relative.
    points.col(i) += DFO_model_.getCenterPoint();
    // Create the cases
    auto new_case = new Case(base_case_);
    new_case->SetRealVarValues(points.col(i));
    new_case->set_objective_function_value(std::numeric_limits<double>::max());
    new_cases.append(new_case);

  }
  return new_cases;
}



}
}

