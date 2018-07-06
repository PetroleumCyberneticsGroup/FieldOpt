#include <limits>
#include "Subproblem.h"
namespace Optimization {
namespace Optimizers {

#ifdef __cplusplus
extern "C" {
#endif
int SNOPTusrFG3_(integer *Status, integer *n, doublereal x[],
                 integer *needF, integer *neF, doublereal F[],
                 integer *needG, integer *neG, doublereal G[],
                 char *cu, integer *lencu,
                 integer iu[], integer *leniu,
                 doublereal ru[], integer *lenru);
#ifdef __cplusplus
}
#endif


void smallTightning(double &value, bool lower){
  /*
  if (lower && value < 0){
    value = value*0.999;
  }
  else if(lower && value > 0){
    value = value*1.001;
  }
  else if((!lower) && value > 0){
    value = value*0.999;
  }
  else if((!lower) && value < 0){
    value = value*1.001;
  }
   */
}

//static double *gradient;
//static double *hessian;
//static double constant;
static Eigen::VectorXd yb_rel;
static Eigen::VectorXd y0;
static int normType;

static Eigen::MatrixXd hessian;
static Eigen::VectorXd gradient;
static double constant;
static double scale;
static VirtualSimulator virtualSimulator;

//Subproblem::Subproblem(SNOPTHandler snoptHandler) {
Subproblem::Subproblem(Settings::Optimizer *settings) {
  settings_ = settings;
  n_ = settings->parameters().number_of_variables;
  y0_ = Eigen::VectorXd::Zero(n_);
  y0 = Eigen::VectorXd::Zero(n_);
  bestPointDisplacement_ = Eigen::VectorXd::Zero(n_);
  xlowCopy_ = Eigen::VectorXd::Zero(n_); /// OBS should be set by the driver file....
  xuppCopy_ = Eigen::VectorXd::Zero(n_);
  loadSNOPT();
  normType = settings->parameters().norm_type;
  normType_ = settings->parameters().norm_type;
  //normType_ = Subproblem::INFINITY_NORM;
  //normType = Subproblem::INFINITY_NORM;
  //normType_ = Subproblem::L2_NORM;
  //normType = Subproblem::L2_NORM;

  virtualSimulator = VirtualSimulator(settings->parameters().test_problem_file);
  m_ = virtualSimulator.GetNumberOfConstraints();
  lastLagrangeMultipliers = Eigen::VectorXd::Zero(m_);



  setConstraintsAndDimensions(); // This one should set the iGfun/jGvar and so on.
  setAndInitializeSNOPTParameters();

  scale = 1;
  hessian = Eigen::MatrixXd::Zero(n_,n_);
  gradient = Eigen::VectorXd::Zero(n_);
  constant = 0;

  ResetSubproblem();

}

SNOPTHandler Subproblem::initSNOPTHandler() {
  string prnt_file, smry_file, optn_file;
  optn_file = settings_->parameters().thrdps_optn_file.toStdString() + ".opt.optn";
  smry_file = settings_->parameters().thrdps_smry_file.toStdString() + ".opt.summ";
  prnt_file = settings_->parameters().thrdps_prnt_file.toStdString() + ".opt.prnt";
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

void Subproblem::setAndInitializeSNOPTParameters() {
  // the controls
  x_ = new double[n_];
  // the initial guess for Lagrange multipliers
  xmul_ = new double[n_];
  // the state of the variables (whether the optimal is likely to be on
  // the boundary or not)
  xstate_ = new integer[n_];
  F_ = new double[neF_];
  Fmul_ = new double[neF_];
  Fstate_ = new integer[neF_];

  nxnames_ = 1;
  nFnames_ = 1;
  xnames_ = new char[nxnames_ * 8];
  Fnames_ = new char[nFnames_ * 8];

}

void Subproblem::Solve(vector<double> &xsol, vector<double> &fsol, char *optimizationType, Eigen::VectorXd centerPoint, Eigen::VectorXd bestPointDisplacement) {
  y0_ = centerPoint;
  y0 = y0_;
  bestPointDisplacement_ = bestPointDisplacement;
  yb_rel = bestPointDisplacement_;
  // Set norm specific constraints
  if (normType_ == INFINITY_NORM){
    for (int i = 0; i < n_; ++i){
      xlow_[i] = std::max(bestPointDisplacement_[i] - trustRegionRadius_, xlowCopy_[i] - y0_[i]);
      xupp_[i] = std::min(bestPointDisplacement_[i] + trustRegionRadius_, xuppCopy_[i] - y0_[i]);
    }
    //xlow_[1] = xlow_[1];
    //xupp_[1] = xupp_[1];
  }
  else if (normType_ == L2_NORM){
    for (int i = 0; i < n_; ++i){
      xlow_[i] = xlowCopy_[i];
      xupp_[i] = xuppCopy_[i];
    }
  }


  // The snoptHandler must be setup and loaded
  SNOPTHandler snoptHandler = initSNOPTHandler();
  snoptHandler.initializeLagrangeVector(neF_-1);
  snoptHandler.setProbName("SNOPTSolver");
  snoptHandler.setParameter(optimizationType);

  setOptionsForSNOPT(snoptHandler);
  snoptHandler.setRealParameter("Major step limit", trustRegionRadius_); //was 0.2
  //target nonlinear constraint violation
  snoptHandler.setRealParameter("Major feasibility tolerance", 0.00000000001); //1.0e-6
  //target complementarity gap
  double val = constant + gradient.transpose() * bestPointDisplacement + 0.5 * bestPointDisplacement.transpose() * hessian * bestPointDisplacement;
  snoptHandler.setRealParameter("Major optimality tolerance", 0.000000000000000001*trustRegionRadius_);
  if (val > 10){
    snoptHandler.setRealParameter("Major optimality tolerance", 0.000000000000000001*val);
  }

  scale = constant + gradient.transpose() * bestPointDisplacement + 0.5* bestPointDisplacement.transpose() * hessian * bestPointDisplacement;
  scale = abs(scale);
  if (abs(scale) <= 0.0000000001){
    scale = 1;
  }

  ResetSubproblem();
  for (int i = 0; i < n_; i++) {
    x_[i] = bestPointDisplacement_[i];
  }
  passParametersToSNOPTHandler(snoptHandler);
  integer Cold = 0, Basis = 1, Warm = 2;

  snoptHandler.solve(Cold, xsol, fsol);
  fsol[0] = fsol[0]*scale;
  integer exitCode = snoptHandler.getExitCode();
/*
  if (exitCode == 32){
    cout << "The major iteration limit was reached, trying to increase it to improve on the result" << endl;
    ResetSubproblem();
    snoptHandler.setIntParameter("Major Iterations Limit", 10000);
    snoptHandler.setRealParameter("Major step limit", 0.2);
    snoptHandler.solve(Cold, xsol, fsol);
  }
  exitCode = snoptHandler.getExitCode();
  if (exitCode == 32 || exitCode == 31){
    cout << "The major iteration limit or the iteration limit was reached, trying to increase it to improve on the result" << endl;
    ResetSubproblem();
    for (int i = 0; i < n_; i++) {
      x_[i] = xsol[i];
    }
    passParametersToSNOPTHandler(snoptHandler);
    snoptHandler.setIntParameter("Major Iterations Limit", 12000);
    snoptHandler.setRealParameter("Major step limit", 0.01);
    snoptHandler.setIntParameter("Iterations limit", 20000);
    snoptHandler.solve(Cold, xsol, fsol);
  }

  if (exitCode != 1 && exitCode != 31&& exitCode != 32&& exitCode != 0&& exitCode != 3){
    std::vector<double> copyX = xsol;
    std::vector<double> copyFsol = fsol;

    ResetSubproblem();
    for (int i = 0; i < n_; i++) {
      x_[i] = bestPointDisplacement_[i];
    }
    passParametersToSNOPTHandler(snoptHandler);
    snoptHandler.solve(Cold, xsol, fsol);
    if (optimizationType == (char *) "Maximize"){
      if (fsol[0] < copyFsol[0]){
        xsol=copyX;
        fsol=copyFsol;
      }
    }else if(optimizationType == (char *) "Minimize"){
      if (fsol[0] > copyFsol[0]){
        xsol=copyX;
        fsol=copyFsol;
      }
    }


  }
*/
}

double Subproblem::computeScale(){
  Eigen::VectorXd yTry(n_); //Displacement from current center point
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-trustRegionRadius_, trustRegionRadius_);
  int k = 0;
  double value = abs(constant + gradient.transpose() * bestPointDisplacement_ + 0.5* bestPointDisplacement_.transpose() * hessian * bestPointDisplacement_);
  ++k;


  while (k < 50) {
    for (int i = 0; i < n_; ++i) {
      yTry(i) = dis(gen) + bestPointDisplacement_[i];
    }
    double val = std::abs(constant + gradient.transpose() * yTry + 0.5 * yTry.transpose() * hessian * yTry);
    value += val;
    ++k;
  }
  return (value/50.0);
}


void Subproblem::Solve(vector<double> &xsol, vector<double> &fsol, char *optimizationType, Eigen::VectorXd centerPoint, Eigen::VectorXd bestPointDisplacement, Eigen::VectorXd startingPoint){
  y0_ = centerPoint;
  y0 = y0_;
  bestPointDisplacement_ = bestPointDisplacement;
  yb_rel = bestPointDisplacement_;
  // Set norm specific constraints
  //std::cout << "lower bounds\n" << xlowCopy_ << "\n";
  //std::cout << "upper bounds\n" << xuppCopy_ << "\n";
  if (normType_ == INFINITY_NORM){
    for (int i = 0; i < n_; ++i){
      xlow_[i] = std::max(bestPointDisplacement_[i] - trustRegionRadius_, xlowCopy_[i] - y0_[i]);
      xupp_[i] = std::min(bestPointDisplacement_[i] + trustRegionRadius_, xuppCopy_[i] - y0_[i]);
    }
  }
  else if (normType_ == L2_NORM){
    for (int i = 0; i < n_; ++i){
      xlow_[i] = xlowCopy_[i];
      xupp_[i] = xuppCopy_[i];
    }
  }
  //std::cout << "lower bounds " << xlow_[0] << " " << xlow_[1] << " " << xlow_[2] << " " << xlow_[3] << " " << "\n";
  //std::cout << "upper bounds " <<  xupp_[0] << " " << xupp_[1] << " " << xupp_[2] << " " << xupp_[3] << " " << "\n";

  double s1 = abs(constant + gradient.transpose() * bestPointDisplacement + 0.5* bestPointDisplacement.transpose() * hessian * bestPointDisplacement);
  Eigen::VectorXd d(n_);
  d.setZero();
  scale = 1; //computeScale();


  // The snoptHandler must be setup and loaded
  SNOPTHandler snoptHandler = initSNOPTHandler();
  snoptHandler.setProbName("SNOPTSolver");
  snoptHandler.setParameter(optimizationType);
  snoptHandler.initializeLagrangeVector(neF_-1);

  setOptionsForSNOPT(snoptHandler);
  //snoptHandler.setIntParameter("Major Iterations Limit", 20000);
  //snoptHandler.setIntParameter("Iterations limit", 20000);
  snoptHandler.setRealParameter("Major step limit", trustRegionRadius_); //was 0.2
  //target nonlinear constraint violation
  snoptHandler.setRealParameter("Major feasibility tolerance", 1.0e-9); //1.0e-6
  //snoptHandler.setRealParameter("Major feasibility tolerance", 1.0e-6); //1.0e-6

  //double val = constant + gradient.transpose() * bestPointDisplacement + 0.5 * bestPointDisplacement.transpose() * hessian * bestPointDisplacement;
  snoptHandler.setRealParameter("Major optimality tolerance", 0.00001);
  //snoptHandler.setRealParameter("Major optimality tolerance", 0.00000000000000000000001*trustRegionRadius_);

  //if (std::abs(val) > 10){
    //snoptHandler.setRealParameter("Major optimality tolerance", 0.000000000000000001*val);
  //}
  //snoptHandler.setRealParameter("Major optimality tolerance", 0.00000001);

  ResetSubproblem();
  for (int i = 0; i < n_; i++) {
    //x_[i] = startingPoint[i];//bestPointDisplacement_[i];
    //x_[i] = 0.0;//bestPointDisplacement_[i];
    //x_[i] = startingPoint[i];
  }
  if (normType_ == Subproblem::L2_NORM) {
    Flow_[1] = 0;
    Fupp_[1] = trustRegionRadius_;
  }
  passParametersToSNOPTHandler(snoptHandler);
  integer Cold = 0, Basis = 1, Warm = 2;

  /*
  std::cout << "Flow_ = \n";
  for (int i = 0; i < neF_; ++i){
    std::cout  << Flow_[i] <<"\n";
  }
  std::cout << "Fupp_ = \n" <<"\n";
  for (int i = 0; i < neF_; ++i){
    std::cout  << Fupp_[i] <<"\n";
  }
*/

  /// Print all the constraint such that I can (try) to see why it becomes infeasible.

  //std::cout << "Flow\t Fupp\n";
  //for (int i = 0; i < neF_; i++){
  //  std::cout << Flow_[i] << "\t" << Fupp_[i] << "\n";
  //}
  std::cout << "\nxlow\t xupp\n";
  for (int i = 0; i < n_; i++){
    std::cout << xlow_[i] << "\t" << xupp_[i] << "\n";
  }


  snoptHandler.solve(Cold, xsol, fsol);
  lastLagrangeMultipliers = snoptHandler.getLagrangeMultipliers();
  fsol[0] = fsol[0]*scale;
  integer exitCode = snoptHandler.getExitCode();
  //std::cout << "snopt exitcode:  " << exitCode << "\n";
  if (exitCode == 40 || exitCode == 41){
    //std::cout << "snopt failed me. current point cannot be improved because of numerical difficulties \n";

    //std::cin.get();
  }
  if (exitCode != 40 && exitCode != 41 && exitCode != 1 && exitCode != 31 && exitCode != 3 && exitCode != 32){
    std::cout << "ExitCode is: " << exitCode << "\n";
    std::cin.get();
  }


  Eigen::VectorXd xvec(n_);
  for (int i = 1; i <= n_; ++i){
    xvec(i-1) = xsol[i-1];
  }
  if ( virtualSimulator.IsFeasiblePoint(xvec + y0) == false){
    std::cout << "output from snopt is infeasible. ExitCode was: " <<  exitCode << "\n";
    auto d = virtualSimulator.evaluateConstraints(xvec + y0);
    std::cout << d <<"\n";
  }


  Eigen::VectorXd tmp(n_);
  if (virtualSimulator.IsFeasiblePoint(xvec + y0) == false) {

    if ( virtualSimulator.IsFeasiblePoint(xvec + y0) == false){
      std::cout << "output from snopt is infeasible. Will do random search\n";
    }


    double snopt_suggested_val = constant + gradient.transpose() * xvec + 0.5 * xvec.transpose() * hessian * xvec;
    //std::cout << "ExitCode was: " << exitCode << "\n";
    //printModel();
    //std::cout << "Diff is: " << (xvec - bestPointDisplacement_).norm() << "\n";
    //Point found by SNOPT is the same. Let's do some horrible brute-force searching.
    Eigen::VectorXd yTry(n_); //Displacement from current center point
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-trustRegionRadius_, trustRegionRadius_);
    //std::cout << "DIS_info\nMin: " << dis.min() << "\nMax: " << dis.max() << "\n";
    int k = 0;

    Eigen::VectorXd yBest = xvec;

    double bestValue = constant + gradient.transpose() * xvec + 0.5 * xvec.transpose() * hessian * xvec;
    double value = 0;

    //std::cout << "trustRegionRadius_ = " << trustRegionRadius_ << "\n";
    int ll = 0;
    bool lock = false;
    int fails = 0;
    while (k < 5000) {
      for (int i = 0; i < n_; ++i) {
        yTry(i) = dis(gen) + bestPointDisplacement_[i];
      }
      if (virtualSimulator.IsFeasiblePoint(yTry + y0) == false){
        fails++;
        continue;
      }
      else{
        if (lock == false){
          yBest = yTry;
          bestValue = constant + gradient.transpose() * yTry + 0.5 * yTry.transpose() * hessian * yTry;
          lock = true;
        }
      }
      value = constant + gradient.transpose() * yTry + 0.5 * yTry.transpose() * hessian * yTry;
      if (std::string(optimizationType) == "Maximize" ) {
        if (value > bestValue) {
          bestValue = value;
          yBest = yTry;
        }
      }
      if (std::string(optimizationType) == "Minimize") {
        if (value < bestValue) {
          bestValue = value;
          yBest = yTry;
        }
      }

      ++k;
    }
    std::cout << "Randomly generated point is infeasible, number of times: " << fails << "\n";
    tmp = yBest;
    for (int i = 1; i <= n_; ++i) {
      xsol[i - 1] = yBest[i - 1];
      fsol[0] = constant + gradient.transpose() * yBest + 0.5 * yBest.transpose() * hessian * yBest;
    }
    //std::cout << "Diff is: " << (yBest-bestPointDisplacement_).lpNorm<Infinity>() << "\n";
    //std::cout << "Radius is: " << trustRegionRadius_ << "\n";
  }

