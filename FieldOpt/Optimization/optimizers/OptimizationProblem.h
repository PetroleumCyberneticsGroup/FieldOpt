//
// Created by joakim on 14.03.18.
//

#ifndef FIELDOPT_OPTIMIZATIONPROBLEM_H
#define FIELDOPT_OPTIMIZATIONPROBLEM_H

#include <Eigen/Dense>
#include "FieldOpt-3rdPartySolvers/handlers/SNOPTHandler.h" //Need the definition of integer and doublereal
//#include "FieldOpt-3rdPartySolvers/handlers/SNOPTLoader.h"
//#include "Optimization/optimizer.h"
namespace Optimization {
namespace Optimizers {
class OptimizationProblem {

 private:
  double x0_; //Center point of the model.
  int n_;   // Number of variables
  int m_;    // Number of nonlinear constraints
  int ml_; //number of linera constraints
  integer neF_; // Number of element in F
  integer neG_;
  integer lenG_;
  integer objRow_;
  double objAdd_;

  integer *iAfun_ = NULL;
  integer *jAvar_ = NULL;
  double *A_ = NULL;
  integer lenA_;
  integer neA_;

  integer *iGfun_ = NULL;
  integer *jGvar_ = NULL;

  double *xlow_ = NULL;
  double *xupp_ = NULL;
  double* Flow_;
  double* Fupp_;
  double constant_;
  Eigen::VectorXd gradient_;
  Eigen::MatrixXd hessian_;

  const double infinity_ = 1e20;

 public:
  OptimizationProblem();
  OptimizationProblem(int n, int numberOfNonlinearConstraints, int numberOfLinearConstraints, int numberOfElementsF, int numberOfElementsG, int objectiveRow);

  void ComputeObjectiveFunction(Eigen::VectorXd d, doublereal F[]);
  void ComputeConstraints(Eigen::VectorXd d, doublereal F[]);
  void ComputeObjectiveGradients(Eigen::VectorXd d, doublereal G[]);
  void ComputeConstraintsGradients(Eigen::VectorXd d, doublereal G[]);
  void SetGradient(Eigen::VectorXd gradient);
  void SetConstant(double constant);
  void SetHessian(Eigen::MatrixXd hessian);
};

}
}
#endif //FIELDOPT_OPTIMIZATIONPROBLEM_H
