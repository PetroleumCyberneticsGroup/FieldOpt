
#ifndef FIELDOPT_DFO_MODEL_H
#define FIELDOPT_DFO_MODEL_H

#include <iostream>
#include <Eigen/Dense>
#include <random>
#include <math.h>
#include <Settings/optimizer.h>
#include <Subproblem.h>
#include "Subproblem.h"
/* References
This implementation is based upon the following papers and book, and I would recommend anyone who
is trying to understand the code to actively use those references.
[1] The BOBYQA algorithm for bound constrained optimization without derivatives by M.J.D. Powell.
[2] The NEWUOA software for unconstrained optimization without derivatives by M.J.D. Powell.
[3] Introduction to Derivative-Free Optimization by Andrew R. Conn, Katya Scheinberg and Luis N. Vicente.
[4] Least Frobenius norm updating of quadratic models that satisfy interpolation conditions by M.J.D. Powell.

Only the parts about model improvement (i.e., finding upper/lower/high value of the Lagrange polynomials,
are based upon [3]. Almost everything else is based upon [2] (practical approach) and [4] (more theoretical view).
*/
namespace Optimization {
namespace Optimizers {

class DFO_Model {

 private:


  bool cmp(const Eigen::VectorXd& a, const Eigen::VectorXd& b)
  {
    // you should probably also add some code to prevent dividing by zero...
    return ( a.norm() > b.norm());
  }



  Settings::Optimizer *settings_;
  Subproblem subproblem;
  unsigned int m; // Number of interpolation points used to create the model. Does not change.
  unsigned int n; // Number of decision variables in your model.
  double rho; // Trust-region radius.
  double lambda; // The required poisedness of the set of interpolation points.
  double r = 2;

  Eigen::VectorXd y0; // The point which the model is ceneterd around.

  Eigen::MatrixXd
      Y;  // Container for the interpolation points. The interpolation point i is given in the following way:
  // yi = y0 + Y.col(i); i.e. Y contains the displacements away from y0.

  Eigen::VectorXd fvals; // Holds the function evaluations for the interpolation points.

  Eigen::VectorXd bestPoint; // Displacement of the optimal point from y0. Absolute point: y0 + bestPoint;
  int bestPointIndex; // This assumes that the point is in the interpolation set (not sure if this will be discarded in future work)

  Eigen::VectorXd bestPointAllTime;
  double bestPointAllTimeFunctionValue;


  //These 4 variables are only used in the case m>2*n+1. How to calculate the p's and q's are well explained in [1], for sigma see [2].
  //The i's are used to find the corresponding interpolation points and function values in Y and Fvals, respectively.
  //The qs, qs and is come in a set of 3, such that the same index of each vector belong together.
  Eigen::VectorXi sigmas;
  Eigen::VectorXi ps;
  Eigen::VectorXi qs;
  Eigen::VectorXi is;

  // The inverse KKT matrix, H. See [2]
  Eigen::MatrixXd Xi;
  Eigen::MatrixXd Upsilon;
  Eigen::MatrixXd Z;
  Eigen::DiagonalMatrix<double, Eigen::Dynamic>
      S; //Should have been a integer matrix, but Eigen doesn't support diagonal int matrices.


  // Containers for the 2nd order model.
  Eigen::VectorXd gradient;
  Eigen::VectorXd centerPoint;
  Eigen::MatrixXd hessian;
  double constant = 0;

  Eigen::MatrixXd Gamma;
  Eigen::VectorXd gammas;

  bool modelInitialized;
  bool initialInterpolationPointsFound;

  /**
  Checks if a value is almost zero.

  @value the value to be checked.
  @zeroLimit the highest value that still is counted as zero.
  @return true / false
  */
  bool isApproxZero(double value, double zeroLimit);

  /**
  The classic kronecker-delta function.

  Compares if two integers are equal or not.

  @param[in] i integer 1
  @param[in] j integer 2
  @return 1 if i==j and zero otherwise.
  */
  int kroneckerDelta(int i, int j);

  /**
  Returns the sign of the value.

  0 is counted as positive (+1).

  @param[in] the value to be checked.
  @return the sign of the value.
  */
  int sign(double value);

  /**
  Initializes the quadratic model.

  Initializes Gamma, gamma, hessian, constant,
  This function must be ran before initializeInverseKKTMatrix()
  */
  void initializeQuadraticModel();

