//
// Created by joakim on 08.03.18.
//

#ifndef FIELDOPT_SUBPROBLEM_H
#define FIELDOPT_SUBPROBLEM_H

#include <Eigen/Core>
#include <Eigen/Dense>
#include "FieldOpt-3rdPartySolvers/handlers/SNOPTHandler.h"
#include "FieldOpt-3rdPartySolvers/handlers/SNOPTLoader.h"
#include "Optimization/optimizer.h"
namespace Optimization {
namespace Optimizers {
class Subproblem {

/*
 This class will find _one_ maximum of a quadratic function (specified by c_, g_ and H_) subject to some specified constraints.
 The constraints must be specified by whomever uses this function and they must be specified by editing the code explicitly
 (i.e, they cannot be set through function calls).
 The objective function and the constraints (except the simple/basic bounds) are put together into one set of equations:
 Let cl_i represent a linear constraint, and let cn_i represent a nonlinear one.
 let n_l and m be the numbers of linear constraints and nonlinear constraints, respectively.
 F = [f_obj, cl_0, cl_1, ..., cl_n_l, cn_0, cn_1, ..., cn_m]^T;

 The inequalities for the constraints and the objective function is given by:
 Flow <= F <= Fupp

 If you don't have a limit, use the infinity_ *(+-1) value.

 maximize    f_obj = c_ + g_^T x + x^T H_ x

 subject to     Flow <= F <= Fupp
 				xlow <= x <= xupp


 Linear and nonlinear constraints are specified differently.

 Note!
 Because F contains both the objective functions and the constraints, the row-indices will start at row 1 and not row 0.

 Linear
 	The linear constraints are specified through lenA, iAfun, jAvar and A.
 	The first 2 are used because the matrix hatA ( "lower <= hatA*x <= upper") might be very sparse. An example will illustrate the usage:

 	A = [0 1 0
 		 5 0 6
 		 0 0 8].

 	lenA = 4; // There are four nonzero elements in A
 	iAfun[0] = 1; //These two indices belongs to the element (0,1) in A (namely, the value 1). Now A[0] must be set to 1.
 	jAvar[0] = 1;
 	A[0] = 1;

	iAfun[1] = 2; //These two indices belongs to the element (1,0) in A (namely, the value 5). Now A[1] must be set to 5.
 	jAvar[1] = 0;
 	A[1] = 5;

 	iAfun[2] = 2; //These two indices belongs to the element (1,2) in A (namely, the value 6). Now A[2] must be set to 6.
 	jAvar[2] = 2;
 	A[2] = 6;

	iAfun[3] = 3; //These two indices belongs to the element (2,2) in A (namely, the value 8). Now A[3] must be set to 8.
 	jAvar[3] = 2;
 	A[3] = 8;


 Nonlinear
 	The nonlinear constraints are specified through lenG, neG, iGfun and jGvar. The actual G matrix is specified in the userfunc.
 	The G matrix contains both the derivative of the objective function and the derivative of the nonlinear constraints.
 	The nonlinear constraints must also be specified by the userfunc, and put into appropriate place in F:
 	F[4]=x_1^2 + x_2^2 + x_3^2;
 	Let's say that we now, in addition to the linear constraints above, also has 1 nonlinear constraint. ("lower <= x_1^2 + x_2^2 + x_3^2  <= upper").
 	The partial derivatives with respect to the variables will then be:
 	G[y] = 2*x_1;
 	G[y+1] = 2*x_2;
 	G[y+2] = 2*x_3;
 	Where y is the number of partial derivatives of the objective functions.
 	If f_obj = x_1 + x_2 + x_3, then
 	y = 3; and
 	F[0] = x_1 + x_2 + x_3;
 	G[0] = 1;
 	G[1] = 1;
 	G[2] = 1;

 	Now we must specify iGfun and jGvar.

 	//From the objective function:
 	iGfun[0] = 0;
 	jGvar[0] = 0;
 	iGfun[1] = 0;
 	jGvar[1] = 1;
 	iGfun[2] = 0;
 	jGvar[2] = 2;


	//From the nonlinear constraints:
 	iGfun[3] = 4; // NOTE NOTE! The reason why this value is 4 is because the first row is for the objective function, then  we have 3 linear constraints
 	jGvar[3] = 0;

	iGfun[4] = 4;
 	jGvar[4] = 1;

	iGfun[5] = 4;
 	jGvar[5] = 2;


	neG = lenG = 6;




*/


