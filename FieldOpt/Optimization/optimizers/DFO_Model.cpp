#include "DFO_Model.h"
namespace Optimization {
namespace Optimizers {

bool DFO_Model::cmp(Eigen::VectorXd a, Eigen::VectorXd b) {
  return (norm(a.topRows(a.rows() - 1))) > (norm(b.topRows(b.rows() - 1)));
  //return ((a.topRows(a.rows() - 1)).norm() > (b.topRows(b.rows() - 1)).norm());
}

bool DFO_Model::isApproxZero(double value, double zeroLimit) {
  if (std::abs(value) <= zeroLimit)
    return true;
  return false;
}

int DFO_Model::kroneckerDelta(int i, int j) {
  if (i == j)
    return 1;
  return 0;
}

int DFO_Model::sign(double value) {
  if (value >= 0) {
    return 1;
  }
  return -1;
}

void DFO_Model::initializeQuadraticModel() {
  constant = fvals[0];
  if (n + 2 <= m && m <= 2 * n) {
    for (int i = 1; i <= m - n - 1; ++i) {
      gradient[i - 1] = (fvals[i] - fvals[i + n]) / (2 * rho);
      hessian(i - 1, i - 1) = (fvals[i] + fvals[i + n]) / (rho * rho) - 2 * constant / (rho * rho);
    }

    for (int i = m - n; i <= n; ++i) {
      gradient[i - 1] = (fvals[i] - fvals[0]) / rho;
    }
  } else if (m >= 2 * n + 1) {
    for (int i = 1; i <= n; ++i) {
      gradient[i - 1] = (fvals[i] - fvals[i + n]) / (2 * rho);
      hessian(i - 1, i - 1) = (fvals[i] + fvals[i + n]) / (rho * rho) - 2 * constant / (rho * rho);
    }
  }

  //Setting the off-diagonal terms in the hessian.
  if (m > 2 * n + 1) {
    for (int i = 0; i < ps.rows(); i++) {
      //First check if we used -rho when we created the point, if so, we must add an offset.
      int pHat = ps[i];

      if (sigmas[ps[i] - 1] < 0)
        pHat += n;

      int qHat = qs[i];
      if (sigmas[qs[i] - 1] < 0)
        qHat += n;

      hessian(ps[i] - 1, qs[i] - 1) =
          sigmas[ps[i] - 1] * sigmas[qs[i] - 1] * (fvals[0] - fvals[pHat] - fvals[qHat] + fvals[is[i] - 1])
              / (rho * rho);
      hessian(qs[i] - 1, ps[i] - 1) = hessian(ps[i] - 1, qs[i] - 1);
    }
  }

  Gamma = hessian;

  //Find the current best point
  int indexBest = 1;
  for (int i = 2; i < m; ++i) {
    if (fvals(i - 1) < fvals(indexBest - 1)) {
      indexBest = i;
    }
  }
  bestPointIndex = indexBest;
  bestPoint = Y.col(bestPointIndex - 1);
  bestPointAllTime = bestPoint;
  bestPointAllTimeFunctionValue = fvals(bestPointIndex - 1);

}

void DFO_Model::initializeInverseKKTMatrix() {
  Xi(0, 0) = 1;
  for (int i = 2; i <= std::min(n + 1, m - n); ++i) {
    Xi(i - 1, i - 1) = 0.5 / (rho);
    Xi(i - 1, i + n - 1) = -0.5 / (rho);
  }

  if (m < 2 * n + 1) {
    for (int i = m - n + 1; i <= n + 1; ++i) {
      Xi(i - 1, 0) = -1 / (rho);
      Xi(i - 1, i - 1) = 1 / (rho);
      Upsilon(i - 1, i - 1) = -0.5 * rho * rho;
    }
  }

  for (int k = 1; k <= std::min(n, m - n - 1); ++k) {
    Z(0, k - 1) = -std::sqrt(2) / (rho * rho);
    Z(k, k - 1) = 0.5 * std::sqrt(2) / (rho * rho);
    Z(k + n, k - 1) = 0.5 * std::sqrt(2) / (rho * rho);
  }

  if (m > 2 * n + 1) {
    for (int k = n + 1; k <= m - n - 1; ++k) {
      //First check if we used -rho when we created the point, if so, we must add an offset.
      int pHat = ps[k - n - 1];
      if (sigmas[ps[k - n - 1] - 1] < 0)
        pHat += n;

      int qHat = qs[k - n - 1];
      if (sigmas[qs[k - n - 1] - 1] < 0)
        qHat += n;

      Z(0, k - 1) = 1 / (rho * rho);
      Z(pHat, k - 1) = -1 / (rho * rho);
      Z(qHat, k - 1) = Z(pHat, k - 1);
      Z(k + n, k - 1) = 1 / (rho * rho);
    }
  }
}

void DFO_Model::updateInverseKKTMatrix(Eigen::VectorXd yNew, unsigned int t) {
  //std::cout << "Start update" << std::endl;
  //std::cout << S.diagonal() << std::endl;
  //std::cout << "t = " << t << ", Z \n" << Z << std::endl;
  Eigen::VectorXd w(n + m + 1);
  for (int i = 1; i <= m; ++i) {
    w(i - 1) = 0.5 * std::pow((Y.col(i - 1)).transpose() * (yNew), 2);
  }
  //w.tail(n) = yNew;
  eigen_tail(w, yNew, n);
  w(m) = 1;

  //Start by performing orthogonal rotations to the Z matrix. It will put zeros in the (t-1)'th row of Z;
  int indexFirstPositiveS = -1;
  int indexFirstNegativeS = -1;
  // All elements in S are either +1 or -1;
  for (int i = 0; i < S.diagonal().rows(); ++i) {
    if (indexFirstPositiveS == -1 && S.diagonal()(i) >= 0.9) {
      indexFirstPositiveS = i;
    }
    if (indexFirstNegativeS == -1 && S.diagonal()(i) <= -0.9) {
      indexFirstNegativeS = i;
    }
    if (indexFirstPositiveS != -1 && indexFirstNegativeS != -1) {
      break;
    }
  }

  double almostZero = std::max(std::abs(Z.maxCoeff()), std::abs(Z.minCoeff())) * 1e-20;

  int baseColPos = indexFirstPositiveS;
  int baseColNeg = indexFirstNegativeS;
  int baseCol;

  for (int i = 0; i < Z.cols(); ++i) {
    if (i != baseColPos && i != baseColNeg && !isApproxZero(Z(t - 1, i), almostZero)) {

      if (S.diagonal()(i) >= 0.9)
        baseCol = baseColPos;
      else {
        baseCol = baseColNeg;
      }

      double length = std::sqrt(std::pow(Z(t - 1, baseCol), 2) + std::pow(Z(t - 1, i), 2));
      double cosTheta = Z(t - 1, baseCol) / length;
      double sinTheta = Z(t - 1, i) / length;
      Eigen::VectorXd newBaseCol = cosTheta * Z.col(baseCol) + sinTheta * Z.col(i);
      //Z.col(i) = cosTheta * Z.col(i) - sinTheta * Z.col(baseColPos);
      eigen_col(Z, cosTheta * Z.col(i) - sinTheta * Z.col(baseCol), i);
      //Z.col(baseColPos) = newBaseCol;
      eigen_col(Z, newBaseCol, baseCol);
    }
  }
  //std::cout << "t = " << t << ", Z \n" << Z << std::endl;
  //Orthogonal rotations are now done.

  // Calculate Hw = H*w
  Eigen::VectorXd Hw = Eigen::VectorXd::Zero(m + n + 1);

  //Hw.head(m) = Z * S * (Z.transpose()) * (w.head(m)) + Xi.transpose() * (w.tail(n + 1));
  eigen_head(Hw, Z * S * (Z.transpose()) * (w.head(m)) + Xi.transpose() * (w.tail(n + 1)), m);
  //Hw.tail(n + 1) = Xi * w.head(m) + Upsilon * w.tail(n + 1);
  eigen_tail(Hw, Xi * w.head(m) + Upsilon * w.tail(n + 1), n + 1);


  // Calculate the updating parameters
  double alpha = Z.row(t - 1) * S * Z.row(t - 1).transpose();
  //double beta = 0.5 * std::pow(yNew.squaredNorm(), 2) - w.transpose() * Hw;
  double beta = std::max(0.0, 0.5 * std::pow(yNew.squaredNorm(), 2) - w.transpose() * Hw);
  double tau = Hw[t - 1];
  double sigma = alpha * beta + std::pow(tau, 2);

  /*
  std::cout << "l(x+) = " << Hw(t - 1) << "\t this should be nonzero \n \n \n";
  std::cout << "alpha = " << alpha << ". >= 0 ??" << std::endl;
  std::cout << "beta = " << beta << ". >= 0 ??" << std::endl;
  std::cout << "tau = " << tau << ". " << std::endl;
  std::cout << "sigma = " << sigma << ". 1/sigma is in the updating formula --> abs(sigma) shouldn't be too small" << std::endl << std::endl;
*/

  // Need to store some rows and columns such that we don't use a partly updated matrix instead of the original one.
  Eigen::VectorXd rowOmega = Z.row(t - 1) * S * Z.transpose();
  Eigen::VectorXd colXi = Xi.col(t - 1);

  // Update Xi and Upsilon
  for (int i = 0; i < n + 1; ++i) {
    for (int j = 0; j < m; ++j) {
      Xi(i, j) += 1.0 / sigma * (alpha * Hw(i + m) * (Hw(j) - kroneckerDelta(t - 1, j)) - beta * colXi(i) * rowOmega(j)
          + tau * (colXi(i) * (kroneckerDelta(t - 1, j) - Hw(j)) - Hw(i + m) * rowOmega(j)));
      if (j < n + 1) {
        Upsilon(i, j) += 1.0 / sigma * (alpha * Hw(i + m) * Hw(j + m) - beta * colXi(i) * colXi(j)
            + tau * (colXi(i) * (-Hw(j + m)) - Hw(i + m) * colXi(j)));
      }
    }
  }

  // Update Omega in the case when all elements of S are positive
  if (baseColNeg == -1) {

    S.diagonal()(baseColPos) = sign(sigma);
    Hw(t - 1) -= 1.0;
    //Z.col(baseColPos) = 1.0 / (std::sqrt(std::abs(sigma))) * (tau * Z.col(baseColPos) + Z(t - 1, baseColPos) * (-Hw.head(m)));
    eigen_col(Z,
              1.0 / (std::sqrt(std::abs(sigma))) * (tau * Z.col(baseColPos) + Z(t - 1, baseColPos) * (-Hw.head(m))),
              baseColPos);
    Hw(t - 1) += 1.0;

  }

    // Update Omega in the case when there are also 1 negative value in S.

    /// What to do when there is only 1 value in S ...?
    /// Tror vi kan
  else {
    // 1 = baseColPos;
    // 2 = baseColNeg;

    if (beta >= 0) {
      double zeta = std::pow(tau, 2) + beta * std::pow(Z(t - 1, baseColPos), 2);
      S.diagonal()(baseColPos) = 1; //Not really necessary to write this explicitly. Should be positive anyways!
      S.diagonal()(baseColNeg) = -sign(sigma);

      Hw(t - 1) -= 1.0;
      //Z.col(baseColPos) = 1.0 / (std::sqrt(std::abs(zeta))) * (tau * Z.col(baseColPos) + Z(t - 1, baseColPos) * (-Hw.head(m)));
      eigen_col(Z,
                1.0 / (std::sqrt(std::abs(zeta))) * (tau * Z.col(baseColPos) + Z(t - 1, baseColPos) * (-Hw.head(m))),
                baseColPos);
      Z.col(baseColNeg) = 1.0 / (std::sqrt(std::abs(zeta * sigma)))
          * (-beta * Z(t - 1, baseColPos) * Z(t - 1, baseColNeg) * Z.col(baseColPos) + zeta * Z.col(baseColNeg)
              + tau * Z(t - 1, baseColNeg) * (-Hw.head(m)));
      eigen_col(Z, 1.0 / (std::sqrt(std::abs(zeta * sigma)))
          * (-beta * Z(t - 1, baseColPos) * Z(t - 1, baseColNeg) * Z.col(baseColPos) + zeta * Z.col(baseColNeg)
              + tau * Z(t - 1, baseColNeg) * (-Hw.head(m))), baseColNeg);
      Hw(t - 1) += 1.0;

    } else {
      double zeta = std::pow(tau, 2) - beta * std::pow(Z(t - 1, baseColNeg), 2);
      S.diagonal()(baseColPos) = sign(sigma);
      S.diagonal()(baseColNeg) = -1; //Not really necessary to write this explicitly. Should be negative anyways!

      Hw(t - 1) -= 1.0;
      //Z.col(baseColPos) = 1.0 / (std::sqrt(std::abs(zeta * sigma))) * (zeta * Z.col(baseColPos) + beta * Z(t - 1, baseColPos) * Z(t - 1, baseColNeg) * Z.col(baseColNeg) + tau * Z(t - 1, baseColPos) * (-Hw.head(m)));
      eigen_col(Z, 1.0 / (std::sqrt(std::abs(zeta * sigma)))
          * (zeta * Z.col(baseColPos) + beta * Z(t - 1, baseColPos) * Z(t - 1, baseColNeg) * Z.col(baseColNeg)
              + tau * Z(t - 1, baseColPos) * (-Hw.head(m))), baseColPos);
      //Z.col(baseColNeg) = 1.0 / (std::sqrt(std::abs(zeta))) * (tau * Z.col(baseColNeg) + Z(t - 1, baseColNeg) * (-Hw.head(m)));
      eigen_col(Z,
                1.0 / (std::sqrt(std::abs(zeta))) * (tau * Z.col(baseColNeg) + Z(t - 1, baseColNeg) * (-Hw.head(m))),
                baseColNeg);
      Hw(t - 1) += 1.0;
    }

  }
  /*
  std::cout << "---------" << std::endl;
  std::cout << S.diagonal() << std::endl;
  std::cout << "End update" << std::endl;
*/
}

void DFO_Model::updateQuadraticModel(Eigen::VectorXd yNew, double fvalNew, unsigned int t) {
  double modelValueYNew = evaluateQuadraticModel(yNew);

  //modelValueYNew = 0;

  //constant = 0;
  //gradient.setZero();
  //Gamma.setZero();
  //gammas.setZero();

  constant += (fvalNew - modelValueYNew) * Xi(0, t - 1);
  gradient += (fvalNew - modelValueYNew) * (Xi.col(t - 1).tail(n));

  Eigen::VectorXd colOmega = Z * S * (Z.row(t - 1)).transpose();

  Gamma += gammas(t - 1) * Y.col(t - 1) * Y.col(t - 1).transpose();
  gammas(t - 1) = (fvalNew - modelValueYNew) * colOmega(t - 1);

  for (int i = 1; i <= m; ++i) {
    if (i != t) {
      gammas(i - 1) += (fvalNew - modelValueYNew) * colOmega(i - 1);
    }
  }

}

double DFO_Model::evaluateLowerBoundQuadraticPolynomial(double radius, double b, int type) {
  switch (type) {
    case 0: return b;
      break;
    case 1: return b * radius;
      break;
    case 2: return 0.25 * b * radius * radius;
      break;
    case 3: return 0.5 * b * radius * radius;
      break;

  }
}

DFO_Model::DFO_Model(unsigned int m,
                     unsigned int n,
                     unsigned int ng,
                     Eigen::VectorXd y0,
                     double rhoBeg,
                     double lambda,
                     double weight_objective_minimum_change,
                     QList<double> weights_derivatives,
                     Settings::Optimizer *settings)
    : subproblem(settings), enhancedModel(n,m,ng,weights_derivatives,weight_objective_minimum_change) {
  //this->normType = 0;
  this->m = m;
  this->n = n;
  //this->y0 = y0;
  this->y0 = Eigen::VectorXd::Zero(n);
  //this->y0 << 1,2;
  this->y0[0] = 10 + 3;
  this->y0[1] = 10 + 2;
  this->y0[2] = 10 - 4;
  //std::cout << "y0\n" << y0 << "\ny0this\n" << this->y0 << "\n";
  //this->y0.setZero();
  this->rho = rhoBeg;
  this->lambda = lambda;

  this->bestPoint = Eigen::VectorXd::Zero(n);
  this->bestPointAllTime = Eigen::VectorXd::Zero(n);
  this->bestPointAllTimeFunctionValue = std::numeric_limits<double>::max();
  this->Y = Eigen::MatrixXd::Zero(n, m);
  this->derivatives = Eigen::MatrixXd(ng, m);
  this->fvals = Eigen::VectorXd(m);
  this->Xi = Eigen::MatrixXd::Zero(n + 1, m);
  this->Upsilon = Eigen::MatrixXd::Zero(n + 1, n + 1);
  this->Z = Eigen::MatrixXd::Zero(m, m - n - 1);
  this->S = Eigen::DiagonalMatrix<double, Eigen::Dynamic>(m - n - 1);
  this->S.diagonal().setOnes();

  if (m > 2 * n + 1) {
    this->sigmas = Eigen::VectorXi(n);
    this->ps = Eigen::VectorXi(m - 2 * n - 1);
    this->qs = Eigen::VectorXi(m - 2 * n - 1);
    this->is = Eigen::VectorXi(m - 2 * n - 1);
  }

  this->gradient = Eigen::VectorXd::Zero(n);
  this->centerPoint = Eigen::VectorXd::Zero(n);
  this->hessian = Eigen::MatrixXd::Zero(n, n);
  this->Gamma = Eigen::MatrixXd::Zero(n, n);
  this->gammas = Eigen::VectorXd::Zero(m);
  this->initialInterpolationPointsFound = false;
  this->modelInitialized = false;
  this->settings_ = settings;

  //subproblem.SetNormType(Subproblem::L2_NORM);

/*
  vector<double> xsol;
  vector<double> fsol;
  //mySub.Solve(xsol, fsol, (char*)"Maximize");
  mySub.Solve(xsol, fsol, (char*)"Minimize");
  */
}

Eigen::MatrixXd DFO_Model::findFirstSetOfInterpolationPoints() {
  int numberOfPointsFound = 0;
  if (m >= 2 * n + 1 && m <= (n + 1) * (n + 2) * 0.5) {
    for (int i = 1; i <= n; ++i) {
      //Y.col(i)[i - 1] += rho;
      Y(i - 1, i) += rho;
      //Y.col(i + n)[i - 1] -= rho;
      Y(i - 1, i + n) -= rho;
    }
    numberOfPointsFound = 2 * n;
  } else if (m >= n + 2 && m <= 2 * n) {
    for (int i = 1; i <= n; ++i) {
      //Y.col(i)[i - 1] += rho;
      Y(i - 1, i) += rho;
    }
    for (int i = 1; i < m - n; ++i) {
      //Y.col(i + n)[i - 1] -= rho;
      Y(i - 1, i + n) -= rho;
    }
    numberOfPointsFound = m;
    initialInterpolationPointsFound = true;
  } else {
    std::cout << "Invalid value of m = " << m << ". Choose  n+2 <= m <= (n+1)(n+2)/2. Recommended: m = 2n+1."
              << std::endl;
    std::cin.get();
    std::exit(1);
  }

  return Y.block(0, 0, n, numberOfPointsFound);
}

Eigen::MatrixXd DFO_Model::findLastSetOfInterpolationPoints() {
  if (m > 2 * n + 1) {
    // Calculate the sigmas
    for (int i = 1; i <= n; ++i) {
      if (fvals[i + n] < fvals[i])
        sigmas[i - 1] = -1;
      else
        sigmas[i - 1] = 1;
    }

    // Find the last set of interpolation points, while also storing the ps, qs and is for later usage.
    // This is a bit messy, for the mathematical expression see (2.4) in [1]
    int j = 2 * n + 2;
    int l = 1; //Number of cycles  (i.e. number of times i has become  i == 3 * n + 2)
    int p;
    int q;
    int index = 0;

    for (int i = 2 * n + 2; i <= m; ++i) {

      if (j >= 3 * n + 2) {
        j = j - n;
        l++;
      }
      p = (j - 2 * n - 1);

      if (p + l >= 1 && p + l <= n)
        q = p + l;
      else
        q = p + l - n;

      ps[index] = p;
      qs[index] = q;
      is[index] = i;

      Y(p - 1, i - 1) += rho * sigmas[p - 1];
      Y(q - 1, i - 1) += rho * sigmas[q - 1];

      index++;
      j++;

    }
  } else {
    std::cout << "findLastSetOfInterpilationPoints() was called when m <= 2*n + 1" << std::endl;
  }
  initialInterpolationPointsFound = true;
  return Y.block(0, 2 * n, n, m - 2 * n);
}

void DFO_Model::initializeModel() {
  initializeQuadraticModel();
  initializeInverseKKTMatrix();
  modelInitialized = true;

}

void DFO_Model::update(Eigen::VectorXd yNew, double fvalNew, unsigned int t, UpdateReason updateReason) {
  int oldBestPointIndex = bestPointIndex;
  int oldBestFval = fvals[bestPointIndex - 1];



  if (updateReason == INCLUDE_NEW_OPTIMUM) {
    if (fvalNew < bestPointAllTimeFunctionValue) {
      bestPointAllTimeFunctionValue = fvalNew;
      bestPointAllTime = yNew;
      printf("\x1b[33mNEW BEST POINT IS FOUND! (ALL TIME): \x1b[0m");
      std::cout << fvalNew << "\n";
      //std::cout << "NEW BEST POINT IS FOUND! (ALL TIME)" << std::endl;
    }
  }

  if (updateReason == IMPROVE_POISEDNESS) {
    if (fvalNew < bestPointAllTimeFunctionValue) {
      bestPointAllTimeFunctionValue = fvalNew;
      bestPointAllTime = yNew;
      std::cout << "\033[1;36;mOBS OBS OBS, improving poisedness: \033[0m" << std::endl;
      printf("\x1b[33mNEW BEST POINT IS FOUND! (ALL TIME): \x1b[0m");
      std::cout << fvalNew << "\n";
      //std::cout << "NEW BEST POINT IS FOUND! (ALL TIME)" << std::endl;
    }
  }
  if (updateReason == INCLUDE_NEW_POINT) {
    if (fvalNew < bestPointAllTimeFunctionValue) {
      bestPointAllTimeFunctionValue = fvalNew;
      bestPointAllTime = yNew;
      std::cout << "\033[1;36;mOBS OBS OBS, include new point: \033[0m" << std::endl;
      printf("\x1b[33mNEW BEST POINT IS FOUND! (ALL TIME): \x1b[0m");
      std::cout << fvalNew << "\n";
      //std::cout << "NEW BEST POINT IS FOUND! (ALL TIME)" << std::endl;
    }
  }

  if (updateReason == FORCED_IMPROVE_MODEL){

  }
  else{
  if (fvalNew < fvals[bestPointIndex - 1] ) {
    bestPoint = yNew;
    bestPointIndex = t;
  }
  }

  //std::cout << "Old point: \n" << Y.col(t-1) << "\nNew point: \n" << yNew << std::endl;

  updateInverseKKTMatrix(yNew, t);
  updateQuadraticModel(yNew, fvalNew, t);

  //Y.col(t - 1) = yNew;
  eigen_col(Y, yNew, t - 1);
  fvals(t - 1) = fvalNew;

  if ((updateReason == IMPROVE_POISEDNESS || updateReason == INCLUDE_NEW_POINT))  {
    if (t == oldBestPointIndex && fvalNew > oldBestFval) { // removing optimum :(
      bestPointIndex = 1;
      for (int j = 2; j <= m; ++j) {
        if (fvals[j - 1] < fvals[bestPointIndex - 1]) {
          bestPointIndex = j;
        }
      }
      bestPoint = Y.col(bestPointIndex - 1);

      if (updateReason == IMPROVE_POISEDNESS) {
        std::cout << "Replaced optimum while IMPROVE_POISEDNESS" << std::endl;
      }
      if (updateReason == INCLUDE_NEW_POINT) {
        std::cout << "Replaced optimum while INCLUDE_NEW_POINT" << std::endl;
      }
    }
  }

}

double DFO_Model::evaluateQuadraticModel(Eigen::VectorXd point) {
  double value = 0;
  value += constant;
  value += gradient.transpose() * (point);
  value += 0.5 * (point).transpose() * (Gamma * (point));
  for (int i = 1; i <= m; ++i) {
    double tmp = (gammas(i - 1) * (Y.col(i - 1)).transpose() * (point));
    value += 0.5 * (point).transpose() * (tmp * Y.col(i - 1));
  }
  return value;
}

void DFO_Model::shiftCenterPointOfQuadraticModel(Eigen::VectorXd s) {
  Eigen::VectorXd r(n);
  Eigen::MatrixXd P(n, m);

  double squaredNorm = s.squaredNorm();
  for (int k = 1; k <= m; ++k) {
    r = Y.col(k - 1) - 0.5 * s;
    //P.col(k - 1) = (s.transpose() * r) * r + 0.25 * squaredNorm * s;
    double tmp0 = (s.transpose() * r);
    Eigen::VectorXd tmp1 = tmp0 * r;
    Eigen::VectorXd tmp2 = 0.25 * squaredNorm * s;
    eigen_col(P, tmp1 + tmp2, k - 1);
  }

  // Pre multiply by inverse(transpose(omega_X))
  for (int i = 1; i <= n; ++i) {
    // Xi.row(0) += 0.5 * s(i - 1) * Xi.row(i);
    eigen_row(Xi, Xi.row(0) + 0.5 * s(i - 1) * Xi.row(i), 0);

    //Upsilon.row(0) += 0.5 * s(i - 1) * Upsilon.row(i);
    eigen_row(Upsilon, Upsilon.row(0) + 0.5 * s(i - 1) * Upsilon.row(i), 0);
  }

  // Post multiply by inverse(omega_X)
  for (int i = 1; i <= n; ++i) {
    //Upsilon.col(0) += 0.5 * s(i - 1) * Upsilon.col(i);
    eigen_col(Upsilon, Upsilon.col(0) + 0.5 * s(i - 1) * Upsilon.col(i), 0);
  }

  Eigen::MatrixXd Omega(m, m);
  Omega = Z * S * Z.transpose();

  Eigen::MatrixXd CopyXiT(n + 1, m);
  CopyXiT = Xi.transpose();

  // Pre multiply by inverse(transpose(omega_A)) and post multiply by inverse(omega_A)
  for (int i = 1; i <= n; ++i) {
    for (int k = 1; k <= m; ++k) {
      //Upsilon.row(i) += P(i - 1, k - 1) * CopyXiT.row(k - 1);
      eigen_row(Upsilon, Upsilon.row(i) + P(i - 1, k - 1) * CopyXiT.row(k - 1), i);
      //Xi.row(i) += P(i - 1, k - 1) * Omega.row(k - 1);
      eigen_row(Xi, Xi.row(i) + P(i - 1, k - 1) * Omega.row(k - 1), i);
    }
  }

  for (int k = 1; k <= m; ++k) {
    for (int i = 1; i <= n; ++i) {
      //Upsilon.col(i) += P(i - 1, k - 1) * Xi.col(k - 1);
      eigen_col(Upsilon, Upsilon.col(i) + P(i - 1, k - 1) * Xi.col(k - 1), i);
    }
  }

  // Pre multiply by inverse(transpose(omega_X))
  for (int i = 1; i <= n; ++i) {
    //Xi.row(0) += 0.5 * s(i - 1) * Xi.row(i);
    eigen_row(Xi, Xi.row(0) + 0.5 * s(i - 1) * Xi.row(i), 0);
    //Upsilon.row(0) += 0.5 * s(i - 1) * Upsilon.row(i);
    eigen_row(Upsilon, Upsilon.row(0) + 0.5 * s(i - 1) * Upsilon.row(i), 0);
  }

  // Post multiply by inverse(omega_X)
  for (int i = 1; i <= n; ++i) {
    //Upsilon.col(0) += 0.5 * s(i - 1) * Upsilon.col(i);
    eigen_col(Upsilon, Upsilon.col(0) + 0.5 * s(i - 1) * Upsilon.col(i), 0);
  }

  //Update the stored constant
  constant += gradient.transpose() * s;
  constant += 0.5 * (s).transpose() * (Gamma * (s));
  for (int i = 1; i <= m; ++i) {
    double tmp0 = (gammas(i - 1) * (Y.col(i - 1)).transpose() * (s));
    constant += 0.5 * (s).transpose() * (tmp0 * Y.col(i - 1));
  }

  //Update the stored gradient
  gradient += Gamma * s;
  for (int i = 1; i <= m; ++i) {
    double tmp0 = (gammas(i - 1) * (Y.col(i - 1)).transpose() * (s));
    gradient += (tmp0 * Y.col(i - 1));
  }

  //Update the stored Gamma
  Eigen::VectorXd v = Eigen::VectorXd::Zero(n);
  for (int i = 1; i <= m; ++i) {
    v += gammas(i - 1) * (Y.col(i - 1) - 0.5 * s);
  }
  Gamma += v * s.transpose() + s * v.transpose();

  // Update Y because it should be the vectors of displacements of the
  //     interpolation points from the center point.
  for (int i = 1; i <= m; ++i) {
    //Y.col(i - 1) -= s;
    eigen_col(Y, Y.col(i - 1) - s, i - 1);
  }
  bestPoint -= s;
  bestPointAllTime -= s;
  y0 += s;
}

void DFO_Model::findLowerAndUpperBoundOnAbsoluteLagrangePolynomial(int i, double &lowerBound, double &upperBound) {
  double maxCoeffVal = 0;
  Eigen::VectorXd grad;
  Eigen::MatrixXd hess = Eigen::MatrixXd::Zero(n, n);
  // Creating the Lagrange polynomial.
  double c = Xi(0, i - 1);
  grad = (Xi.col(i - 1)).tail(n);
  for (int k = 1; k <= m; ++k) {
    double tmp = Z.row(k - 1) * S * (Z.row(i - 1)).transpose();
    hess += tmp * (Y.col(k - 1)) * (Y.col(k - 1)).transpose();
  }

  int type = 0;
  maxCoeffVal = c;

  for (int j = 0; j < n; ++j) {
    if (grad(j) > maxCoeffVal) {
      maxCoeffVal = grad(j);
      type = 1;
    } else if (grad(j) == maxCoeffVal) {
      if (type == 0 && (evaluateLowerBoundQuadraticPolynomial(rho, maxCoeffVal, 1)
          >= evaluateLowerBoundQuadraticPolynomial(rho, maxCoeffVal, 0))) {
        type = 1;
      }
    }
  }

  for (int j = 0; j < n; ++j) {
    if (hess(j, j) > maxCoeffVal) {
      maxCoeffVal = hess(j, j);
      type = 2;
    } else if (hess(j, j) == maxCoeffVal) {
      if (type == 0 && (evaluateLowerBoundQuadraticPolynomial(rho, maxCoeffVal, 2)
          >= evaluateLowerBoundQuadraticPolynomial(rho, maxCoeffVal, 0))) {
        type = 2;
      } else if (type == 1 && (evaluateLowerBoundQuadraticPolynomial(rho, maxCoeffVal, 2)
          >= evaluateLowerBoundQuadraticPolynomial(rho, maxCoeffVal, 1))) {
        type = 2;
      }
    }
  }

  for (int j = 0; j < n; ++j) {
    for (int k = 0; k < n; ++k) {
      if (k > j) { // The matrix is symmetric --> no need to check twice! Diagonal already checked.
        if (hess(j, k) > maxCoeffVal) {
          maxCoeffVal = hess(j, k);
          type = 3;
        } else if (hess(j, k) == maxCoeffVal) {
          if (type == 0 && (evaluateLowerBoundQuadraticPolynomial(rho, maxCoeffVal, 3)
              >= evaluateLowerBoundQuadraticPolynomial(rho, maxCoeffVal, 0))) {
            type = 3;
          } else if (type == 1 && (evaluateLowerBoundQuadraticPolynomial(rho, maxCoeffVal, 3)
              >= evaluateLowerBoundQuadraticPolynomial(rho, maxCoeffVal, 1))) {
            type = 3;
          } else if (type == 2 && (evaluateLowerBoundQuadraticPolynomial(rho, maxCoeffVal, 3)
              >= evaluateLowerBoundQuadraticPolynomial(rho, maxCoeffVal, 2))) {
            type = 3;
          }
        }
      }
    }
  }

  // Lower bound:
  lowerBound = evaluateLowerBoundQuadraticPolynomial(rho, maxCoeffVal, type);

  // Upper bound:
  upperBound = std::abs(c);

  for (int j = 0; j < n; ++j) {
    upperBound += std::abs(grad(j)) * rho;
  }

  for (int j = 0; j < n; ++j) {
    upperBound += 0.5 * std::abs(hess(j, j)) * rho * rho;
  }

  for (int j = 0; j < n; ++j) {
    for (int k = j + 1; k < n; ++k) {
      upperBound += std::abs(hess(j, k)) * rho * rho;
    }
  }
}

void DFO_Model::findWorstPointInInterpolationSet(Eigen::VectorXd &dNew, int &indexOfWorstPoint) {
  // For hvert lagrange polynom l_i: finn max |l_i(x)| innenfor sirkelen.
  // Altså: max { max(l_i(x)), max(-l_i(x))  }

  //
  //std::cout <<"Z\n"<< Z << "\nS\n"<<S.diagonal()<<"\nY\n" << Y << "\n\n";
  double worstPoisedness = 0;
  Eigen::VectorXd poisedness(m);
 // Eigen::VectorXd poisedness2(m);
  int index = -1;

  Eigen::VectorXd grad;
  Eigen::MatrixXd hess = Eigen::MatrixXd::Zero(n, n);
  // Creating the lagrange polynomial.
  for (int t = 1; t <= m; ++t) {
    hess.setZero();
    double c = Xi(0, t - 1);
    grad = (Xi.col(t - 1)).tail(n);
    for (int k = 1; k <= m; ++k) {
      double tmp = Z.row(k - 1) * S * (Z.row(t - 1)).transpose();
      hess += tmp * (Y.col(k - 1)) * (Y.col(k - 1)).transpose();
      //hess += Z.row(k - 1) * S * (Z.row(t - 1)).transpose() * (Y.col(k - 1)) * (Y.col(k - 1)).transpose();
    }

    // Find min and max of l_t(x)
    subproblem.setConstant(c);
    subproblem.setGradient(grad);
    subproblem.setHessian(hess);
    vector<double> xsolMax;
    vector<double> fsolMax;
    vector<double> xsolMin;
    vector<double> fsolMin;
    //subproblem.SetTrustRegionRadius(GetTrustRegionRadius());
    subproblem.Solve(xsolMax, fsolMax, (char *) "Maximize", y0, bestPoint,bestPoint);
    subproblem.Solve(xsolMin, fsolMin, (char *) "Minimize", y0, bestPoint,bestPoint);
    poisedness(t-1) = std::max(abs(fsolMax[0]),abs(fsolMin[0]));

    Eigen::VectorXd d1(n);
    Eigen::VectorXd d2(n);
    for (int i = 0; i < n; ++i){
      d1[i] = xsolMax[i];
      d2[i] = xsolMin[i];
    }

    //poisedness2(t-1) = (abs(fsolMax[0]) > abs(fsolMin[0])) ?  abs(ComputeLagrangePolynomial(t,d1)) : abs(ComputeLagrangePolynomial(t,d2)) ;
    double temp = 0;
    if ((abs(fsolMax[0]) >= abs(fsolMin[0])) && abs(fsolMax[0]) >= worstPoisedness) {
      worstPoisedness = abs(fsolMax[0]);
      for (int i = 0; i < xsolMax.size(); ++i) {
        dNew[i] = xsolMax[i];
      }
      index = t;
    } else if ((abs(fsolMin[0]) > abs(fsolMax[0])) && abs(fsolMin[0]) >= worstPoisedness) {
      worstPoisedness = abs(fsolMin[0]);
      for (int i = 0; i < xsolMin.size(); ++i) {
        dNew[i] = xsolMin[i];
      }
      index = t;
    }
  }
  if (worstPoisedness > lambda) {
    indexOfWorstPoint = index;

    hess.setZero();
    double c = Xi(0, indexOfWorstPoint - 1);
    grad = (Xi.col(indexOfWorstPoint - 1)).tail(n);
    for (int k = 1; k <= m; ++k) {
      double tmp = Z.row(k - 1) * S * (Z.row(indexOfWorstPoint - 1)).transpose();
      hess += tmp * (Y.col(k - 1)) * (Y.col(k - 1)).transpose();
      //hess += Z.row(k - 1) * S * (Z.row(t - 1)).transpose() * (Y.col(k - 1)) * (Y.col(k - 1)).transpose();
    }
    subproblem.setConstant(c);
    subproblem.setGradient(grad);
    subproblem.setHessian(hess);
    vector<double> xsolMax;
    vector<double> fsolMax;
    vector<double> xsolMin;
    vector<double> fsolMin;

    subproblem.SetTrustRegionRadius(GetTrustRegionRadius());
    subproblem.Solve(xsolMax, fsolMax, (char *) "Maximize", y0, bestPoint,bestPoint);
    subproblem.Solve(xsolMin, fsolMin, (char *) "Minimize", y0, bestPoint,bestPoint);

    if ((abs(fsolMax[0]) >= abs(fsolMin[0]))) {
      for (int i = 0; i < xsolMax.size(); ++i) {
        dNew[i] = xsolMax[i];
      }
    } else {
      for (int i = 0; i < xsolMin.size(); ++i) {
        dNew[i] = xsolMin[i];
      }
    }
  } else {
    indexOfWorstPoint = -1; // Indicates that the required poisedness is already achieved
  }
  //std::cout << "Required poisedness: " << lambda << "\nPoisedness: " << worstPoisedness << "\nWorst poisedness because of: " << indexOfWorstPoint
  //          << "-----------\nPoisedness for each sample point (in r*radius): \n" << poisedness << "\n";


  //std::cout << "-----------\nPoisedness for each sample point (in radius): \n" << poisedness << "\n";

  std::cout << "Required poisedness: " << lambda << "\nPoisedness: " << worstPoisedness << "\n";

}

void DFO_Model::findWorstPointInInterpolationSetByLU(Eigen::VectorXd &dNew, int &indexOfWorstPoint) {
  /// Trying LU pivoting instead.
  std::cout << "\n\n\n\n\nNEW ITERATION\n";
  Eigen::MatrixXd copyY = Y;
  Eigen::MatrixXd A(m, m);
  A.setZero();
  Eigen::VectorXi id(m);
  Eigen::MatrixXd changes = Eigen::MatrixXd::Zero(m, m);
  Eigen::VectorXd y0_PU = copyY.col(0);

  // scaling and shifting the interpolation set to be within a ball of radius 1 centered at the origin.
  double scaling = 0;
  for (int i = 1; i <= m; ++i) {
    eigen_col(copyY, copyY.col(i - 1) - y0_PU, i - 1);
    id(i - 1) = i;
    if (copyY.col(i-1).norm() > scaling) {
      scaling = copyY.col(i-1).norm();
    }

  }
  for (int i = 1; i <= m; ++i) {
    eigen_col(copyY, copyY.col(i - 1) / scaling, i - 1);
  }

  calculateAMatrix(A, copyY);
  eigen_col(changes, A.col(m - 1), m - 1);
  Eigen::VectorXd row1(m);
  Eigen::VectorXd row2(m);
  std::cout << "bestPointIndex = " << bestPointIndex << std::endl;
  std::cout << "Y \n" << Y << std::endl;
  std::cout << "fvals \n" << fvals.transpose() << "\n";
  swapRows(A,changes,id,bestPointIndex,1);
  // Used to store row and column
  int r = -1;
  int c = -1;

  pivotingLU(A, changes, id, r, c);


  //std::cout << "A after\n" << A << "\n";
  //std::cout << "changes\n" << changes << "\n";

  if ( r == -1 && c == -1){
    r = m;
    c = m;
  } else{
    std::cout << "A bad pivot occured during the pivoting. Will redo to get the correct changes.\n";
    calculateAMatrix(A, copyY);
    changes.setZero();
    eigen_col(changes, A.col(c - 1), c - 1);
    for (int i = 1; i <= m; ++i) {
      id(i - 1) = i;
    }
    // Swap such that best point is at first row.
    swapRows(A,changes,id,1, bestPointIndex);
    pivotingLU(A,changes, id, r, c);
  }

  if (std::abs(A(r - 1, c - 1)) >= 0.01) {
    indexOfWorstPoint = -1; // Done. The interpolation set is well-poised
  } else {
    Subproblem_LU mySubLu(settings_);
    mySubLu.SetCoefficients(changes);

    vector<double> xsolMax;
    vector<double> fsolMax;
    vector<double> xsolMin;
    vector<double> fsolMin;
    mySubLu.Solve(xsolMax, fsolMax, (char *) "Maximize", Eigen::VectorXd::Zero(n), Eigen::VectorXd::Zero(n));
    mySubLu.Solve(xsolMin, fsolMin, (char *) "Minimize", Eigen::VectorXd::Zero(n), Eigen::VectorXd::Zero(n));

    Eigen::VectorXd optimum(n);

    for (int i = 0; i < xsolMax.size(); ++i) {
      if (abs(fsolMax[0]) >= abs(fsolMin[0])) {
        optimum[i] = xsolMax[i];
      } else {
        optimum[i] = xsolMin[i];
      }
    }

    indexOfWorstPoint = id(c - 1);
    dNew = optimum * scaling + y0_PU;
  }

  std::cout << "\nDONE\n\n\n\n\n";

}

void DFO_Model::pivotingLU(Eigen::MatrixXd &A, Eigen::MatrixXd &changes, Eigen::VectorXi &id, int &r, int &c){
  Eigen::VectorXd row1 = A.row(0);
  Eigen::VectorXd row2 = A.row(bestPointIndex - 1);
  //std::cout << "A at the begining\n" << A << "\n";

  int colToStore = m;

  if (c != -1){
    colToStore = c;
  }

  for (int i = 1; i <= m; ++i) {
    int indexOfHighestPivot = 0;
    int indexOfLowestPivot = 0;
    //Eigen // copy vector..
    //std::cout << "A matrix, iteration: " << i << "\n" << A <<"\n";
    A.col(i - 1).maxCoeff(&indexOfHighestPivot);
    A.col(i - 1).minCoeff(&indexOfLowestPivot);
    int index = indexOfLowestPivot +1;
    if (std::abs(A(indexOfHighestPivot,i-1)) >= std::abs(A(indexOfLowestPivot,i-1))){
      index = indexOfHighestPivot +1;
    }

    // don't swap  first row, if pivot is too small.
    if (i != 1){ // && std::abs(A(index-1,i-1)) > 0.1) {
      swapRows(A,changes,id,index,i);
      //std::cout << "A\n" << A << "\n";
    }
    if (std::abs(A(i-1,i-1)) > 0.01) {
      for (int j = i + 1; j <= m; ++j) {
        double scale = A(i - 1, j - 1) / A(i - 1, i - 1);
        eigen_block(A, A.col(j - 1) - scale * A.col(i - 1), 0, j - 1);
        if (j == colToStore) {
          eigen_col(changes, -scale * A.col(i - 1), i - 1);
        }
      }
      //std::cout << "Should have zeros on (incl.) row: "<< i << "\t ";
      //std::cout << "A\n" << A << "\n";

    }
    else{
      if (i != m){
        r = i; /// hvilke verdier skal det være her? alltid på diagonalen?
        c = i;
        //std::cout << "Magnitude of pivot is too small during pivoting. Need to find new point.\n";
        //std::cout << "r = " << r << "\tc = " << c << "\n";
      }

      //std::cout << "A after\n" << A << "\n";
      break;
    }
  }
  //std::cout << "A after pivoting, (r,c) = " << r << ", " << c << "\n" << A << "\n";

}

void DFO_Model::swapRows(Eigen::MatrixXd &A, Eigen::MatrixXd &changes, Eigen::VectorXi &id, int r1, int r2) {
  if (r1 != r2){
    //std::cout << "swapping rows: " << r1 << " and " << r2 << std::endl;
    Eigen::VectorXd row1(m);
    Eigen::VectorXd row2(m);
    // Swap such that best point is at first row.
    row1 = A.row(r1-1);
    row2 = A.row(r2 - 1);
    eigen_row(A,row1,r2 - 1);
    eigen_row(A,row2, r1-1);

    row1 = changes.row(r1-1);
    row2 = changes.row(r2 - 1);
    eigen_row(changes,row1,r2 - 1);
    eigen_row(changes,row2, r1-1);

    changes.row(r1-1).swap(changes.row(r2 - 1));

    int tmp = id(r1-1);
    id(r1-1) = r2;
    id(r2 - 1) = tmp;
  }
}

void DFO_Model::calculateAMatrix(Eigen::MatrixXd &A, Eigen::MatrixXd &Ycopy) {
  int elem = 0;
  /// Constant
  for (int i = 1; i <= m; ++i) {
    A(i - 1, 0) = 1;
  }
  elem++;
  /// Linear
  for (int i = 1; i <= n; ++i) {
    if (elem < m) {
      for (int j = 1; j <= m; ++j) {
        A(j - 1, elem) = Ycopy(i - 1, j - 1);
      }
      elem++;
    } else {
      break;
    }
  }
  /// Squared
  for (int i = 1; i <= n; ++i) {
    if (elem < m) {
      for (int j = 1; j <= m; ++j) {
        A(j - 1, elem) = Ycopy(i - 1, j - 1) * Ycopy(i - 1, j - 1);
      }
      elem++;
    } else {
      break;
    }
  }
  /// Cross terms
  int rows = n;
  int it = 1;
  int col = 1;
  int t = 2;
  for (int k = 1; k <= n - 1; ++k) {
    for (int i = t; i <= n - 1; ++i) {
      for (int j = 1; j <= m; ++j) {
        if (elem < m) {
          double x1 = Ycopy(k - 1, j - 1);
          double x2 = Ycopy(i - 1, j - 1);
          A(j - 1, elem) = x1 * x2;
        } else {
          break;
        }
      }
      elem++;
    }
    t++;
  }
}

void DFO_Model::findPointToImprovePoisedness(Eigen::VectorXd &dNew, int &yk) {
  // Choose which Lagrange polynomial to work with. (rowindex + 1 is the chosen polynomial)
  Eigen::MatrixXd poisednessBounds(m, 2);
  Eigen::VectorXd newPoint(n);

  for (int i = 1; i <= m; ++i) {
    findLowerAndUpperBoundOnAbsoluteLagrangePolynomial(i, poisednessBounds(i - 1, 0), poisednessBounds(i - 1, 1));
  }
  int rowIndex;
  int colIndex;
  double highestPoisednessUpperBound = 0;
  double highestPoisednessLowerBound = 0;
  highestPoisednessUpperBound = poisednessBounds.col(1).maxCoeff(&rowIndex, &colIndex);
  highestPoisednessLowerBound = poisednessBounds(rowIndex, 0);

  Eigen::VectorXd grad;
  Eigen::MatrixXd hess = Eigen::MatrixXd::Zero(n, n);
  // Creating the lagrange polynomial.
  double c = Xi(0, rowIndex + 1 - 1);
  grad = (Xi.col(rowIndex + 1 - 1)).tail(n);
  for (int k = 1; k <= m; ++k) {
    double tmp = Z.row(k - 1) * S * (Z.row(rowIndex + 1 - 1)).transpose();
    hess += tmp * (Y.col(k - 1)) * (Y.col(k - 1)).transpose();
  }

  if (highestPoisednessUpperBound > lambda) {
    dNew = findHighValueOfAbsoluteLagrangePolynomial(rowIndex + 1);
    yk = rowIndex + 1;
  } else if (highestPoisednessLowerBound <= lambda) {
    yk = -1;
  } else {
    std::cout << "Estimate of the upper and lower bounds are too bad." << std::endl;
    yk = -1;
  }
}

Eigen::VectorXd DFO_Model::findHighValueOfAbsoluteLagrangePolynomial(int t) {
  Eigen::VectorXd grad;
  Eigen::MatrixXd hess = Eigen::MatrixXd::Zero(n, n);
  // Creating the lagrange polynomial.
  double c = Xi(0, t - 1);
  grad = (Xi.col(t - 1)).tail(n);
  for (int k = 1; k <= m; ++k) {
    double tmp = Z.row(k - 1) * S * (Z.row(t - 1)).transpose();
    hess += tmp * (Y.col(k - 1)) * (Y.col(k - 1)).transpose();
  }

  Eigen::VectorXd yTry(n); //Displacement from current center point
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_real_distribution<> dis(-rho, rho);
  int k = 0;

  for (int i = 0; i < n; ++i) {
    yTry(i) = dis(gen)+bestPoint[i];
  }
  Eigen::VectorXd yBest = yTry;

  double maxValue = c + grad.transpose() * yTry + 0.5 * yTry.transpose() * hess * yTry;
  maxValue = std::abs(maxValue);
  double value = 0;

  while (k < 5000) {
    for (int i = 0; i < n; ++i) {
      yTry(i) = dis(gen)+bestPoint[i];
    }
    value = c + grad.transpose() * yTry + 0.5 * yTry.transpose() * hess * yTry;
    if (std::abs(value) > maxValue) {
      maxValue = std::abs(value);
      yBest = yTry;
    }
    ++k;
  }
  return yBest;
}

int DFO_Model::findPointToReplaceWithNewOptimum(Eigen::VectorXd yNew) {

  // Create the w vector
  Eigen::VectorXd w(n + m + 1);
  for (int i = 1; i <= m; ++i) {
    w(i - 1) = 0.5 * std::pow((Y.col(i - 1)).transpose() * (yNew), 2);
  }
  //w.tail(n) = yNew;
  eigen_tail(w, yNew, n);
  w(m) = 1;

  Eigen::VectorXd Hw = Eigen::VectorXd::Zero(m + n + 1);

  //Hw.head(m) = Z * S * (Z.transpose()) * (w.head(m)) + Xi.transpose() * (w.tail(n + 1));
  eigen_head(Hw, Z * S * (Z.transpose()) * (w.head(m)) + Xi.transpose() * (w.tail(n + 1)), m);
  //Hw.tail(n + 1) = Xi * w.head(m) + Upsilon * w.tail(n + 1);
  eigen_tail(Hw, Xi * w.head(m) + Upsilon * w.tail(n + 1), n + 1);

  int indexToBeReplaced = 1;
  double currentMax = -1;
  std::cout << "Selecting point to be replaced by new optimum " << std::endl;
  std::cout << "New optimum \n" << yNew << "\n";
  std::cout << "BestPoint Index " << bestPointIndex << "\n";
  std::cout << "Y\n" << Y << "\n";
  for (int i = 1; i <= m; ++i) {
    if (i == bestPointIndex) {
      //continue;
    }
    double distance = norm((bestPoint - Y.col(i - 1)));
    //double distanceWeight = std::pow(distance, 2);
    double distanceWeight = distance;
    //if (distance > 2*rho ){
    //  distanceWeight += 100000000*distanceWeight;
      /*
      if (distanceWeight < 1){
        distanceWeight += 10*distanceWeight;
      }
      else{
        distanceWeight = std::pow(distance, 4);
      }
       */
    //}

    double lagval = std::abs((Hw)(i - 1));
    double value = distanceWeight * lagval;
    std::cout << "distance: " << distance << "\t distanceweight: " << distanceWeight << "\t lagval: " << lagval <<"\t " << "value: " << value << "\n";
    if (value >= currentMax) {
      indexToBeReplaced = i;
      currentMax = value;
    }
  }

  std::cout << "Point selected: "<< indexToBeReplaced << "\n";
  /*
  for (int i = 0; i < m; i++){
    std::cout << "Lagrange polynomial : "<< i << "\n";
    ComputeLagrangePolynomial(i+1, yNew);
  }
  */
  return indexToBeReplaced;
}

int DFO_Model::getBestPointIndex() {
  return bestPointIndex;
}

void DFO_Model::printParametersMatlabFriendly() {
  Eigen::MatrixXd Hessian(n, n);

  Hessian = Gamma;

  for (int i = 1; i <= m; ++i) {
    Hessian += gammas(i - 1) * Y.col(i - 1) * (Y.col(i - 1)).transpose();
  }

  std::cout << "_______________________________" << std::endl;
  std::cout << "_____Matlab friendly print_____" << std::endl;
  std::cout << "_______________________________" << std::endl;

  std::cout << "H = [ " << std::endl;
  for (int i = 0; i < n; ++i) {
    std::cout << Hessian.row(i);
    if (i != n - 1) {
      std::cout << "; \n";
    }
  }
  std::cout << " \n ];" << std::endl;

  std::cout << "g = [" << std::endl;
  std::cout << gradient << std::endl;
  std::cout << "]';" << std::endl;
  std::cout << "c = " << constant << ";" << std::endl;
  std::cout << "y0 = [" << std::endl;
  std::cout << y0 << std::endl;
  std::cout << "]';" << std::endl;
  std::cout << "rho = " << rho << ";" << std::endl;
}

void DFO_Model::printQuadraticModel() {

  Eigen::MatrixXd hess(n, n);

  hess = Gamma;

  for (int i = 1; i <= m; ++i) {
    hess += gammas(i - 1) * Y.col(i - 1) * (Y.col(i - 1)).transpose();
  }

  //std::cout << "---------------- Extracted form the object----" << std::endl;
  std::cout << "c = " << constant << std::endl;
  std::cout << "gradient = " << std::endl << gradient << std::endl;
  std::cout << "hessian = " << std::endl << hess << std::endl;
}

void DFO_Model::printSlowShiftCenterPointOfQuadraticModel(Eigen::VectorXd s) {
  Eigen::VectorXd r(n);
  Eigen::MatrixXd P = Eigen::MatrixXd::Zero(n, m);

  double squaredNorm = s.squaredNorm();
  for (int k = 1; k <= m; ++k) {
    r = Y.col(k - 1) - 0.5 * s;
    //P.col(k - 1) = (s.transpose() * r) * r + 0.25 * squaredNorm * s;
    Eigen::VectorXd tmp1 = (s.transpose() * r) * r;
    Eigen::VectorXd tmp2 = 0.25 * squaredNorm * s;
    //Eigen::VectorXd tmp = tmp1 + tmp2;
    eigen_col(P, tmp1 + tmp2, k - 1);
  }

  Eigen::MatrixXd Omega(m, m);
  Omega = Z * S * Z.transpose();
  Eigen::MatrixXd H(m + n + 1, m + n + 1);
  H.topLeftCorner(m, m) = Omega;
  H.bottomLeftCorner(n + 1, m) = Xi;
  H.topRightCorner(m, n + 1) = Xi.transpose();
  H.bottomRightCorner(n + 1, n + 1) = Upsilon;

  Eigen::MatrixXd invOmegaXtranspose = Eigen::MatrixXd::Zero(m + n + 1, m + n + 1);
  Eigen::MatrixXd invOmegaATranspose = Eigen::MatrixXd::Zero(m + n + 1, m + n + 1);
  invOmegaXtranspose.setIdentity();
  invOmegaATranspose.setIdentity();

  //(invOmegaXtranspose.row(m)).tail(n) = 0.5 * s.transpose();
  eigen_block(invOmegaXtranspose, 0.5 * s.transpose(), m, m + n + 1 - n);

  invOmegaATranspose.bottomLeftCorner(n, m) = P;

  H = invOmegaXtranspose * invOmegaATranspose * invOmegaXtranspose * H * invOmegaXtranspose.transpose()
      * invOmegaATranspose.transpose() * invOmegaXtranspose.transpose();

  std::cout << "H after shift. Calculated by pure matrix multiplications \n" << H << std::endl;
}

void DFO_Model::compareHMatrices() {
  double precision = 0.00001;
  Eigen::MatrixXd W = Eigen::MatrixXd::Zero(m + n + 1, m + n + 1);
  Eigen::MatrixXd A = Eigen::MatrixXd::Zero(m, m);
  Eigen::MatrixXd X = Eigen::MatrixXd::Zero(n + 1, m);
  for (int i = 1; i <= m; ++i) {
    for (int j = 1; j <= m; ++j) {
      A(i - 1, j - 1) = 0.5 * std::pow(Y.col(i - 1).transpose() * Y.col(j - 1), 2);
    }
  }

  X.row(0).setOnes();
  for (int i = 1; i <= m; ++i) {
    //(X.col(i - 1)).tail(n) = Y.col(i - 1);
    eigen_block(X, Y.col(i - 1), n + 1 - n, i - 1);
  }

  W.topLeftCorner(m, m) = A;
  W.bottomLeftCorner(n + 1, m) = X;
  W.topRightCorner(m, n + 1) = X.transpose();

  Eigen::MatrixXd Winv(m + n + 1, m + n + 1);
  Winv = W.inverse();
  Eigen::MatrixXd Omega(m, m);
  Omega = Z * S * Z.transpose();

  Eigen::MatrixXd H(m + n + 1, m + n + 1);
  H.topLeftCorner(m, m) = Omega;
  H.bottomLeftCorner(n + 1, m) = Xi;
  H.topRightCorner(m, n + 1) = Xi.transpose();
  H.bottomRightCorner(n + 1, n + 1) = Upsilon;

  std::cout << "H " << std::endl << H << std::endl;
  std::cout << " is Omega - invWOmega == 0 ?  " << std::endl << Omega.isApprox(Winv.topLeftCorner(m, m), precision)
            << std::endl;
  std::cout << " is Xi - invWXi == 0 ?  " << std::endl << Xi.isApprox(Winv.bottomLeftCorner(n + 1, m), precision)
            << std::endl;
  std::cout << " is Upsilon - invWUpsilon == 0 ?  " << std::endl
            << Upsilon.isApprox(Winv.bottomRightCorner(n + 1, n + 1), precision) << std::endl;
  std::cout << " is H - invW == 0 ?  " << std::endl << H.isApprox(Winv, precision) << std::endl;

  std::cout << "Winverse:" << std::endl << Winv << std::endl << std::endl;
  std::cout << "H:" << std::endl << H << std::endl << std::endl;
}
void DFO_Model::SetFunctionValue(int t, double value) {
  fvals[t - 1] = value;
}

void DFO_Model::SetFunctionValueAndDerivatives(int t, Eigen::VectorXd values) {
  fvals[t - 1] = values[0];
  for (int i = 1; i < values.rows(); ++i){
    derivatives(i-1,t-1) = values(i);
  }
}

void DFO_Model::SetTrustRegionRadiusForSubproblem(double radius) {
  subproblem.SetTrustRegionRadius(radius);
}
Eigen::VectorXd DFO_Model::FindLocalOptimum() {
  Eigen::MatrixXd Hessian(n, n);
  Hessian = Gamma;
  for (int i = 1; i <= m; ++i) {
    Hessian += gammas(i - 1) * Y.col(i - 1) * (Y.col(i - 1)).transpose();
  }
  subproblem.setHessian(Hessian);
  subproblem.setGradient(gradient);
  subproblem.setConstant(constant);

  Eigen::VectorXd localOptimum(n);
  vector<double> xsol;
  vector<double> fsol;
  //printf("\x1b[33mLooking for new optimum: \n\x1b[0m");
  //streambuf *old = cout.rdbuf(0);
  //cout << "Hidden text!\n";

  subproblem.Solve(xsol, fsol, (char *) "Minimize", y0, bestPoint);
  for (int i = 0; i < n; i++) {
    localOptimum[i] = xsol[i];
  }
  //cout.rdbuf(old);
  //std::cout << fsol[0]<<"\n";
  //std::cout << "new point " <<xsol[0] + y0(0) << "\t" << xsol[1]+ y0(1) <<"\n";
  //std::cout << "best point " << bestPoint[0] + y0(0) << "\t" << bestPoint[1] + y0(1) <<"\n";

  //subproblem.printModel();
  return localOptimum;
}

int DFO_Model::findPointFarthestAwayFromOptimum() {
  int t = -1;
  double maxDistance = -1;
  for (int i = 0; i < m; ++i) {
    //double dist = (Y.col(i) - bestPoint).norm();
    double dist = norm(Y.col(i) - bestPoint);
    if (dist > maxDistance) {
      t = i + 1;
      maxDistance = dist;
    }
  }
  return t;
}

double DFO_Model::findLargestDistanceBetweenPointsAndOptimum() {
  int t = -1;
  double maxDistance = -1;
  for (int i = 0; i < m; ++i) {
    //double dist = (Y.col(i) - bestPoint).norm();
    double dist = norm(Y.col(i) - bestPoint);

    if (dist > maxDistance) {
      t = i + 1;
      maxDistance = dist;
    }
  }
  return maxDistance;
}
double DFO_Model::ComputeLagrangePolynomial(int t, Eigen::VectorXd point) {
  Eigen::VectorXd grad;
  Eigen::MatrixXd hess = Eigen::MatrixXd::Zero(n, n);
  hess.setZero();
  // Creating the Lagrange polynomial.
  double c = Xi(0, t - 1);
  grad = (Xi.col(t - 1)).tail(n);
  for (int k = 1; k <= m; ++k) {
    double tmp = Z.row(k - 1) * S * (Z.row(t - 1)).transpose();
    hess += tmp * (Y.col(k - 1)) * (Y.col(k - 1)).transpose();
  }

  /*
  std::cout << "c = " << c << std::endl;
  std::cout << "gradient = " << std::endl << grad << std::endl;
  std::cout << "hessian = " << std::endl << hess << std::endl;
  std::cout << "value is: " << c + grad.transpose() * point + point.transpose() * hess * point << "\n";
  */
   double val = c + grad.transpose() * point + 0.5*point.transpose() * hess * point;

  return val;
}

double DFO_Model::PrintLagrangePolynomial(int t) {
  Eigen::VectorXd grad;
  Eigen::MatrixXd hess = Eigen::MatrixXd::Zero(n, n);
  // Creating the Lagrange polynomial.
  double c = Xi(0, t - 1);
  grad = (Xi.col(t - 1)).tail(n);
  for (int k = 1; k <= m; ++k) {
    double tmp = Z.row(k - 1) * S * (Z.row(t - 1)).transpose();
    hess += tmp * (Y.col(k - 1)) * (Y.col(k - 1)).transpose();
  }
  std::cout << "Lagrange polynomial ----------------------- " << t << "\n";
  std::cout << "c = " << c << std::endl;
  std::cout << "gradient = " << std::endl << grad << std::endl;
  std::cout << "hessian = " << std::endl << hess << std::endl;
}

Eigen::VectorXd DFO_Model::FindLocalOptimumOfAbsoluteLagrangePolynomial(int t) {
  Eigen::VectorXd grad;
  Eigen::MatrixXd hess = Eigen::MatrixXd::Zero(n, n);
  // Creating the lagrange polynomial.
  hess.setZero();
  double c = Xi(0, t - 1);
  grad = (Xi.col(t - 1)).tail(n);
  for (int k = 1; k <= m; ++k) {
    double tmp = Z.row(k - 1) * S * (Z.row(t - 1)).transpose();
    hess += tmp * (Y.col(k - 1)) * (Y.col(k - 1)).transpose();
  }

  // Find min and max of l_t(x)
  subproblem.setConstant(c);
  subproblem.setGradient(grad);
  subproblem.setHessian(hess);
  vector<double> xsolMax;
  vector<double> fsolMax;
  vector<double> xsolMin;
  vector<double> fsolMin;
  subproblem.Solve(xsolMax, fsolMax, (char *) "Maximize", y0, bestPoint, bestPoint);
  subproblem.Solve(xsolMin, fsolMin, (char *) "Minimize", y0, bestPoint, bestPoint);


  Eigen::VectorXd optimum(n);
//double maxVal = 0;
  for (int i = 0; i < xsolMax.size(); ++i) {
    if (abs(fsolMax[0]) >= abs(fsolMin[0])) {
      optimum[i] = xsolMax[i];
      //maxVal = abs(fsolMax[0]);
    } else {
      optimum[i] = xsolMin[i];
      //maxVal = abs(fsolMin[0]);
    }
  }
  /*
  Eigen::VectorXd startingPoint = Eigen::VectorXd::Zero(n);
  subproblem.Solve(xsolMax, fsolMax, (char *) "Maximize", y0, bestPoint, startingPoint);
  subproblem.Solve(xsolMin, fsolMin, (char *) "Minimize", y0, bestPoint, startingPoint);
  for (int i = 0; i < xsolMax.size(); ++i) {
    if (abs(fsolMax[0]) >= abs(fsolMin[0]) && abs(fsolMax[0]) > maxVal) {
      optimum[i] = xsolMax[i];
    } else if (abs(fsolMin[0]) >= abs(fsolMax[0]) && abs(fsolMin[0]) > maxVal){
      optimum[i] = xsolMin[i];
    }
  }
   */
  return optimum;
}

Eigen::VectorXi DFO_Model::GetInterpolationPointsSortedByDistanceFromBestPoint() {

  std::vector<Eigen::VectorXd> tmp;
  for (int i = 0; i < m; ++i) {
    Eigen::VectorXd t(n + 1);
    for (int j = 0; j < n; ++j) {
      t(j) = Y(j, i)-Y(j,bestPointIndex-1);
    }
    t(n) = i + 1;
    tmp.push_back(t);
  }
  std::sort(tmp.begin(), tmp.end(), cmp);

  Eigen::VectorXi indicesSortedByDescendingNorm(m);
  for (int i = 0; i < m; ++i) {
    indicesSortedByDescendingNorm[i] = tmp[i][n];
  }
  return indicesSortedByDescendingNorm;
}

bool DFO_Model::FindPointToReplaceWithPointOutsideScaledTrustRegion(int t, Eigen::VectorXd &dNew) {
  subproblem.SetTrustRegionRadius(rho);
  dNew = FindLocalOptimumOfAbsoluteLagrangePolynomial(t);
  if (std::abs(ComputeLagrangePolynomial(t, dNew)) > lambda) {
    return true;
  }
  return false;
}

bool DFO_Model::FindPointToIncreasePoisedness(Eigen::VectorXd &dNew, int &t) {

  // If point found is optimum; remove point that is farthest away from bestpoint, with?

  findWorstPointInInterpolationSet(dNew, t);
  if (t == -1){
    return false;
  }
  else if (t == bestPointIndex){
    return false;
    /*
    Eigen::VectorXd poisednessvals(m);
    double worstPoisedness = 0;

    int index = -1;

    Eigen::VectorXd grad;
    Eigen::MatrixXd hess = Eigen::MatrixXd::Zero(n, n);
    // Creating the lagrange polynomial.
    for (int t = 1; t <= m; ++t) {
      hess.setZero();
      double c = Xi(0, t - 1);
      grad = (Xi.col(t - 1)).tail(n);
      for (int k = 1; k <= m; ++k) {
        double tmp = Z.row(k - 1) * S * (Z.row(t - 1)).transpose();
        hess += tmp * (Y.col(k - 1)) * (Y.col(k - 1)).transpose();
        //hess += Z.row(k - 1) * S * (Z.row(t - 1)).transpose() * (Y.col(k - 1)) * (Y.col(k - 1)).transpose();
      }

      // Find min and max of l_t(x)
      subproblem.setConstant(c);
      subproblem.setGradient(grad);
      subproblem.setHessian(hess);
      vector<double> xsolMax;
      vector<double> fsolMax;
      vector<double> xsolMin;
      vector<double> fsolMin;
      subproblem.Solve(xsolMax, fsolMax, (char *) "Maximize", y0, bestPoint);
      subproblem.Solve(xsolMin, fsolMin, (char *) "Minimize", y0, bestPoint);

      double temp = 0;
      poisednessvals[t-1] = (abs(fsolMax[0]) >= abs(fsolMin[0])) ? abs(fsolMax[0]) : abs(fsolMin[0]);
      if ((abs(fsolMax[0]) >= abs(fsolMin[0])) && abs(fsolMax[0]) >= worstPoisedness) {
        worstPoisedness = abs(fsolMax[0]);
        for (int i = 0; i < xsolMax.size(); ++i) {
          dNew[i] = xsolMax[i];
        }
        index = t;
      } else if ((abs(fsolMin[0]) > abs(fsolMax[0])) && abs(fsolMin[0]) >= worstPoisedness) {
        worstPoisedness = abs(fsolMin[0]);
        for (int i = 0; i < xsolMin.size(); ++i) {
          dNew[i] = xsolMin[i];
        }
        index = t;
      }

    }


    std::cout << "Y\n" << Y << std::endl;
    std::cout << "index of best point: "<< bestPointIndex << "\nvalues of abs lagrange \n" << poisednessvals << std::endl;
    int newi = 1;
    double sdf = -100;
    for (int i = 1; i <= m; i++){
      if (poisednessvals[i-1] >= sdf && i != bestPointIndex){
        newi = i;
        sdf = poisednessvals[i-1];
      }
    }
    t = newi;

    hess.setZero();
    double c = Xi(0, t - 1);
    grad = (Xi.col(t - 1)).tail(n);
    for (int k = 1; k <= m; ++k) {
      double tmp = Z.row(k - 1) * S * (Z.row(t - 1)).transpose();
      hess += tmp * (Y.col(k - 1)) * (Y.col(k - 1)).transpose();
      //hess += Z.row(k - 1) * S * (Z.row(t - 1)).transpose() * (Y.col(k - 1)) * (Y.col(k - 1)).transpose();
    }

    // Find min and max of l_t(x)
    subproblem.setConstant(c);
    subproblem.setGradient(grad);
    subproblem.setHessian(hess);
    vector<double> xsolMax;
    vector<double> fsolMax;
    vector<double> xsolMin;
    vector<double> fsolMin;
    subproblem.SetTrustRegionRadius(GetTrustRegionRadius());
    subproblem.Solve(xsolMax, fsolMax, (char *) "Maximize", y0, bestPoint);
    subproblem.Solve(xsolMin, fsolMin, (char *) "Minimize", y0, bestPoint);

    double temp = 0;
    if ((abs(fsolMax[0]) >= abs(fsolMin[0]))) {
      for (int i = 0; i < xsolMax.size(); ++i) {
        dNew[i] = xsolMax[i];
      }
    } else {
      for (int i = 0; i < xsolMin.size(); ++i) {
        dNew[i] = xsolMin[i];
      }
    }

*/

    /*
    t = findPointFarthestAwayFromOptimum();

    Eigen::VectorXd grad;
    Eigen::MatrixXd hess = Eigen::MatrixXd::Zero(n, n);
    // Creating the lagrange polynomial.

    double c = Xi(0, t - 1);
    grad = (Xi.col(t - 1)).tail(n);
    for (int k = 1; k <= m; ++k) {
      double tmp = Z.row(k - 1) * S * (Z.row(t - 1)).transpose();
      hess += tmp * (Y.col(k - 1)) * (Y.col(k - 1)).transpose();
      //hess += Z.row(k - 1) * S * (Z.row(t - 1)).transpose() * (Y.col(k - 1)) * (Y.col(k - 1)).transpose();
    }

    // Find min and max of l_t(x)
    subproblem.setConstant(c);
    subproblem.setGradient(grad);
    subproblem.setHessian(hess);
    vector<double> xsolMax;
    vector<double> fsolMax;
    vector<double> xsolMin;
    vector<double> fsolMin;
    subproblem.SetTrustRegionRadius(GetTrustRegionRadius());
    subproblem.Solve(xsolMax, fsolMax, (char *) "Maximize", y0, bestPoint);
    subproblem.Solve(xsolMin, fsolMin, (char *) "Minimize", y0, bestPoint);
    if ((abs(fsolMax[0]) >= abs(fsolMin[0]))) {
      for (int i = 0; i < xsolMax.size(); ++i) {
        dNew[i] = xsolMax[i];
      }
    } else if ((abs(fsolMin[0]) > abs(fsolMax[0]))) {
      for (int i = 0; i < xsolMin.size(); ++i) {
        dNew[i] = xsolMin[i];
      }
    }
    std::cout << "poisedness is max of  " << fsolMax[0] << " and " << fsolMin[0] << std::endl;
    std::cout << "Y\n" << Y << std::endl;
    std::cout << "Old point (" << t << ")\n" << Y.col(t-1)<< "\n";
    std::cout << "New point \n" << dNew << std::endl;
*/
  }
  return false;
}

void DFO_Model::printParametersMatlabFriendlyGradientEnhanced() {
  std::cout << "y0 in dfo model \n" << y0 << "\n";
  enhancedModel.ComputeModel(Y, derivatives, derivatives.col(0), fvals, y0, bestPoint, rho, r,0);
  enhancedModel.PrintParametersMatlabFriendly();
  enhancedModel.ComputeModel2(Y, derivatives, derivatives.col(0), fvals, y0, bestPoint, rho, r,0);
  enhancedModel.PrintParametersMatlabFriendly();
}
int DFO_Model::isPointAcceptable(Eigen::VectorXd point) {
  // Create the w vector
  Eigen::VectorXd w(n + m + 1);
  for (int i = 1; i <= m; ++i) {
    w(i - 1) = 0.5 * std::pow((Y.col(i - 1)).transpose() * (point), 2);
  }
  //w.tail(n) = yNew;
  eigen_tail(w, point, n);
  w(m) = 1;

  Eigen::VectorXd Hw = Eigen::VectorXd::Zero(m + n + 1);

  //Hw.head(m) = Z * S * (Z.transpose()) * (w.head(m)) + Xi.transpose() * (w.tail(n + 1));
  eigen_head(Hw, Z * S * (Z.transpose()) * (w.head(m)) + Xi.transpose() * (w.tail(n + 1)), m);
  //Hw.tail(n + 1) = Xi * w.head(m) + Upsilon * w.tail(n + 1);
  eigen_tail(Hw, Xi * w.head(m) + Upsilon * w.tail(n + 1), n + 1);

  int indexToBeReplaced = -1;
  double currentMax = -1;
  for (int j = 1; j <= m; ++j){
    if (j == bestPointIndex) {
      continue;
    }
    double lagval = std::abs((Hw)(j - 1));
    if ( (lagval > 1) || (norm(Y.col(j - 1)-bestPoint) > 2*rho)){
      // do the maximization thing.
      double distance = norm((bestPoint - Y.col(j - 1)));
      double distanceWeight = std::pow(distance, 2);
      if (distance > 2*rho ){
        distanceWeight += 100000000*distanceWeight;
      }
        double value = distanceWeight * lagval;
        //std::cout << "distance: " << distance << "\t distanceweight: " << distanceWeight << "\t lagval: " << lagval <<"\t " << "value: " << value << "\n";
      if (value >= currentMax) {
          indexToBeReplaced = j;
          currentMax = value;
        }
      }
    }
  if (indexToBeReplaced == bestPointIndex) {
    std::cout << "Possibly swapping best point for new best point \n";
    //std::cin.get();
  }
  return indexToBeReplaced;
}

double DFO_Model::DistanceFromOptimum(Eigen::VectorXd point){


}
double DFO_Model::norm(Eigen::VectorXd a) {
  // infinity = 0;
  // l2 = 2
  if (normType == 0){
    return (a).lpNorm<Infinity>();
  }
  else if (normType == 2){
    return (a).norm();
  }
}
bool DFO_Model::FindReplacementForPointsOutsideRadius(double radius, Eigen::MatrixXd &newPoints, Eigen::VectorXi& newIndices) {
  Eigen::DiagonalMatrix<double, Eigen::Dynamic> copyS = S;
  Eigen::MatrixXd copyZ = Z;
  Eigen::MatrixXd copyUpsilon = Upsilon;
  Eigen::MatrixXd copyXi = Xi;
  Eigen::MatrixXd copyY = Y;
  bool retVal = true;


  /// Find points outside r*radius
  Eigen::VectorXi sortedPoints = GetInterpolationPointsSortedByDistanceFromBestPoint();
  int number_of_points_outside = GetNumberOfPointsOutsideRadius(radius);
  sortedPoints.conservativeResize(number_of_points_outside);

  if (number_of_points_outside <= 0){
    return false;
  }


  //newIndices = Eigen::VectorXi(sortedPoints.rows());
  newIndices.resize(number_of_points_outside);
  for (int i = 0; i < number_of_points_outside; ++i){
    newIndices[i] = -1;
  }

  subproblem.SetTrustRegionRadius(radius/r);
  newPoints.resize(n, number_of_points_outside);
  newPoints.setZero();
  Eigen::VectorXd dNew(n);
  int addedPoints = 0;
  int j = 0;
  for (int i = 0; i < number_of_points_outside; ++i){
    dNew = FindLocalOptimumOfAbsoluteLagrangePolynomial(sortedPoints[i]);
    //dNew = findHighValueOfAbsoluteLagrangePolynomial(sortedPoints[i]);
    double lagabsval = std::abs(ComputeLagrangePolynomial(sortedPoints[i], dNew));
    std::cout << "lagabsval = " << lagabsval << "\n";
    if (lagabsval > 0.001) {
      newIndices[addedPoints] = sortedPoints[i];
      newPoints.col(addedPoints) = dNew;
      eigen_col(newPoints, dNew, addedPoints);
      //do the update
      updateInverseKKTMatrix(dNew,sortedPoints[i]);
      eigen_col(Y, dNew, sortedPoints[i] - 1);
      addedPoints++;
      j++;
      //break;
    }
    else{
      std::cout << "best point (init point): \n" << bestPoint << "\n";
      std::cout << "point found: \n" << dNew <<"\n";
      std::cout << "Lag pol that cannot be maximized\n";
      PrintLagrangePolynomial(sortedPoints[i]);
      //newIndices.conservativeResize(addedPoints);
      //newPoints.conservativeResize(n, addedPoints);
      if (newIndices.rows() == 0 || newIndices.rows() == addedPoints ){
        break;
      }
      //break;
      }
    }

  if (addedPoints != newIndices.rows()){
    newIndices.conservativeResize(addedPoints);
    newPoints.conservativeResize(n, addedPoints);
  }
  if (addedPoints == 0){
    retVal = false;
  }
  /*
  /// Find replacement points
  int addedPoints = 0;
  int j = 0;

  for (int i = 0; i < sortedPoints.rows(); ++i){
    dNew = FindLocalOptimumOfAbsoluteLagrangePolynomial(sortedPoints[i]);
    std::cout << "Lagval = " << ComputeLagrangePolynomial(sortedPoints[i], dNew) << "\n";
    if (std::abs(ComputeLagrangePolynomial(sortedPoints[i], dNew)) > 1) {
      newIndices[addedPoints] = sortedPoints[i];
      newPoints.col(addedPoints) = dNew;
      //do the update
      updateInverseKKTMatrix(dNew,sortedPoints[i]);
      eigen_col(Y, dNew, sortedPoints[i] - 1);
      addedPoints++;
      j++;
    }
  }
  //subproblem.SetTrustRegionRadius(radius);
  //std::cout << "Best point index = " << bestPointIndex << "\n";
   */
  /*
  for (int i = 1; i <= m; ++i){
    dNew = FindLocalOptimumOfAbsoluteLagrangePolynomial(i);
    std::cout << "Lagval = " << ComputeLagrangePolynomial(i, dNew) << "\n";

  }
   */
/*
  if (j == 0){
    std::cout << "No points has |l(x)| > 1 :O :O :O\n";
    int t = sortedPoints[0];
    int absmax = 0;

    // infinity norm...
    for (int i = 1; i <= n; ++i){
      if (std::abs(Y(i-1,t-1)) > absmax){
        absmax = std::abs(Y(i-1,t-1));
      }
    }
    if (absmax > radius*0.5){
      dNew = dNew*(radius*0.5/absmax);
      newIndices[0] = t;
      newPoints.col(0) = dNew;
      //do the update
      updateInverseKKTMatrix(dNew,t);
    }
    if (sortedPoints.rows() > 1 ) {
      t = sortedPoints[1];
      absmax = 0;

      // infinity norm...
      for (int i = 1; i <= n; ++i) {
        if (std::abs(Y(i - 1, t - 1)) > absmax) {
          absmax = std::abs(Y(i - 1, t - 1));
        }
      }
      if (absmax > radius * 0.5) {
        dNew = dNew * (radius * 0.5 / absmax);
        newIndices[1] = t;
        newPoints.col(1) = dNew;
        //do the update
        updateInverseKKTMatrix(dNew, t);
      }
      //std::cin.get();
    }
  }
  */
  /*
  while(j < sortedPoints.rows()){
    dNew = FindLocalOptimumOfAbsoluteLagrangePolynomial(sortedPoints[j]);
    if (std::abs(ComputeLagrangePolynomial(sortedPoints[j], dNew)) > 1) {
      newIndices[addedPoints] = sortedPoints[j];
      newPoints.col(addedPoints) = dNew;
      //do the update
      updateInverseKKTMatrix(dNew,sortedPoints[j]);
      eigen_col(Y, dNew, sortedPoints[j] - 1);
      addedPoints++;
      j++;
    }
    else{
      //Swap order of 2 points and hope for the best...
      if (addedPoints < sortedPoints.rows()){
        int tmp = sortedPoints[addedPoints+1];
        sortedPoints[addedPoints+1] = sortedPoints[addedPoints];
        sortedPoints[addedPoints] = tmp;
      } else{
        std::cout << "not enough points for swapping.\n";
        std::cin.get();
      }
    }
  }
   */


  /// reset!!
  Y = copyY;
  Xi = copyXi;
  Upsilon = copyUpsilon;
  S = copyS;
  Z = copyZ;
  return retVal;
}
int DFO_Model::GetNumberOfPointsOutsideRadius(double radius) {
  /// Number of points outside radius
  int number = 0;
  //std::cout << "radius " << radius << "\n";
  for (int j = 1; j <= m; ++j){
    //std::cout << "Point minus bestpoint: \n" << Y.col(j-1) - bestPoint<< "\nNorm: \t " << norm(Y.col(j-1) - bestPoint) << std::endl;
    if (norm(Y.col(j-1) - bestPoint) > radius){
      number++;
    }
  }
  return number;
}


void DFO_Model::calculatePolynomialModelDirectlyFromWinverse() {
  Eigen::MatrixXd Winv(m + n + 1, m + n + 1);
  Winv = calculateWExplicitly();

  Eigen::VectorXd rhs = Eigen::VectorXd::Zero(m + n + 1);
  //rhs.head(m) = fvals;
  eigen_head(rhs,fvals,m);

  Eigen::VectorXd ans(m + n + 1);

  ans = Winv*rhs;

  Eigen::VectorXd grad(n);
  Eigen::MatrixXd hess = Eigen::MatrixXd::Zero(n, n);
  double c;

  grad = ans.tail(n);
  c = ans(m);

  for (int i = 1; i <= m; ++i) {
    hess += ans(i - 1)*(Y.col(i - 1) * (Y.col(i - 1)).transpose());
  }

  std::cout << "---------------- Calculated from Winv ----" << std::endl;
  std::cout << "c = " << c << std::endl;
  std::cout << "gradient = " << std::endl << grad << std::endl;
  std::cout << "hessian = " << std::endl << hess << std::endl;

}

// This is slow and should only be used for testing and debugging.
Eigen::MatrixXd DFO_Model::calculateWExplicitly() {
  double precision = 0.00001;
  //[2] page 9
  Eigen::MatrixXd W = Eigen::MatrixXd::Zero(m + n + 1, m + n + 1);
  Eigen::MatrixXd A = Eigen::MatrixXd::Zero(m, m);
  //A.setZero();
  Eigen::MatrixXd X = Eigen::MatrixXd::Zero(n + 1, m);
  //std::cout << "Y: " << std::endl << Y <<std::endl;
  for (int i = 1; i <= m; ++i) {
    for (int j = 1; j <= m; ++j) {
      //A(i - 1, j - 1) = 0.5*std::pow(Y.col(i - 1).dot(Y.col(j - 1)), 2);
      A(i - 1, j - 1) = 0.5*std::pow(Y.col(i-1).transpose()*Y.col(j-1), 2);
    }
  }

  //X.row(0).setOnes();
  //eigen_row(X,Eigen::VectorXd::Ones(1,m),0);

  for (int i = 1; i <= m; ++i){
    X(0,i-1) = 1;
  }

  for (int i = 1; i <= m; ++i) {
    //(X.col(i - 1)).tail(n) = Y.col(i - 1);
    Eigen::VectorXd tmp = X.col(i - 1);
    eigen_tail(tmp,Y.col(i-1),n);
    eigen_col(X, tmp, i-1);
  }

  //std::cout << "" << std::endl <<  <<std::endl;
  //std::cout << "A: " << std::endl << A <<std::endl;
  //std::cout << "X: " << std::endl << X << std::endl;
  //std::cout << "A: " << std::endl << W.topLeftCorner(m, m) << std::endl;
  //std::cout << "W: " << std::endl << W <<std::endl;
  W.topLeftCorner(m, m) = A;
  W.bottomLeftCorner(n + 1, m) = X;
  W.topRightCorner(m, n + 1) = X.transpose();
  //std::cout << "W: " << std::endl << W <<std::endl;


  //std::cout << "W: " << std::endl << W << std::endl;

  Eigen::MatrixXd Winv(m + n + 1, m + n + 1);
  Winv = W.inverse();
  Eigen::FullPivLU<Eigen::MatrixXd> lu_decompW(W);;
  Eigen::MatrixXd WLUinv(m + n + 1, m + n + 1);
  WLUinv = lu_decompW.inverse();
  //std::cout << "Winv: " << std::endl << Winv <<std::endl;

  //std::cout << "Winv lu: " << std::endl << WLUinv <<std::endl;
  //std::cout << "done: " << std::endl;




  //std::cout << "Omega from inverse of W: " << std::endl << Winv.topLeftCorner(m, m) << std::endl;

/*
  Eigen::MatrixXd Omega(m, m);
  Omega = Z*S*Z.transpose();
*/
  //std::cout << "Omega" << std::endl << Omega << std::endl;
  //std::cout << "Xi" << std::endl << Xi << std::endl;
  //std::cout << "Upsilon" << std::endl << Upsilon << std::endl;
/*
  Eigen::MatrixXd H(m + n + 1, m + n + 1);
  H.topLeftCorner(m, m) = Omega;
  H.bottomLeftCorner(n + 1, m) = Xi;
  H.topRightCorner(m, n + 1) = Xi.transpose();
  H.bottomRightCorner(n + 1, n + 1) = Upsilon;
*/
/*
  std::cout << "H " << std::endl << H << std::endl;
  std::cout << " is Omega - invWOmega == 0 ?  " << std::endl << Omega.isApprox(Winv.topLeftCorner(m,m), precision) << std::endl;
  std::cout << " is Xi - invWXi == 0 ?  " << std::endl <<          Xi.isApprox(Winv.bottomLeftCorner(n+1,m), precision) << std::endl;
  std::cout << " is Upsilon - invWUpsilon == 0 ?  " << std::endl << Upsilon.isApprox(Winv.bottomRightCorner(n+1,n+1), precision) << std::endl;
  std::cout << " is H - invW == 0 ?  " << std::endl << H.isApprox(Winv, precision) << std::endl;

  std::cout << "invWUpsilon" << std::endl << Winv.bottomRightCorner(n + 1, n + 1) << std::endl;
  std::cout << "Upsilon" << std::endl << Upsilon << std::endl;
  std::cout << "invWOmega" << std::endl << Winv.topLeftCorner(m,m) << std::endl;
  std::cout << "Omega" << std::endl << Omega << std::endl;
  std::cout << "invWXi" << std::endl << Winv.bottomLeftCorner(n+1,m) << std::endl;
  std::cout << "Xi" << std::endl << Xi << std::endl;

  Eigen::MatrixXd Hinv(m + n + 1, m + n + 1);
  Hinv = H.inverse();

  std::cout << "Winverse:" << std::endl << Winv << std::endl << std::endl;

  std::cout << "H:" << std::endl << H << std::endl << std::endl;
*/
  /*
  std::cout << "W:" << std::endl << W << std::endl << std::endl;
  std::cout << "Hinverse:" << std::endl << Hinv << std::endl << std::endl;
  */

/*
  std::cout << "H * W (should be identity):" << std::endl << H * W << std::endl << std::endl;
*/
  //std::cout << "W * H (should be identity):" << std::endl << W * H << std::endl << std::endl;
/*
  std::cout << " Xi*A + Upsilon*X (should be zero):" << std::endl << Xi*A + Upsilon*X << std::endl << std::endl;

  Eigen::FullPivLU<Eigen::MatrixXd> lu_decompH(H);
  Eigen::FullPivLU<Eigen::MatrixXd> lu_decompWinv(Winv);

  std::cout << "rank(H) = " << lu_decompH.rank() << "\t rank(Winv) = " << lu_decompWinv.rank() << std::endl << std::endl;
*/
  return WLUinv;
}


bool DFO_Model::FindReplacementPoint(int t, Eigen::VectorXd &dNew, int compareIdx) {
  Eigen::VectorXd grad;
  Eigen::MatrixXd hess = Eigen::MatrixXd::Zero(n, n);
  // Creating the lagrange polynomial.
  hess.setZero();
  double c = Xi(0, t - 1);
  grad = (Xi.col(t - 1)).tail(n);
  for (int k = 1; k <= m; ++k) {
    double tmp = Z.row(k - 1) * S * (Z.row(t - 1)).transpose();
    hess += tmp * (Y.col(k - 1)) * (Y.col(k - 1)).transpose();
  }
  double val = 0; //c + dNew.transpose()*grad + dNew.transpose()*hess*dNew;

  // Find min and max of l_t(x)
  subproblem.setConstant(c);
  subproblem.setGradient(grad);
  subproblem.setHessian(hess);

  vector<double> xsolMax;
  vector<double> fsolMax;
  vector<double> xsolMin;
  vector<double> fsolMin;

  Eigen::VectorXd startingPoint = Eigen::VectorXd::Zero(n);
  int j = 1;
  int sub = 0;
  double incBest = 0;
  for (int i = 1; i <= 2 * n; ++i) {
    startingPoint[i - 1 - sub] = rho * j + bestPoint[i - 1 - sub];
    subproblem.Solve(xsolMax, fsolMax, (char *) "Maximize", y0, bestPoint, startingPoint);
    subproblem.Solve(xsolMin, fsolMin, (char *) "Minimize", y0, bestPoint, startingPoint);

    Eigen::VectorXd optimum(n);

    if (abs(fsolMax[0]) >= abs(fsolMin[0])) {
      val = abs(fsolMax[0]);
    } else {
      val = abs(fsolMin[0]);
    }

    if (val > incBest) {
      incBest = val;
      for (int i = 0; i < xsolMax.size(); ++i) {
        if (abs(fsolMax[0]) >= abs(fsolMin[0])) {
          dNew[i] = xsolMax[i];
        } else {
          dNew[i] = xsolMin[i];
        }
      }
    }

    //if (norm(dNew - bestPoint)/rho >  0.001* norm( Y.col(compareIdx-1) - bestPoint)/rho){

    //  break;
    //}
    startingPoint[i - 1 - sub] = 0;
    if (i == n) {
      j = -1;
      sub += n;
    }
  }
  //std::cout << "abs lagrange value: " << incBest << std::endl;
  //std::cout << "New suggested point is: \n" << dNew << "\n";

  //if (incBest < 0.000000001){
    Eigen::VectorXd yTry(n); //Displacement from current center point
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(-rho, rho);
    int k = 0;

    for (int k = 0; k <= 2*n; ++k){
      for (int t = 0; t < n; ++t) {
        startingPoint(t) = dis(gen) + bestPoint[t];
      }

      subproblem.Solve(xsolMax, fsolMax, (char *) "Maximize", y0, bestPoint, startingPoint);
      subproblem.Solve(xsolMin, fsolMin, (char *) "Minimize", y0, bestPoint, startingPoint);
      double val2 = c + startingPoint.transpose()*startingPoint + startingPoint.transpose()*hess*startingPoint;

      Eigen::VectorXd optimum(n);

      if (abs(fsolMax[0]) >= abs(fsolMin[0])) {
        val = abs(fsolMax[0]);
      } else {
        val = abs(fsolMin[0]);
      }

      if (val > incBest) {
        incBest = val;
        for (int i = 0; i < xsolMax.size(); ++i) {
          if (abs(fsolMax[0]) >= abs(fsolMin[0])) {
            dNew[i] = xsolMax[i];
          } else {
            dNew[i] = xsolMin[i];
          }
        }
      }
      if (val2 > incBest){
        incBest = val2;
        dNew = startingPoint;
      }


    }
    std::cout << "abs lagrange value: " << incBest << std::endl;
    std::cout << "New suggested point is: \n" << dNew << "\n";
  //}


} //abs lagrange value: 0.00000000056247488472


void DFO_Model::UpdateOptimum(){
  int i = 1;

  for (int j = 2; j <= m; ++j){
    if (fvals[j-1] < fvals[i-1]){
      i = j;
    }
  }

  if (i != bestPointIndex){
    bestPointIndex = i;
    bestPoint = Y.col(i-1);
  }
}


void DFO_Model::isPoised(Eigen::VectorXd &dNew, int &indexOfPointToBeReplaced, double radius){
  indexOfPointToBeReplaced = -1;
  int numberOfPointsOutsideRadius = GetNumberOfPointsOutsideRadius(radius);
  if (numberOfPointsOutsideRadius >= 1){
    /// Find points outside r*radius
    Eigen::VectorXi sortedPoints = GetInterpolationPointsSortedByDistanceFromBestPoint();
    sortedPoints.conservativeResize(numberOfPointsOutsideRadius);

    subproblem.SetTrustRegionRadius(radius/r);
    for (int i = 0; i < numberOfPointsOutsideRadius; ++i){
      dNew = FindLocalOptimumOfAbsoluteLagrangePolynomial(sortedPoints[i]);
      double lagabsval = std::abs(ComputeLagrangePolynomial(sortedPoints[i], dNew));
      std::cout << "lagabsval = " << lagabsval << "\n";
      if (lagabsval > 0.001) {
        indexOfPointToBeReplaced = sortedPoints[i];
        break;
      }
      else{
        std::cout << "best point (init point): \n" << bestPoint << "\n";
        std::cout << "point found: \n" << dNew <<"\n";
        std::cout << "Lag pol that cannot be maximized\n";
        PrintLagrangePolynomial(sortedPoints[i]);
        }
        //break;
      }
    }

  else{
    subproblem.SetTrustRegionRadius(radius);
    findWorstPointInInterpolationSet(dNew,indexOfPointToBeReplaced);
  }
}

void DFO_Model::modelImprovementStep(Eigen::VectorXd &dNew, int &indexOfPointToBeReplaced){
  indexOfPointToBeReplaced = -1;
  Eigen::VectorXi sortedPoints = GetInterpolationPointsSortedByDistanceFromBestPoint();
  for (int i = 0; i < m; ++i){
    int t = sortedPoints[i];
    subproblem.SetTrustRegionRadius(rho);
    dNew = FindLocalOptimumOfAbsoluteLagrangePolynomial(t);
    double lagabsval = std::abs(ComputeLagrangePolynomial(t, dNew));
    if (lagabsval > lambda || norm((Y.col(t-1) - bestPoint))
        > r * rho) {

      if (lagabsval > 0.001){
        std::cout << "lagabsval = " << lagabsval << "\n";
        std::cout << "Distance from optimum:   " << norm(Y.col(t-1) - bestPoint) << "\n";
        std::cout << "r*radius:                " << r*rho << "\n";
        indexOfPointToBeReplaced = t;
        break;
      }
    }
  }
}

}
}