  /**
  Initializes the H matrix.

  Initializes Xi, Upsilon, Z, S.
  */
  void initializeInverseKKTMatrix();

  /**
  Updates the H matrix with one new point.

  @param[in] yNew is the  is the displacement of the new point from the current center point(y0).
  @param[in] fvalNew is the function evaluation corresponding to yNew.
  @param[in] t-1 is the index of the point that is going to be replaced by yNew in Y.
  */
  void updateInverseKKTMatrix(Eigen::VectorXd yNew, double fvalNew, unsigned int t);

  /**
  Updates the model with a new point.

  @param[in] yNew is the  is the displacement of the new point from the current center point(y0).
  @param[in] fvalNew is the function evaluation corresponding to yNew.
  @param[in] t-1 is the index of the point that is going to be replaced by yNew in Y.
  */
  void updateQuadraticModel(Eigen::VectorXd yNew, double fvalNew, unsigned int t);

  /**
  Finds a lower bound, within a ball of radius and centered at y0,
  for a quadratic function.

  The value type is decided by where the b was found:
  0 - in the constant term.
  1 - in a linear term.
  2 - in a quadratic term.
  3 - in a cross term.
  This function only gives the correct output if the ball of interest is
  centered at y0.

  @param[in] radius is the radius of the ball
  @param[in] b is the highest absolute value coefficient in the polynomial.
  @param[in] type is used to tell in which type of term was b found.
  @return the lower bound.
  */
  double evaluateLowerBoundQuadraticPolynomial(double radius, double b, int type);

 public:

  enum UpdateReason {
    IMPROVE_POISEDNESS = -1,
    INCLUDE_NEW_OPTIMUM = -2,
    INCLUDE_NEW_POINT = -3
  };

  bool isInitialInterpolationPointsFound() {
    return initialInterpolationPointsFound;
  }

  bool isModelInitialized() {
    return modelInitialized;
  }

  /**
  Constructor for the class.

  A naming convenvention:
  model = this entire class.
  quadratic model = just the quadratic model.
  Inverse KKT matrix = H

  @param[in] m number of interpolation points.
  @param[in] n number of decision variables.
  @param[in] y0 the center point of the model.
  @param[in] rhoBeg the initial trust-region radius.
  @param[in] lambda the required poisedness of the interpolation set (lambda > 1)
  */
  DFO_Model(unsigned int m,
            unsigned int n,
            Eigen::VectorXd y0,
            double rhoBeg,
            double lambda,
            Settings::Optimizer *settings);

  DFO_Model() {};

  /**
  Finds the first set of interpolation points.

  Finds the first set of interpolation points based
  upon the initial center point (y0) and the trust-region radius.

  */
  Eigen::MatrixXd findFirstSetOfInterpolationPoints();

  /**
  Finds the last set of interpolation points.

  If m > 2n+1, then this function must also be ran. Before it is ran,
  make sure that the local variable fvals is filled up with the function
  evaluations for the first set of interpolation points.

  */
  Eigen::MatrixXd findLastSetOfInterpolationPoints();

  /**
  Initializes the model.

  Both findFirstSetOfInterpolationPoints() and findLastSetOfInterpolationPoints() (if m > 2n+1)
  must be called before this function.
  */
  void initializeModel();

  /**
  Updates the model with a new point.

  Updates the model with a new point, the reason for updating must be provided.
  Some changes must be done, and this function is NOT done!
  Cannot be used when INCLUDE_NEW_OPTIMUM is the reason.

  @param[in] yNew is the  is the displacement of the new point from the current center point(y0).
  @param[in] fvalNew is the function evaluation corresponding to yNew.
  @param[in] t-1 is the index of the point that is going to be replaced by yNew in Y.
  @param[in] updateReason is either IMPROVE_POISEDNESS or INCLUDE_NEW_OPTIMUM.
  */
  void update(Eigen::VectorXd yNew, double fvalNew, unsigned int t, UpdateReason updateReason);

  /**
  Evaluates the current quadratic model at point.

  @param[in] point is the displacement from current center point (y0).
  @return the value of the model at point.
  */
  double evaluateQuadraticModel(Eigen::VectorXd point);

  /**
  This is a bad way of accessing and updating the function evaluations.

  It will be replaced by other functions later, when I have decided how
  the interface should be.
  @return a reference to the fvals
  */
  Eigen::VectorXd *getFvalsReference() {
    return &(this->fvals);
  }