 private:

  int normType_;
  Eigen::VectorXd y0_;
  Eigen::VectorXd bestPointDisplacement_;

  int n_; // Number of variables
  int m_;    // Number of nonlinear constraints
  integer neF_; // Number of element in F
  integer neG_;
  integer lenG_;
  integer objRow_;
  double objAdd_;
  double trustRegionRadius_;

  integer *iAfun_ = NULL;
  integer *jAvar_ = NULL;
  double *A_ = NULL;
  integer lenA_;
  integer neA_;

  integer *iGfun_ = NULL;
  integer *jGvar_ = NULL;

  double *x_;

  // controls lower and upper bounds
  double *xlow_ = NULL;
  double *xupp_ = NULL;

  Eigen::VectorXd xlowCopy_;
  Eigen::VectorXd xuppCopy_;

  // the initial guess for Lagrange multipliers
  double *xmul_ = NULL;;

  // the state of the variables (whether the optimal is likely to be on
  // the boundary or not)
  integer *xstate_ = NULL;

  double *F_ = NULL;
  double *Flow_ = NULL;
  double *Fupp_ = NULL;
  double *Fmul_ = NULL;
  integer *Fstate_ = NULL;
  char *xnames_ = NULL;
  char *Fnames_ = NULL;

  integer nxnames_;
  integer nFnames_;
  Settings::Optimizer *settings_;

  // this is the value SNOPT considers as infinity
  double infinity_ = 1e20;

  void setConstraintsAndDimensions();

  void setOptionsForSNOPT(SNOPTHandler &snoptHandler);

  bool loadSNOPT(string libname = "libsnopt-7.2.12.2.so");

  void setAndInitializeSNOPTParameters();

  void passParametersToSNOPTHandler(SNOPTHandler &snoptHandler);

  void setNormType(int type);
  void setCenterPointOfModel(Eigen::VectorXd cp);
  void setCurrentBestPointDisplacement(Eigen::VectorXd db);

 public:


 //EIGEN_MAKE_ALIGNED_OPERATOR_NEW


  enum NormType {
    INFINITY_NORM = 0,
    L2_NORM = 2,
  };

  void setQuadraticModel(double c, Eigen::VectorXd g, Eigen::MatrixXd H);

  void setGradient(Eigen::VectorXd g);

  void setHessian(Eigen::MatrixXd H);

  void setConstant(double constant);

  void printModel();


  void SetNormType(int type){
    normType_ = type;
  }

  void SetCenterPoint(Eigen::VectorXd cp);

  void SetBestPointRelativeToCenterPoint(Eigen::VectorXd bp);


  ~Subproblem();

  SNOPTHandler initSNOPTHandler();

  Subproblem(Settings::Optimizer *settings);
  Subproblem(){};

  void ResetSubproblem();

  //void Solve(std::vector<double> xsol, std::vector<double> fsol, char *optimizationType);

  //void Solve(vector<double> *xsol, vector<double> *fsol, char *optimizationType);

  void Solve(vector<double> &xsol, vector<double> &fsol, char *optimizationType,   Eigen::VectorXd y0, Eigen::VectorXd bestPointDisplacement);

  void SetTrustRegionRadius(double radius){
    trustRegionRadius_ = radius;
    Flow_[1] = 0;
    Fupp_[1] = trustRegionRadius_;
  }
};

}
}

#endif //FIELDOPT_SUBPROBLEM_H
