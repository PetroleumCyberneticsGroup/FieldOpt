//
// Created by joakim on 24.04.18.
//

#ifndef FIELDOPT_GRADIENTENHANCEDMODEL_H
#define FIELDOPT_GRADIENTENHANCEDMODEL_H

#include <Eigen/Dense>
#include "FieldOpt-3rdPartySolvers/handlers/SNOPTHandler.h"
#include "FieldOpt-3rdPartySolvers/handlers/SNOPTLoader.h"

namespace Optimization {
namespace Optimizers {

class GradientEnhancedModel {
 private:
  Eigen::VectorXd h_old_;
  double constant_;
  Eigen::VectorXd gradient_;
  Eigen::MatrixXd hessian_;
  Eigen::MatrixXd hessian_old_;
  Eigen::MatrixXd points_;
  Eigen::MatrixXd v_;
  Eigen::VectorXd y0_;

  double alpha_;
  Eigen::VectorXd weights_derivatives_;

  int n_;
  int m_;
  int ng_; // number_of_variables_with_gradient


  SNOPTHandler initSNOPTHandler();
  void setOptionsForSNOPT(SNOPTHandler &snoptHandler);
  void solveLinearSystem(Eigen::MatrixXd D, Eigen::VectorXd v, Eigen::VectorXd funcVals, Eigen::VectorXd derivatives_at_y0, Eigen::VectorXd weights_least_square, Eigen::VectorXd &ans);
  int convert_h_ij_to_t_lsq(int i, int j);
  int convert_h_ij_to_t_vectorized(int i, int j);
  void convert_t_to_ij_lsq(int t, int &i, int &j);
  void convert_t_to_ij_vectorized(int t, int &i, int &j);


 public:
  GradientEnhancedModel(int n, int m, int number_of_variables_with_gradient, Eigen::VectorXd weights_derivatives, double weight_objective_minimum_change);
  void GetConstant(double &c);
  void GetGradient(Eigen::VectorXd &g);
  void GetHessian(Eigen::MatrixXd &H);
  void GetModel(double &c, Eigen::VectorXd &g, Eigen::MatrixXd &H);
  void ComputeModel(Eigen::MatrixXd Y, Eigen::MatrixXd derivatives,
                    Eigen::VectorXd derivatives_at_y0, Eigen::VectorXd funcVals ,Eigen::VectorXd y0, Eigen::VectorXd best_point, double radius,
                    double scaling_factor_r, int index_of_center_point);



};
}
}
#endif //FIELDOPT_GRADIENTENHANCEDMODEL_H