  /**
  This is a bad way of accessing Y outside of the class.

  It will be replaced by other functions later, when I have decided how
  the interface should be.
  @return a reference to the Y
  */
  Eigen::MatrixXd *getYReference() {
    return &(this->Y);
  }

  /**
  Returns the center point of the quadratic model

  @return the center point of the quadratic model
  */
  Eigen::MatrixXd getCenterPoint() {
    return y0;
  }

  /**
  Shifts the center point of the quadratic model by the vector s.

  @param[in] s is the displacement from the current center point (y0)
  */
  void shiftCenterPointOfQuadraticModel(Eigen::VectorXd s);

  /**
  Finds an upper and a lower bound on the absolute value of the i-th
  Lagrange polynomial.

  This is a crude estimate.

  @param[in] i the index of the Lagrange polynomial that is going to found bounds for.
  @param[out] lowerBound gets the value of the lower bound. Disregards initial value.
  @param[out] upperBound gets the value of the upper bound. Disregards initial value.
  */
  void findLowerAndUpperBoundOnAbsoluteLagrangePolynomial(int i, double &lowerBound, double &upperBound);

  /**
  Finds a point to replace to increase the poisedness of the interpolation set.
  It also finds out which new point should be added.

  @param[out] the vector that should be included. Disregards initial value.
  @param[out] index of the interpolation point that should be replaced. Disregards initial value.
  */
  void findPointToImprovePoisedness(Eigen::VectorXd &dNew, int &yk);

  void findWorstPointInInterpolationSet(Eigen::VectorXd &dNew, int &yk);

  /**
  Finds a vector that gives a high value of the
  absolute value of the i-th Lagrange polynomial.

  The search is currently based upon a random uniform search strategy.

  @return the value that gives the highest found value.
  */
  Eigen::VectorXd findHighValueOfAbsoluteLagrangePolynomial(int i);

  Eigen::VectorXd FindLocalOptimumOfAbsoluteLagrangePolynomial(int t);

  /**
  Finds the point that is best to replace with the new one.

  @yNew is the point that we want to add to the model, where yNew is given as the displacement of the current center point.
  @return index of the point that is best to replace.
  */
  int findPointToReplaceWithNewOptimum(Eigen::VectorXd yNew);

  /**
  Returns the index of the best point.

  Note: this will only be valid as long as the best yet found
  point is never removed from the interpolation set.

  @return the index of the best point.
  */
  int getBestPointIndex();

  /**
  Prints the Hessian, gradient, constant, center point and the trust-region radius
  in a matlab friendly format.

  The output can be copy-pasted to matlab.
  */
  void printParametersMatlabFriendly();

  /**
  Prints the quadratic model.

  Only used for debugging.
  */
  void printQuadraticModel();

  /**
  Prints the shifted matrix if pure matrix multplication is used.

  Only used for debugging. Does not alter any local variables.

  @param[in] s is the displacement from the current center point (y0)
  */
  void printSlowShiftCenterPointOfQuadraticModel(Eigen::VectorXd s);

  /**
  Calculate the H matrix explicitly based upon the interpolation points alone.

  Only used for debugging.
  Compares the iteratively updated H matrix with the one calculated explicitly.
  The result is printed.
  */
  void compareHMatrices();

  void SetFunctionValue(int t, double value);

  void SetTrustRegionRadiusForSubproblem(double radius);

  double GetFunctionValue(int t){
    return fvals[t-1];
  }

  double GetTrustRegionRadius(){
    return rho;
  }

  void SetTrustRegionRadius(double radius){
    rho = radius;
  }

  Eigen::VectorXd GetGradient(){
    return gradient;
  }
  Eigen::VectorXd GetPoint(int t){
    return Y.col(t-1);
  }

  Eigen::VectorXd GetBestPoint(){
    return Y.col(bestPointIndex);
  }

  Eigen::VectorXd FindLocalOptimum();

  int findPointFarthestAwayFromOptimum();

  double findLargestDistanceBetweenPointsAndOptimum();

  double ComputeLagrangePolynomial(int t, Eigen::VectorXd point);

  Eigen::VectorXd GetInterpolationPointsSortedByDistanceFromBestPoint();
};
}
}

#endif //FIELDOPT_DFO_MODEL_H