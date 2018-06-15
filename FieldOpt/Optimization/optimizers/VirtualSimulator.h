//
// Created by joakim on 12.06.18.
//

#ifndef FIELDOPT_VIRTUALSIMULATOR_H
#define FIELDOPT_VIRTUALSIMULATOR_H
#include <iostream>
#include <iomanip>
#include <casadi/casadi.hpp>
#include <Eigen/Core>

class VirtualSimulator {
 private:
  //nlbuilder.
  std::string problem; // =  "/home/joakim/git/casadi/docs/examples/nl_files/nlTestSet/qptest1.nl";
  casadi::NlpBuilder nl;
  //nl.import_nl(problem);
  std::vector<casadi::DM> input;
  casadi::Function f;
  casadi::Function fj;
  casadi::Function g;
  casadi::Function gj;
  int m_; //Number of constraints (linear and nonlinear);
  int n_; //Number of variables;
  int mb_; //Number of bounds.


 public:
  VirtualSimulator(std::string problemFile);
  VirtualSimulator();
  //get gradient?
  //get functionvalue
  //get constraintvalue -> for the subproblem (snopt)

  Eigen::VectorXd evaluateConstraints(Eigen::VectorXd point);
  Eigen::MatrixXd evaluateConstraintGradients(Eigen::VectorXd point);
  double evaluateFunction(Eigen::VectorXd point);
  Eigen::VectorXd evaluateFunctionGradients(Eigen::VectorXd point);
  casadi::DM convertEigenMatrixToCasadi(Eigen::MatrixXd point);
  Eigen::MatrixXd convertCasadiMatrixToEigen(casadi::DM casadiMatrix);
  int GetNumberOfVariables();
  int GetNumberOfConstraints();
  Eigen::VectorXd GetInitialPoint();
  Eigen::VectorXd GetLowerBoundsForVariables();
  Eigen::VectorXd GetUpperBoundsForVariables();
  Eigen::VectorXd GetLowerBoundsForConstraints();
  Eigen::VectorXd GetUpperBoundsForConstraints();
  bool IsFeasiblePoint(Eigen::VectorXd point);

  Eigen::VectorXd Solve();

  //get functinogradients ->
  //get constraintgradients -> for the subproblem (snopt)
  //void run(
};

#endif //FIELDOPT_VIRTUALSIMULATOR_H