  if ( virtualSimulator.IsFeasiblePoint(xvec + y0) == false){
    std::cout << "Failed to find feasible points...\n";
  }

}


void Subproblem::ResetSubproblem() {
  for (int i = 0; i < n_; i++) {
    Fstate_[i] = 0;
    xstate_[i] = 0;
    x_[i] = 0.0;
    xmul_[i] = 0;
  }

  for (int h = 0; h < neF_; h++) {
    F_[h] = 0.0;
    Fmul_[h] = 0.0;
  }

}

void Subproblem::passParametersToSNOPTHandler(SNOPTHandler &snoptHandler) {
  snoptHandler.setProblemSize(n_, neF_);
  snoptHandler.setObjective(objRow_);
  snoptHandler.setA(lenA_, iAfun_, jAvar_, A_);
  snoptHandler.setG(lenG_, iGfun_, jGvar_);
  snoptHandler.setX(x_, xlow_, xupp_, xmul_, xstate_);
  snoptHandler.setF(F_, Flow_, Fupp_, Fmul_, Fstate_);
  snoptHandler.setXNames(xnames_, nxnames_);
  snoptHandler.setFNames(Fnames_, nFnames_);
  snoptHandler.setNeA(neA_);
  snoptHandler.setNeG(neG_);
  snoptHandler.setUserFun(SNOPTusrFG3_);
}

void Subproblem::setConstraintsAndDimensions() {
  // This must be set before compiling the code. It cannot be done during runtime through function calls.
  //n_ = 10;

  if (normType_ == Subproblem::L2_NORM){
    m_++;
  }
  neF_ = m_ + 1;
  lenA_ = 0;
  lenG_ = n_ + m_ * n_;
  objRow_ = 0; // In theory the objective function could be any of the elements in F.
  objAdd_ = 0.0;

  iGfun_ = new integer[lenG_];
  jGvar_ = new integer[lenG_];

  iAfun_  = NULL;
  jAvar_  = NULL;
  A_      = NULL;

  /*
 iAfun_ = new integer[lenA_];
 jAvar_ = new integer[lenA_];
 A_ = new double[lenA_];
*/
  /*
  iAfun_[0] = 1;
  jAvar_[0] = 0;
  iAfun_[1] = 1;
  jAvar_[1] = 1;
  iAfun_[2] = 2;
  jAvar_[2] = 0;
  iAfun_[3] = 2;
  jAvar_[3] = 1;
  A_[0] = 1.0;
  A_[1] = 1.2;
  A_[2] = 0.9;
  A_[3] = 3.0;
*/
  // controls lower and upper bounds
  xlow_ = new double[n_];
  xupp_ = new double[n_];

  Flow_ = new double[neF_];
  Fupp_ = new double[neF_];


  // Objective function
  Flow_[0] = -infinity_;
  Fupp_[0] =  infinity_;

  // Trust region radius
  if (normType_ == Subproblem::L2_NORM) {
    Flow_[1] = 0;
    Fupp_[1] = trustRegionRadius_;
  }
  Eigen::VectorXd x_lb = virtualSimulator.GetLowerBoundsForVariables();
  if (x_lb.rows() < n_){
    int tmp1 = x_lb.rows();
    x_lb.conservativeResize(n_);
    for (int i = tmp1; i < n_;++i){
      x_lb[i] = -infinity_;
    }
  }
  Eigen::VectorXd x_ub = virtualSimulator.GetUpperBoundsForVariables();
  if (x_ub.rows() < n_){
    int tmp2 = x_ub.rows();
    x_ub.conservativeResize(n_);
    for (int i = tmp2; i < n_;++i){
      x_ub[i] = infinity_;
    }
  }
  //std::cout << "x_lb \n" << x_lb <<"\nx_ub\n"<<x_ub << "\n";
  for(int i = 0; i < n_; ++i){
    //std::cout << "std::isinf(x_lb[i]) = " << std::isinf(x_lb[i]) << "\n";
    //std::cout << "std::isinf(x_ub[i]) = " << std::isinf(x_ub[i]) << "\n";

    if (std::isinf(x_lb[i])){
      xlow_[i] = - infinity_;
    } else{
      xlow_[i] = x_lb[i];
    }
    if (std::isinf(x_ub[i])){
      xupp_[i] = infinity_;
    } else{
      xupp_[i] = x_ub[i];

    }

    if (std::abs(x_lb[i]-x_ub[i]) <= 0.00000000001){
      xlow_[i] = x_lb[i];
      xupp_[i] = x_lb[i];
    }

    xlowCopy_[i] = xlow_[i];
    xuppCopy_[i] = xupp_[i];
  }

  int startI = 1;
  if (normType_ == Subproblem::L2_NORM){
    startI++;
  }
  Eigen::VectorXd g_lb = virtualSimulator.GetLowerBoundsForConstraints();
  Eigen::VectorXd g_ub = virtualSimulator.GetUpperBoundsForConstraints();
  std::cout << "g_lb \n" << g_lb <<"\ng_ub\n"<<g_ub << "\n";

  for (int i = startI; i < neF_; ++i){

    if (std::isinf(g_lb[i-startI])){
      Flow_[i] = -infinity_;
    } else{
      Flow_[i] = g_lb[i-startI];
      smallTightning(Flow_[i], 1);
    }
    if (std::isinf(g_ub[i-startI])){
      Fupp_[i] = infinity_;
    } else{
      Fupp_[i] = g_ub[i-startI];
      smallTightning(Fupp_[i], 0);

    }

    /*
    if (std::abs(g_lb[i-startI]-g_ub[i-startI]) <= 0.00000000001){
      Flow_[i] = g_lb[i-startI];
      Fupp_[i] = g_lb[i-startI];
    }
    */

    //Flow_[i] = g_lb[i-startI];
    //Fupp_[i] = g_ub[i-startI];
  }

  std::cout << "Flow_ = \n";
  for (int i = 0; i < neF_; ++i){
    std::cout  << Flow_[i] <<"\n";
  }
  std::cout << "\nFupp_ = \n";
  for (int i = 0; i < neF_; ++i){
    std::cout  << Fupp_[i] <<"\n";
  }

  std::cout << "xlow_ = \n";
  for (int i = 0; i < n_; ++i){
    std::cout  << xlow_[i] <<"\n";
  }
  std::cout << "\nxupp_ = \n";
  for (int i = 0; i < n_; ++i){
    std::cout  << xupp_[i] <<"\n";
  }

  // first the objective
  for (int i = 0; i < n_; i++)
  {
    iGfun_[i] = 0;
    jGvar_[i] = i;
  }

  if (m_ != 0)
  {
    // and then the constraints
    for (int j = 1; j <= m_; j++)
    {
      for (int i = 0; i < n_; i++)
      {
        iGfun_[i + j * n_] = j;
        jGvar_[i + j * n_] = i;
      }
    }
  }

  neG_ = lenG_;
  neA_ = lenA_;


}

Subproblem::~Subproblem() {
  delete[] iGfun_;
  delete[] jGvar_;
  delete[] x_;
  delete[] xlow_;
  delete[] xupp_;
  delete[] xmul_;
  delete[] xstate_;
  delete[] F_;
  delete[] Flow_;
  delete[] Fupp_;
  delete[] Fmul_;
  //delete[] Fstate_;
  //delete[] xnames_;
  //delete[] Fnames_;
}

void Subproblem::setOptionsForSNOPT(SNOPTHandler &snoptHandler) {

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
  snoptHandler.setRealParameter("Major feasibility tolerance", 0.0000001);
  snoptHandler.setIntParameter("Major Iterations Limit", 1000);

  //target complementarity gap
  //snoptHandler.setRealParameter("Major optimality tolerance", 0.000000000001);

  snoptHandler.setParameter("Major Print level  00000"); //  00001"
  snoptHandler.setRealParameter("Major step limit", 0.2); //was 0.2
  //snoptHandler.setIntParameter("Minor iterations limit", 200); // 200

  //for satisfying the QP bounds
  snoptHandler.setRealParameter("Minor feasibility tolerance",  1.0e-3);
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
  snoptHandler.setRealParameter("Unbounded objective value", 1.0e+18); // infinity_ = 1e20; "Unbounded objective value   1.0e+15"
  snoptHandler.setRealParameter("Unbounded step size",       1.0e+19); //"Unbounded step size             1.0e+18"
  //snoptHandler.setIntParameter("Verify level", -1); //-1
  //snoptHandler.setRealParameter("Violation limit", 1e-8); //1e-8

//  if (settings_->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
//    cout << "[opt]Set options for SNOPT.---" << endl;

}

/*****************************************************
ADGPRS, version 1.0, Copyright (c) 2010-2015 SUPRI-B
Author(s): Oleg Volkov          (ovolkov@stanford.edu)
           Vladislav Bukshtynov (bukshtu@stanford.edu)
******************************************************/

bool Subproblem::loadSNOPT(const string libname) {

//#ifdef NDEBUG
  if (LSL_isSNOPTLoaded()) {
    printf("\x1b[33mSnopt is already loaded.\n\x1b[0m");
    return true;
  }

  char buf[256];
  int rc;
  if (libname.empty()) {
    rc = LSL_loadSNOPTLib(NULL, buf, 255);
  } else {
    rc = LSL_loadSNOPTLib(libname.c_str(), buf, 255);
  }

  if (rc) {
    string errmsg;
    errmsg = "Selected NLP solver SNOPT not available.\n"
        "Tried to obtain SNOPT from shared library \"";
    errmsg += LSL_SNOPTLibraryName();
    errmsg += "\", but the following error occured:\n";
    errmsg += buf;
    cout << errmsg << endl;
    return false;
  }
//#endif

  return true;
}

void restrictNumberValueToMatchSNOPT(double &value){
  if (value == -std::numeric_limits<double>::infinity() || value <= -1e20){
    value = -1e20;
  }
  else if(value == std::numeric_limits<double>::infinity() || value >= 1e20){
    value = 1e20;
  }
}

int SNOPTusrFG3_(integer *Status, integer *n, double x[],
                 integer *needF, integer *neF, double F[],
                 integer *needG, integer *neG, double G[],
                 char *cu, integer *lencu,
                 integer iu[], integer *leniu,
                 double ru[], integer *lenru) {

  // Convert std::vector into Eigen3 vector
  Eigen::VectorXd xvec(*n);
  for (int i = 0; i < *n; ++i){
    xvec[i] = x[i];
  }

  int m = *neF - 1;
/*
  Eigen::MatrixXd h = Eigen::MatrixXd::Zero(2,2);
  h << 1, 2, 2, -1;
  Eigen::VectorXd g(2);
  g << 1, 0.9;
  double c = 12;
  constant = c;
  hessian = h;
  gradient = g;
  //normType = 2;
*/
  // If the values for the objective and/or the constraints are desired
  if (*needF > 0) {
    /// The objective function
    F[0] = (constant + gradient.transpose() * xvec + 0.5* xvec.transpose() * hessian * xvec)/scale;
    restrictNumberValueToMatchSNOPT(F[0]);
    if (m) {
      /// The constraints
      int startI = 1;
      if (normType == Subproblem::L2_NORM) {
        F[1] = (xvec - yb_rel).norm();
        startI++;
      }
    Eigen::VectorXd constraints = virtualSimulator.evaluateConstraints(xvec+y0);
      //std::cout << constraints << "\n";
      for (int i = 0; i < constraints.rows(); ++i){
        F[i+startI] = constraints[i];
        restrictNumberValueToMatchSNOPT(F[i+startI]);
      }
    }
  }


  if (*needG > 0) {
    /// The Derivatives of the objective function
    Eigen::VectorXd grad(*n);
    grad = gradient + hessian*xvec;
    for (int i = 0; i < *n; ++i){
      G[i] = grad(i);
      restrictNumberValueToMatchSNOPT(G[i]);
    }

    /// The derivatives of the constraints
    if (m) {
      Eigen::VectorXd gradConstraint(*n);
      int startI=*n;
      if (normType == Subproblem::L2_NORM) {
        gradConstraint = (xvec - yb_rel) / ((xvec - yb_rel).norm() + 0.000000000000001);
        for (int i = 0; i < *n; ++i) {
          //G[startI + i] = gradConstraint(i);
          //restrictNumberValueToMatchSNOPT(G[startI + i]);
        }
        startI+=*n;
      }
      Eigen::MatrixXd constraintsGradients = virtualSimulator.evaluateConstraintGradients(xvec+y0);
      for (int i = 0; i < constraintsGradients.rows() ; ++i){ //for each contraint
        for (int j = 0; j < constraintsGradients.cols(); ++j){ //for each variable.
          G[startI + i*(*n)+j] = constraintsGradients(i,j);
          restrictNumberValueToMatchSNOPT(G[startI + i*(*n)+j]);
        }
      }
      //std::cout << grad.transpose() << "\n";
      //std::cout << constraintsGradients << "\n";
      //std::cout << "G:\n"<< G[0] << "  " << G[1] << "\n"<< G[2] << "  " << G[3] << "\n"<< G[4] << "  " << G[5] <<  "\n"<< G[6] << "  " << G[7] << "\n";
    }
  }

  // The sphere
  /*
  F[0] = 0;
  for (int i = 0; i < *n; i++){
    F[0] += (xvec(i))*(xvec(i));
    G[i] = 2*(xvec(i));
  }
  F[1] = xvec.norm();
  Eigen::VectorXd gradConstraint(*n);
  gradConstraint = xvec/(xvec.norm()+0.00000000001);
  for (int i = 0; i < *n; ++i){
    G[i+*n] = gradConstraint(i);
  }
  */
  return 0;
}



void Subproblem::setQuadraticModel(double c, Eigen::VectorXd g, Eigen::MatrixXd H) {
  constant = c;
  gradient = g;
  hessian = H;
}

void Subproblem::setGradient(Eigen::VectorXd g) {
  gradient = g;
}

void Subproblem::setHessian(Eigen::MatrixXd H) {
  hessian = H;
}

void Subproblem::setConstant(double c) {
  constant = c;
}
void Subproblem::setNormType(int type) {
  normType = type;
}
void Subproblem::setCenterPointOfModel(Eigen::VectorXd cp) {
  y0 = cp;
}
void Subproblem::setCurrentBestPointDisplacement(Eigen::VectorXd db) {
  bestPointDisplacement_ = db;
  yb_rel = bestPointDisplacement_;
}
void Subproblem::SetCenterPoint(Eigen::VectorXd cp) {
  y0_ = cp;
  y0 = y0_;
}
void Subproblem::SetBestPointRelativeToCenterPoint(Eigen::VectorXd bp) {

}
void Subproblem::printModel() {
  std::cout << "The model of the subproblem \n";
  std::cout << "constant = \n" << constant << std::endl;
  std::cout << "gradient = \n" << gradient << std::endl;
  std::cout << "hessian = \n" << hessian << std::endl;
  std::cout << "trust region radius = " << trustRegionRadius_ << std::endl;

}
Eigen::VectorXd Subproblem::GetInitialPoint() {

   return virtualSimulator.GetInitialPoint();
}
Eigen::MatrixXd Subproblem::getGradientConstraints(Eigen::VectorXd point) {
  return virtualSimulator.evaluateConstraintGradients(point);
}
int Subproblem::getNumberOfConstraints() {
  return virtualSimulator.GetNumberOfConstraints();
}
void Subproblem::SolveVirtualSimulator() {
  virtualSimulator.Solve();
}
Eigen::VectorXd Subproblem::FindFeasiblePoint() {
  hessian = Eigen::MatrixXd::Zero(n_,n_);
  gradient = Eigen::VectorXd::Zero(n_);
  constant = 1;
  yb_rel = Eigen::VectorXd::Zero(n_);
  // The snoptHandler must be setup and loaded
  SNOPTHandler snoptHandler = initSNOPTHandler();
  snoptHandler.setProbName("SNOPTSolver");
  snoptHandler.setParameter("Feasible point");
  snoptHandler.initializeLagrangeVector(neF_-1);

  setOptionsForSNOPT(snoptHandler);
  //snoptHandler.setIntParameter("Major Iterations Limit", 20000);
  //snoptHandler.setIntParameter("Iterations limit", 20000);
  snoptHandler.setRealParameter("Major step limit", 0.2); //was 0.2
  //target nonlinear constraint violation
  snoptHandler.setRealParameter("Major feasibility tolerance", 1.0e-6); //1.0e-6
  //snoptHandler.setRealParameter("Major feasibility tolerance", 1.0e-6); //1.0e-6

  //double val = constant + gradient.transpose() * bestPointDisplacement + 0.5 * bestPointDisplacement.transpose() * hessian * bestPointDisplacement;
  //snoptHandler.setRealParameter("Major optimality tolerance", 0.00001);
  //snoptHandler.setRealParameter("Major optimality tolerance", 0.00000000000000000000001*trustRegionRadius_);

  //if (std::abs(val) > 10){
  //snoptHandler.setRealParameter("Major optimality tolerance", 0.000000000000000001*val);
  //}
  //snoptHandler.setRealParameter("Major optimality tolerance", 0.00000001);

  ResetSubproblem();
  for (int i = 0; i < n_; i++) {
    //x_[i] = startingPoint[i];//bestPointDisplacement_[i];
    //x_[i] = 0.0;//bestPointDisplacement_[i];
    //x_[i] = startingPoint[i];
  }
  //x_[0] = 1;
  //x_[1] = 2;
  if (normType_ == Subproblem::L2_NORM) {
    Flow_[1] = 0;
    Fupp_[1] = trustRegionRadius_;
  }
  passParametersToSNOPTHandler(snoptHandler);
  integer Cold = 0, Basis = 1, Warm = 2;

  /*
  std::cout << "Flow_ = \n";
  for (int i = 0; i < neF_; ++i){
    std::cout  << Flow_[i] <<"\n";
  }
  std::cout << "Fupp_ = \n" <<"\n";
  for (int i = 0; i < neF_; ++i){
    std::cout  << Fupp_[i] <<"\n";
  }
*/
  vector<double> xsol;
  vector<double> fsol;
      snoptHandler.solve(Cold, xsol, fsol);
  lastLagrangeMultipliers = snoptHandler.getLagrangeMultipliers();
  fsol[0] = fsol[0]*scale;
  integer exitCode = snoptHandler.getExitCode();
  //std::cout << "snopt exitcode:  " << exitCode << "\n";
  if (exitCode == 40 || exitCode == 41){
    //std::cout << "snopt failed me. current point cannot be improved because of numerical difficulties \n";

    //std::cin.get();
  }
  if (exitCode != 40 && exitCode != 41 && exitCode != 1 && exitCode != 31 && exitCode != 3 && exitCode != 32){
    std::cout << "ExitCode is: " << exitCode << "\n";
    std::cin.get();
  }


  Eigen::VectorXd xvec(n_);
  for (int i = 1; i <= n_; ++i){
    xvec(i-1) = xsol[i-1];
  }
  return xvec;
}
void Subproblem::evaluateConstraints(Eigen::VectorXd point) {
  if (virtualSimulator.GetNumberOfConstraints() > 0){
  auto d = virtualSimulator.evaluateConstraints(point+y0);
  std::cout << "Upper limits: \n" << virtualSimulator.GetUpperBoundsForConstraints() <<"\n___\n" <<
               "Lower limits: \n" << virtualSimulator.GetLowerBoundsForConstraints() <<"\n___\n" <<
               "Values: \n" << d << "\n" << "\n";

  }
}

void Subproblem::calculateLagrangeMultipliers(char *optimizationType,
                                  VectorXd centerPoint,
                                  VectorXd bestPointDisplacement,
                                  VectorXd startingPoint) {

  y0_ = centerPoint;
  y0 = y0_;
  bestPointDisplacement_ = bestPointDisplacement;
  yb_rel = bestPointDisplacement_;
  // Set norm specific constraints
  std::cout << "lower bounds\n" << xlowCopy_ << "\n";
  std::cout << "upper bounds\n" << xuppCopy_ << "\n";
  if (normType_ == INFINITY_NORM){
    for (int i = 0; i < n_; ++i){
      xlow_[i] = std::max(bestPointDisplacement_[i] - trustRegionRadius_, xlowCopy_[i] - y0_[i]);
      xupp_[i] = std::min(bestPointDisplacement_[i] + trustRegionRadius_, xuppCopy_[i] - y0_[i]);
    }
  }
  else if (normType_ == L2_NORM){
    for (int i = 0; i < n_; ++i){
      xlow_[i] = xlowCopy_[i];
      xupp_[i] = xuppCopy_[i];
    }
  }

  scale = 1; //computeScale();


  // The snoptHandler must be setup and loaded
  SNOPTHandler snoptHandler = initSNOPTHandler();
  snoptHandler.setProbName("SNOPTSolver");
  snoptHandler.setParameter(optimizationType);
  snoptHandler.initializeLagrangeVector(neF_-1);

  setOptionsForSNOPT(snoptHandler);
  snoptHandler.setIntParameter("Major Iterations Limit", 2);
  snoptHandler.setIntParameter("Iterations limit", 2);
  snoptHandler.setRealParameter("Major step limit", trustRegionRadius_*0.0001); //was 0.2
  //target nonlinear constraint violation
  snoptHandler.setRealParameter("Major feasibility tolerance", 1.0e-9); //1.0e-6
  //snoptHandler.setRealParameter("Major feasibility tolerance", 1.0e-6); //1.0e-6

  //double val = constant + gradient.transpose() * bestPointDisplacement + 0.5 * bestPointDisplacement.transpose() * hessian * bestPointDisplacement;
  snoptHandler.setRealParameter("Major optimality tolerance", 0.00001);
  snoptHandler.setIntParameter("Minor iterations limit", 1); // 200

  //snoptHandler.setRealParameter("Major optimality tolerance", 0.00000000000000000000001*trustRegionRadius_);

  //if (std::abs(val) > 10){
  //snoptHandler.setRealParameter("Major optimality tolerance", 0.000000000000000001*val);
  //}
  //snoptHandler.setRealParameter("Major optimality tolerance", 0.00000001);

  ResetSubproblem();
  for (int i = 0; i < n_; i++) {
    //x_[i] = startingPoint[i];//bestPointDisplacement_[i];
    x_[i] = bestPointDisplacement_[i];
    //x_[i] = startingPoint[i];
  }
  if (normType_ == Subproblem::L2_NORM) {
    Flow_[1] = 0;
    Fupp_[1] = trustRegionRadius_;
  }
  passParametersToSNOPTHandler(snoptHandler);
  integer Cold = 0, Basis = 1, Warm = 2;

  /// Print all the constraint such that I can (try) to see why it becomes infeasible.

  std::cout << "Flow\t Fupp\n";
  for (int i = 0; i < neF_; i++){
    std::cout << Flow_[i] << "\t" << Fupp_[i] << "\n";
  }
  std::cout << "\nxlow\t xupp\n";
  for (int i = 0; i < n_; i++){
    std::cout << xlow_[i] << "\t" << xupp_[i] << "\n";
  }


  vector<double> xsol;
  vector<double> fsol;
  snoptHandler.solve(Cold, xsol, fsol);
  lastLagrangeMultipliers = snoptHandler.getLagrangeMultipliers();

  fsol[0] = fsol[0]*scale;
  integer exitCode = snoptHandler.getExitCode();
  //std::cout << "snopt exitcode:  " << exitCode << "\n";
  if (exitCode == 40 || exitCode == 41){
    //std::cout << "snopt failed me. current point cannot be improved because of numerical difficulties \n";

    //std::cin.get();
  }
  if (exitCode != 40 && exitCode != 41 && exitCode != 1 && exitCode != 31 && exitCode != 3 && exitCode != 32){
    std::cout << "ExitCode is: " << exitCode << "\n";
    std::cin.get();
  }


  Eigen::VectorXd xvec(n_);
  for (int i = 1; i <= n_; ++i){
    xvec(i-1) = xsol[i-1];
  }
  if ( virtualSimulator.IsFeasiblePoint(xvec + y0) == false){
    std::cout << "output from snopt is infeasible. ExitCode was: " <<  exitCode << "\n";
    auto d = virtualSimulator.evaluateConstraints(xvec + y0);
    std::cout << d <<"\n";
  }


  if ( virtualSimulator.IsFeasiblePoint(xvec + y0) == false){
    std::cout << "Failed to find feasible points...\n";
  }


}
bool Subproblem::isPointFeasible(Eigen::VectorXd point) {
  return virtualSimulator.IsFeasiblePoint(point + y0);
}
}
}
