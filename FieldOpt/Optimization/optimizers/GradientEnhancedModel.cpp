//
// Created by joakim on 24.04.18.
//

#include <iostream>
#include "GradientEnhancedModel.h"
#include "EigenUtil.h"

namespace Optimization {
namespace Optimizers {

static Eigen::VectorXd _y0_;
static Eigen::MatrixXd _hessian_old_;
static Eigen::MatrixXd _D_;
static Eigen::MatrixXd _v_;
static double _alpha_;
static Eigen::VectorXd _weights_least_square_;
static Eigen::MatrixXd _points_;
static Eigen::VectorXd _best_point_;
static int _n_;
static int _ng_;
static int _m_;


#ifdef __cplusplus
extern "C" {
#endif
int SNOPTUserFunction(integer *Status, integer *n, doublereal x[],
                      integer *needF, integer *neF, doublereal F[],
                      integer *needG, integer *neG, doublereal G[],
                      char *cu, integer *lencu,
                      integer iu[], integer *leniu,
                      doublereal ru[], integer *lenru);
#ifdef __cplusplus
}
#endif

int SNOPTusrFG3_(integer *Status, integer *n, double x[],
                 integer *needF, integer *neF, double F[],
                 integer *needG, integer *neG, double G[],
                 char *cu, integer *lencu,
                 integer iu[], integer *leniu,
                 double ru[], integer *lenru) {

  // Convert std::vector into Eigen3 vector
  Eigen::VectorXd xvec(*n);
  for (int i = 0; i < *n; ++i) {
    xvec[i] = x[i];
  }

  int m = *neF - 1;

  // If the values for the objective and/or the constraints are desired
  if (*needF > 0) {
    /// The objective function
    ///   Loop it.
    F[0] = 1;
    if (m) {
      /// The constraints
      ///     One for each interpolation constraint.
      ///     A bunch for the symmetric requirement
      if (1) {
        F[1] = 2;
      }
    }
  }

  if (*needG > 0) {
    /// The derivatives of the objective function

    /// The derivatives of the constraints
    if (m) {

    }
  } // end needG
  return 0;
}




void GradientEnhancedModel::GetConstant(double &c) {
  c = constant_;
}
void GradientEnhancedModel::GetGradient(Eigen::VectorXd &g) {
  g = gradient_;
}
void GradientEnhancedModel::GetHessian(Eigen::MatrixXd &H) {
  H = hessian_;
}
void GradientEnhancedModel::GetModel(double &c, Eigen::VectorXd &g, Eigen::MatrixXd &H) {
  c = constant_;
  g = gradient_;
  H = hessian_;
}
GradientEnhancedModel::GradientEnhancedModel(int n,
                                             int m,
                                             int number_of_variables_with_gradient,
                                             QList<double> weights_derivatives,
                                             double weight_objective_minimum_change) {

  _n_ = n;
  n_ = n;
  _m_ = m;
  m_ = m;
  _ng_ = number_of_variables_with_gradient;
  ng_ = number_of_variables_with_gradient;
  //_alpha_ = weight_objective_minimum_change;
  alpha_ = weight_objective_minimum_change;
  weights_derivatives_ = Eigen::VectorXd::Zero(weights_derivatives.size());
  int j = 0;
  for (auto i = weights_derivatives.begin(); i != weights_derivatives.end(); ++i){
    weights_derivatives_[j] = *i;
    j++;
  }
  constant_ = 0;
  gradient_ = Eigen::VectorXd::Zero(n_);
  hessian_ = Eigen::MatrixXd::Zero(n_, n_);
  //_hessian_old_ = Eigen::MatrixXd::Zero(n_, n_);
  hessian_old_ = Eigen::MatrixXd::Zero(n_, n_);
  //_points_ = Eigen::MatrixXd::Zero(n_, m_);
  points_ = Eigen::MatrixXd::Zero(n_, m_);
  funcVals_ = Eigen::VectorXd::Zero(n_);
  //_v_ = Eigen::VectorXd::Zero(ng_*m_);
  v_ = Eigen::VectorXd::Zero(ng_*m_);
  //_y0_ = Eigen::VectorXd::Zero(n_);
  y0_ = Eigen::VectorXd::Zero(n_);
  //_best_point_ = Eigen::VectorXd::Zero(n_);
  best_point_ = Eigen::VectorXd::Zero(n_);
  //_weights_least_square_ = Eigen::VectorXd::Zero(m_);
  weights_least_square_ = Eigen::VectorXd::Zero(m_);

  int t = 0;
  int y = n_;
  for (int i = 0; i < ng_; ++i) {
    t += y;
    y--;
  }
  h_old_ = Eigen::VectorXd::Zero(t);
  //_D_ = Eigen::MatrixXd::Zero((ng_)*m_, t);
  D_ = Eigen::MatrixXd::Zero((ng_)*m_, t);

}

void GradientEnhancedModel::ComputeModel(Eigen::MatrixXd Y,
                                         Eigen::MatrixXd derivatives,
                                         Eigen::VectorXd derivatives_at_y0,
                                         Eigen::VectorXd funcVals,
                                         Eigen::VectorXd y0,
                                         Eigen::VectorXd best_point,
                                         double radius, double scaling_factor_r,
                                         int index_of_center_point) {
  //funcVals[0] = 0.0818000000000001;
  //funcVals[1] = 0.2797999999999998;
  //funcVals[2] = 0.5977999999999994;
  // init the snopt handler
  // set up all constraints.
  //
  funcVals_ = funcVals;
  points_ = Y;
  //derivatives_at_y0.setZero();
  //points_ <<
  //        1.1, 2.1, 3.1,
  //    1.2, 2.2, 3.2;


  //_y0_ = y0;
  y0_ = y0;
  best_point_ = best_point;

  // Set up _weights_least_square_
  for (int t = 0; t < m_; ++t){
    double norm = (Y.col(t) - best_point).norm();
    if (norm <= radius ){
      weights_least_square_[t] = weights_derivatives_[0];
    }
    else if (norm <= scaling_factor_r * radius ){
      weights_least_square_[t] = weights_derivatives_[1];
    }
    else{
      weights_least_square_[t] = weights_derivatives_[2];
    }
  }

  // Set the _D_ matrix and the _v_ vector
  if (ng_ > 0){
  int base_row = 0;
  for (int t = 0; t < m_; ++t) { // for each sample point

    int y = n_;
    int c0 = 0;
    for (int i = 0; i < ng_; ++i) { // for each row
      for (int k = 0; k < y; ++k) { // for each elem in row
        D_(base_row + i, c0 + k) = points_(k, t)*weights_least_square_[t];
      }
      c0 += y;
      y--;
    }
    y = n_ - 1;
    c0 = 0;
    for (int k = 0; k < (ng_ - 1); ++k) { // for each row that begins an inverse diagonal
      int ii = 1;
      int jj = 1;
      for (int i = k; i < (ng_ - 1); ++i) {//for each element in that inverse diagonal
        //_D_(base_row + k + ii, c0 + y - jj) = points_(y, t)*_weights_least_square_[t];
        D_(base_row + k + ii, c0 + y - jj) = points_(y, t)*weights_least_square_[t];
        ii++;
        jj++;
      }
      c0 += (y + 1);
      y--;
    }
    base_row += ng_;


    for (int i = 0; i < ng_; ++i){
      //_v_(t*ng_ +i) = (derivatives((ng_ - i - 1), t) - derivatives_at_y0(ng_ - i - 1))*_weights_least_square_[t];
         v_(t*ng_ +i) = (derivatives((ng_ - i - 1), t) - derivatives_at_y0(ng_ - i - 1))*weights_least_square_[t];
    }
  }
  }

  //std::cout << "v\n" << _v_ << std::endl;

  // set the constraints
  Eigen::VectorXd ans;
  //hessian_old_.setZero();
  //solveLinearSystem(_D_, _v_, funcVals, derivatives_at_y0, _weights_least_square_, ans);
  solveLinearSystem(funcVals, derivatives_at_y0, ans);

  // calculate start indices;
  int start_h_ij = 0;
  int start_g_i = (int) ((n_*n_ + n_)*0.5);
  int start_c = start_g_i + (n_-ng_);
  int start_lambda_i = start_c + 1;

  //extract the results
  constant_ = ans(start_c);
  for (int i = 0; i < n_-ng_; ++i){
    gradient_[i] = ans(start_g_i+i);
  }
  eigen_tail(gradient_, derivatives_at_y0,ng_);
  for (int i = 1; i <= (int) ((n_*n_ + n_)*0.5); ++i){
    int ii = 0;
    int jj = 0;
    convert_t_to_ij_vectorized(i, ii,jj);

    hessian_(ii-1,jj-1) = ans(start_h_ij + i-1);
    if (ii != jj){
      hessian_(jj-1,ii-1) = hessian_(ii-1,jj-1);
    }
  }

  hessian_old_ = hessian_;
  }

void GradientEnhancedModel::solveLinearSystem(Eigen::VectorXd funcVals,
                                              Eigen::VectorXd derivatives_at_y0,
                                              Eigen::VectorXd &ans) {

  /// Test for the convert functions. The answer is on the right. Case: ng_=4 and n_=5;
  //int t1 = convert_h_ij_to_t_vectorized(1,1); // 1
  //int t2 = convert_h_ij_to_t_vectorized(5,3); // 12
  //int t3 = convert_h_ij_to_t_vectorized(5,5); // 15
  //int t1 = convert_h_ij_to_t_lsq(2,1); // 13
  //int t2 = convert_h_ij_to_t_lsq(5,3); // 3
  //int t3 = convert_h_ij_to_t_lsq(5,5); // 5
  //std::cout << "t1 = " << t1 << "\t t2 = " << t2 << "\t t3 = " << t3 << "\n\n";

  //int ii1, jj1, ii2, jj2, ii3, jj3;
  //convert_t_to_ij_vectorized(1,ii1,jj1);  // (1,1)
  //convert_t_to_ij_vectorized(12,ii2,jj2); // (5,3)
  //convert_t_to_ij_vectorized(15,ii3,jj3); // (5,5)
  //std::cout << ii1 << ", " << jj1 << "\t"<< ii2 << ", " << jj2 << "\t"<< ii3 << ", " << jj3 << "\n";
  //convert_t_to_ij_lsq(13,ii1,jj1); // (2,1)
  //convert_t_to_ij_lsq(3,ii2,jj2);  // (5,3)
  //convert_t_to_ij_lsq(5,ii3,jj3);  // (5,5)
  //std::cout << ii1 << ", " << jj1 << "\t"<< ii2 << ", " << jj2 << "\t"<< ii3 << ", " << jj3 << "\n";


  // dL/dhij, (n² + n)/2
  // ans = [h11, h21, h31, h41, ... hn1, h22, h32, h42, h52, ... hn1,  ......, g1,g2,g3,...,gn, c, lambda1,lambda2,...,lambdam]
  //std::cout << "D\n" << D << std::endl;
  //std::cout << "funcVals\n" << funcVals << std::endl;
  int colsD = 0;
  int y = n_;
  for (int i = 0; i < ng_; ++i) {
    colsD += y;
    y--;
  }
  Eigen::MatrixXd A = Eigen::MatrixXd::Zero((int) (1 + (n_-ng_) + m_ + (n_*n_ + n_)*0.5) ,(int) (1 + (n_-ng_) + m_ + (n_*n_ + n_)*0.5));
  Eigen::VectorXd b = Eigen::VectorXd::Zero((int) (1 + (n_-ng_) + m_ + (n_*n_ + n_)*0.5));
  // calculate start indices;
  int start_h_ij = 0;
  int start_g_i = (int) ((n_*n_ + n_)*0.5);
  int start_c = start_g_i + (n_-ng_);
  int start_lambda_i = start_c + 1;
  int row = 0;




  // dL/dhij,
  for (int i = 1; i <= n_; ++i){
    for (int j = 1; j <= i; ++j){ // the derivative with respect to (i,j)

      b(row) += hessian_old_(i-1,j-1)*alpha_; //right hand side
      A(row, convert_h_ij_to_t_vectorized(i,j)-1) = alpha_;

      for (int t = 1; t <= m_; ++t){
        A(row, start_lambda_i+ t-1) += -0.5*points_(i-1,t-1)*points_(j-1,t-1);
      }

      if (i > (n_-ng_)){
        for (int p = 1; p <= ng_*m_; p++) {
          int t = convert_h_ij_to_t_lsq(i,j); // taking derivative w.r.t. h_t

          b(row) += (1 - alpha_) * v_(p - 1) * D_(p-1, t-1); // right hand side

          //b(convert_h_ij_to_t_vectorized(i, j) - 1) += (1 - alpha) * v(p - 1) * D(convert_h_ij_to_t_lsq(i, j) - 1, p - 1);
          for (int k = 1; k <= colsD; ++k) {
            int ii = 0;
            int jj = 0;

            convert_t_to_ij_lsq(k, ii, jj);
            A(row, convert_h_ij_to_t_vectorized(ii, jj) - 1) += (1 - alpha_) * D_(p-1,k-1) * D_(p-1, t-1);
          }
        }

      }

      row++;
    }
  }
  //std::cout << "A, dL/dhij\n" << A << std::endl;
  //A.setZero();
  // dL/dgi,
  for (int i = 1; i <= (n_-ng_); ++i){
    for (int t = 1; t <= m_; ++t){
      A(row, start_lambda_i + t -1) += points_(i-1,t-1);
    }
    row++;
  }

  //std::cout << "A, dL/dgi\n" << A << std::endl;
  //A.setZero();
  // dL/dc
  for (int t = 1; t <= m_; ++t){
    A(row, start_lambda_i + t -1) = 1;
  }
  row++;

  //std::cout << "A, dL/dc \n" << A << std::endl;
  //A.setZero();
  //interpolation conditions
  for (int k = 1; k <= m_; ++k){
    b(row) = funcVals(k-1) - derivatives_at_y0.dot((points_.col(k-1)).tail(ng_));
    A(row, start_c) = 1;

    for (int t = 1; t <= (n_-ng_); ++t){
      A(row, start_g_i + t -1) = points_(t-1,k-1);
    }

    for (int i = 1; i <= n_; ++i) {
      for (int j = 1; j <= i; ++j) {
        if (i == j){
          A(row,start_h_ij+convert_h_ij_to_t_vectorized(i,j)-1) = 0.5*points_(i-1,k-1)*points_(i-1,k-1);
        }
        else{
          A(row,start_h_ij+convert_h_ij_to_t_vectorized(i,j)-1) =     points_(i-1,k-1)*points_(j-1,k-1);
        }
      }
    }

    row++;
  }
  //std::cout << "A, interpolation cond\n" << A << std::endl;

  //std::cout << "A\n" << A << std::endl;
  //std::cout << "b\n" << b << std::endl;
  //Eigen::MatrixXd Asub = Eigen::MatrixXd::Zero();

  // Ax = b; -> solve
  ans = A.colPivHouseholderQr().solve(b);
  //std::cout << "ans\n" << ans << std::endl;

}

void GradientEnhancedModel::ComputeModel2(Eigen::MatrixXd Y,
                                         Eigen::MatrixXd derivatives,
                                         Eigen::VectorXd derivatives_at_y0,
                                         Eigen::VectorXd funcVals,
                                         Eigen::VectorXd y0,
                                         Eigen::VectorXd best_point,
                                         double radius, double scaling_factor_r,
                                         int index_of_center_point) {

  m_ = 3;
  constant_ = 0;
  gradient_.setZero();
  hessian_.setZero();
  hessian_old_.setZero();
  points_.resize(n_, m_);
  points_.setZero();
  v_.resize(ng_*m_,1);
  v_.setZero();
  y0_ = Eigen::VectorXd::Zero(n_);
  best_point_ = Eigen::VectorXd::Zero(n_);
  weights_least_square_ = Eigen::VectorXd::Zero(m_);

  int t = 0;
  int y = n_;
  for (int i = 0; i < ng_; ++i) {
    t += y;
    y--;
  }
  h_old_ = Eigen::VectorXd::Zero(t);
  //_D_ = Eigen::MatrixXd::Zero((ng_)*m_, t);
  D_.resize((ng_)*m_, t);
  D_.setZero();
  h_old_.setZero();








  //funcVals[0] = 0.0818000000000001;
  //funcVals[1] = 0.2797999999999998;
  //funcVals[2] = 0.5977999999999994;
  // init the snopt handler
  // set up all constraints.
  //

  points_ = Y.block(0,0,2,m_);
  Eigen::MatrixXd derivatives2 = derivatives.block(0,0,1,m_);
  //derivatives_at_y0.setZero();
  //points_ <<
  //        1.1, 2.1, 3.1,
  //    1.2, 2.2, 3.2;


  //_y0_ = y0;
  y0_ = y0;
  best_point_ = best_point;

  // Set up _weights_least_square_
  for (int t = 0; t < m_; ++t){
    double norm = (Y.col(t) - best_point).norm();
    if (norm <= radius ){
      weights_least_square_[t] = weights_derivatives_[0];
    }
    else if (norm <= scaling_factor_r * radius ){
      weights_least_square_[t] = weights_derivatives_[1];
    }
    else{
      weights_least_square_[t] = weights_derivatives_[2];
    }
  }

  // Set the _D_ matrix and the _v_ vector
  int base_row = 0;
  for (int t = 0; t < m_; ++t) { // for each sample point

    int y = n_;
    int c0 = 0;
    for (int i = 0; i < ng_; ++i) { // for each row
      for (int k = 0; k < y; ++k) { // for each elem in row
        //_D_(base_row + i, c0 + k) = points_(k, t)*_weights_least_square_[t];
        D_(base_row + i, c0 + k) = points_(k, t)*weights_least_square_[t];
      }
      c0 += y;
      y--;
    }
    y = n_ - 1;
    c0 = 0;
    for (int k = 0; k < (ng_ - 1); ++k) { // for each row that begins an inverse diagonal
      int ii = 1;
      int jj = 1;
      for (int i = k; i < (ng_ - 1); ++i) {//for each element in that inverse diagonal
        //_D_(base_row + k + ii, c0 + y - jj) = points_(y, t)*_weights_least_square_[t];
        D_(base_row + k + ii, c0 + y - jj) = points_(y, t)*weights_least_square_[t];
        ii++;
        jj++;
      }
      c0 += (y + 1);
      y--;
    }
    base_row += ng_;


    for (int i = 0; i < ng_; ++i){
      //_v_(t*ng_ +i) = (derivatives((ng_ - i - 1), t) - derivatives_at_y0(ng_ - i - 1))*_weights_least_square_[t];
      v_(t*ng_ +i) = (derivatives2((ng_ - i - 1), t) - derivatives_at_y0(ng_ - i - 1))*weights_least_square_[t];
    }
  }

  //std::cout << "v\n" << _v_ << std::endl;




  // set the constraints
  Eigen::VectorXd ans;
  //solveLinearSystem(_D_, _v_, funcVals, derivatives_at_y0, _weights_least_square_, ans);
  solveLinearSystem(funcVals, derivatives_at_y0, ans);

  // calculate start indices;
  int start_h_ij = 0;
  int start_g_i = (int) ((n_*n_ + n_)*0.5);
  int start_c = start_g_i + (n_-ng_);
  int start_lambda_i = start_c + 1;

  //extract the results
  constant_ = ans(start_c);
  for (int i = 0; i < n_-ng_; ++i){
    gradient_[i] = ans(start_g_i+i);
  }
  eigen_tail(gradient_, derivatives_at_y0,ng_);
  for (int i = 1; i <= (int) ((n_*n_ + n_)*0.5); ++i){
    int ii = 0;
    int jj = 0;
    convert_t_to_ij_vectorized(i, ii,jj);

    hessian_(ii-1,jj-1) = ans(start_h_ij + i-1);
    if (ii != jj){
      hessian_(jj-1,ii-1) = hessian_(ii-1,jj-1);
    }
  }

  hessian_old_ = hessian_;
}

int GradientEnhancedModel::convert_h_ij_to_t_lsq(int i, int j) {
  int t = 0;
    for (int p = n_; p>i; --p){
      t += p;
    }

    t+=j;
  return t;
}

int GradientEnhancedModel::convert_h_ij_to_t_vectorized(int i, int j) {
  int t = 0;
  int y = n_;
  for (int k = 1; k < j; ++k){
    t += y;
    y--;
  }
  t += (i-j+1);
  return t;
}

void GradientEnhancedModel::convert_t_to_ij_lsq(int t, int &i, int &j) {
  i = n_;
  j = 0;
  for (i = n_; t-i > 0 ;--i){
    t -= i;
  }
  j = t;
}
void GradientEnhancedModel::convert_t_to_ij_vectorized(int t, int &i, int &j) {
  j = 1;
  for (int k = n_; t > k; k--){
    t = t - k;
    j ++;
  }
  i = j + (t-1);
}
/// SNOPT FUNCTIONALITY
SNOPTHandler GradientEnhancedModel::initSNOPTHandler() {
  std::string prnt_file, smry_file, optn_file;
  optn_file = "optionfile.opt.optn";
  smry_file = "summaryfile.opt.summ";
  prnt_file = "thirdpartyprintfile.opt.prnt";
  SNOPTHandler snoptHandler(prnt_file.c_str(),
                            smry_file.c_str(),
                            optn_file.c_str());
  /*
  prnt_file = "snopt_print.opt.prnt";
  smry_file = "snopt_summary.opt.summ";
  optn_file = "snopt_options.opt.optn";

  SNOPTHandler snoptHandler(prnt_file.c_str(),
                            smry_file.c_str(),
                            optn_file.c_str());*/
  //cout << "[opt]Init. SNOPTHandler.------" << endl;
  return snoptHandler;
}
void GradientEnhancedModel::setOptionsForSNOPT(SNOPTHandler &snoptHandler) {

  //if (settings_->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
  //cout << "[opt]Set options for SNOPT.---" << endl;

  //snoptHandler.setParameter("Backup basis file              0");
//  snoptHandler.setRealParameter("Central difference interval", 2 * derivativeRelativePerturbation);

  //snoptHandler.setIntParameter("Check frequency",           60);
  //snoptHandler.setParameter("Cold Start                     Cold");

  //snoptHandler.setParameter("Crash option                   3");
  //snoptHandler.setParameter("Crash tolerance                0.1");
  //snoptHandler.setParameter("Derivative level               3");

//  if ( (optdata.optimizationType == HISTORY_MATCHING) || hasNonderivativeLinesearch )
//    snoptHandler.setParameter((char*)"Nonderivative linesearch");
//  else
  //snoptHandler.setParameter((char*)"Derivative linesearch");
  //snoptHandler.setIntParameter("Derivative option", 0);

//  snoptHandler.setRealParameter("Difference interval", optdata.derivativeRelativePerturbation);

  //snoptHandler.setParameter("Dump file                      0");
  //snoptHandler.setParameter("Elastic weight                 1.0e+4");
  //snoptHandler.setParameter("Expand frequency               10000");
  //snoptHandler.setParameter("Factorization frequency        50");
  //snoptHandler.setRealParameter("Function precision", sim_data.tuningParam.tstep.minDeltat);
  //snoptHandler.setParameter("Hessian full memory");
  //snoptHandler.setParameter("Hessian limited memory");

//  snoptHandler.setIntParameter("Hessian frequency", optdata.frequencyToResetHessian);
  //snoptHandler.setIntParameter("Hessian updates", 0);
  //snoptHandler.setIntParameter("Hessian flush", 1);  // Does NOT work in the current version of SNOPT!!!

  //snoptHandler.setParameter("Insert file                    0");
//  snoptHandler.setRealParameter("Infinite bound", optdata.defaultControlUpperBound);

  //snoptHandler.setParameter("Iterations limit");
  //snoptHandler.setRealParameter("Linesearch tolerance",0.9);
  //snoptHandler.setParameter("Load file                      0");
  //snoptHandler.setParameter("Log frequency                  100");
  //snoptHandler.setParameter("LU factor tolerance            3.99");
  //snoptHandler.setParameter("LU update tolerance            3.99");
  //snoptHandler.setRealParameter("LU factor tolerance", 3.99);
  //snoptHandler.setRealParameter("LU update tolerance", 3.99);
  //snoptHandler.setParameter("LU partial pivoting");
  //snoptHandler.setParameter("LU density tolerance           0.6");
  //snoptHandler.setParameter("LU singularity tolerance       3.2e-11");

  //target nonlinear constraint violation
  //snoptHandler.setRealParameter("Major feasibility tolerance", 0.000001);
  snoptHandler.setIntParameter("Major Iterations Limit", 1000);

  //target complementarity gap
  //snoptHandler.setRealParameter("Major optimality tolerance", 0.0001);

  snoptHandler.setParameter("Major Print level  00000"); //  000001"
  snoptHandler.setRealParameter("Major step limit", 0.2);
  //snoptHandler.setIntParameter("Minor iterations limit", 200); // 200

  //for satisfying the QP bounds
//  snoptHandler.setRealParameter("Minor feasibility tolerance", optdata.constraintTolerance);
  snoptHandler.setIntParameter("Minor print level", 0);
  //snoptHandler.setParameter("New basis file                 0");
  //snoptHandler.setParameter("New superbasics limit          99");
  //snoptHandler.setParameter("Objective Row");
  //snoptHandler.setParameter("Old basis file                 0");
  //snoptHandler.setParameter("Partial price                  1");
  //snoptHandler.setParameter("Pivot tolerance                3.7e-11");
  //snoptHandler.setParameter("Print frequency                100");
  //snoptHandler.setParameter("Proximal point method          1");
  //snoptHandler.setParameter("QPSolver Cholesky");
  //snoptHandler.setParameter("Reduced Hessian dimension");
  //snoptHandler.setParameter("Save frequency                 100");
  snoptHandler.setIntParameter("Scale option", 1);
  //snoptHandler.setParameter("Scale tolerance                0.9");
  snoptHandler.setParameter((char *) "Scale Print");
  snoptHandler.setParameter((char *) "Solution  Yes");
  //snoptHandler.setParameter("Start Objective Check at Column 1");
  //snoptHandler.setParameter("Start Constraint Check at Column 1");
  //snoptHandler.setParameter("Stop Objective Check at Column");
  //snoptHandler.setParameter("Stop Constraint Check at Column");
  //snoptHandler.setParameter("Sticky parameters               No");
  //snoptHandler.setParameter("Summary frequency               100");
  //snoptHandler.setParameter("Superbasics limit");
  //snoptHandler.setParameter("Suppress parameters");
  //snoptHandler.setParameter((char*)"System information  No");
  //snoptHandler.setParameter("Timing level                    3");
  //snoptHandler.setRealParameter("Unbounded objective value   1.0e+15");
  //snoptHandler.setParameter("Unbounded step size             1.0e+18");
  //snoptHandler.setIntParameter("Verify level", -1); //-1
  //snoptHandler.setRealParameter("Violation limit", 1e-8); //1e-8

//  if (settings_->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
//    cout << "[opt]Set options for SNOPT.---" << endl;

}

void GradientEnhancedModel::PrintParametersMatlabFriendly() {

  std::cout << "_______________________________" << std::endl;
  std::cout << "_____Matlab friendly print_____" << std::endl;
  std::cout << "_________Enhanced model________" << std::endl;

  std::cout << "H = [ " << std::endl;
  for (int i = 0; i < n_; ++i) {
    std::cout << hessian_old_.row(i);
    if (i != n_ - 1) {
      std::cout << "; \n";
    }
  }
  std::cout << " \n ];" << std::endl;

  std::cout << "g = [" << std::endl;
  std::cout << gradient_ << std::endl;
  std::cout << "]';" << std::endl;
  std::cout << "c = " << constant_ << ";" << std::endl;
  std::cout << "y0 = [" << std::endl;
  std::cout << y0_ << std::endl;
  std::cout << "]';" << std::endl;
  //std::cout << "rho = " < << ";" << std::endl;
}

void GradientEnhancedModel::isInterpolating(){
  std::cout << "Checking if the gradient enhanced model actually interpolates the points\n";
  for (int i = 1; i<=m_; ++i){
    double val = constant_ + gradient_.transpose() *points_.col(i-1)+ 0.5*(points_.col(i-1)).transpose()*hessian_old_*points_.col(i-1);
    std::cout << funcVals_[i-1] << " == " << val <<"\t"<< (std::abs(val-funcVals_[i-1]) <= 0.000000001) << "\n";
  }
}

}
}