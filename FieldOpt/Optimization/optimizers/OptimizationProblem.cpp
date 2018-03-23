//
// Created by joakim on 14.03.18.
//

#include "OptimizationProblem.h"


namespace Optimization {
namespace Optimizers {


OptimizationProblem::OptimizationProblem(int numberOfVariables,
                                         int numberOfNonlinearConstraints,
                                         int numberOfLinearConstraints,
                                         int numberOfElementsF,
                                         int numberOfElementsG,
                                         int objectiveRow)  {

  n_ = numberOfVariables;
  m_ = numberOfNonlinearConstraints;
  ml_ = numberOfLinearConstraints;
  neF_= numberOfElementsF;
  neG_= numberOfElementsG;
  objRow_= objectiveRow;
  objAdd_=0;
  //= n_ + m_*n_; // Derivatives of objective function and nonlinear constraints.


/*

  if ( ml_ )
  {
    neF_  += ml_;
    lenA_ += ml_ * n_;
    iAfun_ = new integer[lenA_];
    jAvar_ = new integer[lenA_];
    A_     = new double[lenA_];
  }
  iGfun_  = new integer[lenG_];
  jGvar_  = new integer[lenG_];

  xlow_    = new double[n_];
  xupp_    = new double[n_];
  Flow_ = new double[neF_];
  Fupp_ = new double[neF_];
  for (int i = 0; i<n_; i++){
    xlow_[i] = -infinity_;
    xupp_[i] =  infinity_;
  }
  for (int i = 0; i<neF_; i++){
    Flow_[i] = -infinity_;
    Fupp_[i] =  infinity_;
  }
  */

}
void OptimizationProblem::ComputeObjectiveFunction(Eigen::VectorXd d, doublereal *F) {
  F[objRow_] = (constant_ + gradient_.transpose()*d + d.transpose() * hessian_ * d );
}
void OptimizationProblem::ComputeConstraints(Eigen::VectorXd d, doublereal *F) {

}
void OptimizationProblem::ComputeObjectiveGradients(Eigen::VectorXd d, doublereal *G) {
  Eigen::VectorXd grad = gradient_ + hessian_*d;
  for (int i = 0; i < n_;  ++i){
    G[i] = grad[i];
  }
}
void OptimizationProblem::ComputeConstraintsGradients(Eigen::VectorXd d, doublereal *G) {

}
void OptimizationProblem::SetGradient(Eigen::VectorXd gradient) {
  gradient_ = gradient;
}
void OptimizationProblem::SetConstant(double constant) {
  constant_ = constant;
}
void OptimizationProblem::SetHessian(Eigen::MatrixXd hessian) {
  hessian_ = hessian;
}

}
}