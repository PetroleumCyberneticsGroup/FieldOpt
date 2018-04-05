//
// Created by bellout on 2/10/18.
//

// -----------------------------------------------------------------
#include <iostream>
#include <iomanip>

// -----------------------------------------------------------------
#include "SNOPTSolver.h"
//#include "Subproblem.cpp"
//#include "testOne.h"

//enum PorosityModelType {
//  MATRIX_MODEL,
//  FRACTURE_MODEL
//};

// -----------------------------------------------------------------
namespace Optimization {
namespace Optimizers {

// -----------------------------------------------------------------
SNOPTSolver::SNOPTSolver(Settings::Optimizer *settings,
                         Case *wcpl_ch_case,
                         ::Reservoir::Grid::Grid *grid,
                         RICaseData *ricasedata) {

  // ---------------------------------------------------------------
  if (settings->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Init. SNOPTSolver.------ Constraint-handling." << endl;

  // ---------------------------------------------------------------
  ricasedata_ = ricasedata;
  grid_ = grid;

  size_t cellcount = ricasedata_->mainGrid()->cellCount();

//  auto startp = ricasedata_->mainGrid()->gridPointCoordinate(1,1,1);
//  auto endp = ricasedata_->mainGrid()->gridPointCoordinate(1,1,1);

  auto startp = ricasedata_->mainGrid()->cellCentroid(1);
  auto endp = ricasedata_->mainGrid()->cellCentroid(cellcount);
  cout << "x:" << startp.x() << " y:" << startp.y() << " z:" << startp.z() << endl;
  cout << "x:" << endp.x() << " y:" << endp.y() << " z:" << endp.z() << endl;

//  cout << "RICaseData_->gridCount()" << RICaseData_->gridCount() << endl;

//  auto gbb = ricasedata_
//      ->activeCellInfo(ricasedata_->PorosityModelTypeMATRIX_)
//      ->geometryBoundingBox();
//  cout << gbb.debugString().toStdString();

  // ---------------------------------------------------------------
  settings_ = settings;
  wcpl_ch_case_ = wcpl_ch_case;
  opt_prob_ = "Rosenbrock";
  // opt_prob_ = "Wplc_WL";

  // ---------------------------------------------------------------
  loadSNOPT();
  initSNOPTHandler();
  callSNOPT();
}

// -----------------------------------------------------------------
SNOPTSolver::SNOPTSolver(Settings::Optimizer *settings,
                         Case *base_case,
                         Model::Properties::VariablePropertyContainer *variables,
                         Reservoir::Grid::Grid *grid,
                         Logger *logger) {

  // ---------------------------------------------------------------
  if (settings->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Init. SNOPTSolver.-------" << endl;
  settings_ = settings;

  // ---------------------------------------------------------------
  Eigen::VectorXd vars = base_case->GetRealVarVector();

  // ---------------------------------------------------------------
  Optimization::Case *newCase = new Case(base_case);
  newCase->SetRealVarValues(vars);
  newCase->set_objective_function_value(std::numeric_limits<double>::max());
//  case_handler_->AddNewCase(newCase);

}

// -----------------------------------------------------------------
//SNOPTSolver::~SNOPTSolver(){}

// ---------------------------------------------------------
Optimizer::TerminationCondition SNOPTSolver::IsFinished() {
  Optimizer::TerminationCondition tc = NOT_FINISHED;
}

// -----------------------------------------------------------------
void SNOPTSolver::initSNOPTHandler() {

  // ---------------------------------------------------------------
  string prnt_file, smry_file, optn_file;

  string cdir = settings_->output_dir_.toStdString() + "/";
  optn_file = cdir + settings_->constraints()[0].thrdps_optn_file.toStdString() + ".opt.optn";
  smry_file = cdir + settings_->constraints()[0].thrdps_smry_file.toStdString() + ".opt.summ";
  prnt_file = cdir + settings_->constraints()[0].thrdps_prnt_file.toStdString() + ".opt.prnt";

  cout << "optn_file: " << optn_file << endl;
  cout << "smry_file: " << smry_file << endl;
  cout << "prnt_file: " << prnt_file << endl;

  // ---------------------------------------------------------------
  SNOPTHandler_ = new SNOPTHandler(prnt_file.c_str(),
                                   smry_file.c_str(),
                                   optn_file.c_str());

  // ---------------------------------------------------------------
  if (settings_->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Init. SNOPTHandler.--------" << endl;

}

// -------------------------------------------------------------------
int Rosenbrock_( integer    *Status, integer *n,    double x[],
                 integer    *needF,  integer *neF,  double F[],
                 integer    *needG,  integer *neG,  double G[],
                 char       *cu,     integer *lencu,
                 integer    iu[],    integer *leniu,
                 double     ru[],    integer *lenru ) {
  // -----------------------------------------------------------------
  // Rosenbrock testproblem
  F[0] = (1.0 - x[0]) * (1.0 - x[0]) + 100.0 * (x[1] - x[0] * x[0]) * (x[1] - x[0] * x[0]);
  F[1] = x[0] * x[0] + x[1] * x[1]; // nonlinear constraint: unit disk

  // dF/dx0, dF/dx1
  G[0] = -2.0 * (1.0 - x[0]) - 400.0 * x[0] * (x[1] - x[0] * x[0]);
  G[1] = 200.0 * (x[1] - x[0] * x[0]);

  // dC/dx0, dC/dx1
  G[2] = 2.0 * x[0];
  G[3] = 2.0 * x[1];

  // -----------------------------------------------------------------
  if(0) {
    cout << "[opt]Rosenbrock_.-------------" << endl;

    for (int i = 0; i < *n; i++) {
      cout << "x[" << i << "]:" << x[i] << " ";
    }
    cout << endl;

    cout << "F[0]: " << F[0] << endl;
    cout << "F[1]: " << F[1] << endl;
    cout << "G[0]: " << G[0] << endl;
    cout << "G[1]: " << G[1] << endl;
    cout << "G[2]: " << G[2] << endl;
    cout << "G[3]: " << G[3] << endl;
  }

  // -----------------------------------------------------------------
  // SOLUTION
  //  x[0]:  0.78641516  <- x0*
  //  x[1]:  0.61769831  <- x1*
  //  F[0]:  0.04567481  <- f*
  //  F[1]:  1.00000000  <- on constraint boundary
  //  G[0]: -0.19109155
  //  G[1]: -0.15009765
  //  G[2]:  1.57283031
  //  G[3]:  1.23539662
}

// -------------------------------------------------------------------
int SNOPTusrFG_( integer    *Status, integer *n,    double x[],
                 integer    *needF,  integer *neF,  double F[],
                 integer    *needG,  integer *neG,  double G[],
                 char       *cu,     integer *lencu,
                 integer    iu[],    integer *leniu,
                 double     ru[],    integer *lenru ) {

  int nf = *neF;

  // =================================================================
  // Computes the nonlinear objective and constraint terms for the
  // problem featured of interest. The problem is considered to be
  // written as:
  //
  //       Minimize     Fobj(x)
  //          x
  //
  //    subject to:
  //
  //        bounds      l_x <=   x  <= u_x
  //   constraints      l_F <= F(x) <= u_F
  //
  // The triples (g(k),iGfun(k),jGvar(k)), k = 1:neG, define
  // the sparsity pattern and values of the nonlinear elements
  // of the Jacobian.
  // =================================================================

  // ADGPRS LEGACY (keep for ref.)
  //  OptimizationData& optdata =  OptimizationData::reference();
  //
  //  if (( optdata.numberOfSimulations >= optdata.maxNumberOfSimulations ) &&
  //      ( optdata.maxNumberOfSimulations != 0 ))
  //  {
  //    *Status = -2;
  //    return 0;
  //  }

  // -----------------------------------------------------------------
  // # of constraints; neF is the total number of constraints + obj.
  //  int m = *neF - 1 - optdata.numberOfLinearConstraints;
  int m = (int)*neF - 1;

  // -----------------------------------------------------------------
  // Function call from SNOPT for objective/gradient computation
  if (*needF > 0) {

    // ADGPRS LEGACY (keep for ref.)
    // The value of the objective goes to the first entry of F
    //    if (FAILED ==
    //        optdata.pOptimizationProblem->eval_f(*n, x, true, F[0])) {
    //      *Status = -1;
    //      return 0;
    //    }

    // the values of the constraints follow that of the objective
    if (m) {

      // ADGPRS LEGACY (keep for ref.)
      // optdata.pOptimizationProblem->eval_g(*n, x, false, m, &F[1]);
    }
  }

  // -----------------------------------------------------------------
  // If the values for the constraints are desired
  if (*needG > 0) {

    // ADGPRS LEGACY (keep for ref.)
    // We have as many derivatives as the number of the controls, n
    //    optdata.pOptimizationProblem->eval_grad_f(*n, x, false, G);

    // Derivatives of constraints:
    if (m) {

      // ADGPRS LEGACY (keep for ref.)
      // G[1] = 100*4*x1*x1*x1;//-4*(x2-0.7);
      // optdata.pOptimizationProblem->eval_jac_g(*n, x, false, m,
      //                                          *neG, 0, 0, &G[*n]);

    }

  }

  return 0;
}

// -----------------------------------------------------------------
void SNOPTSolver::callSNOPT() {

  // ---------------------------------------------------------------
  // Set problem dimensions
  if(opt_prob_ == "Rosenbrock") {
    n_ = 2;
    m_ = 1;        // # of nonlinear constraints

  } else if(opt_prob_ == "Wplc_WL") {
    n_ = (int)wcpl_ch_case_->GetRealWSplineVarVector().rows();
    m_ = 1;        // # of nonlinear constraints
  }

  // ---------------------------------------------------------------
  // Set problem dimensions
  neF_ = m_ + 1; // # of elements in F vector
  lenA_ = 0;     // # of linear constraints

  nFnames_ = 1;
  nxnames_ = 1;

  lenG_    = n_  // derivatives of the objective
      + m_ * n_; // derivatives of the constraints

  objRow_ = 0;   // row # of objective
  objAdd_ = 0;   // add nothing to objective

  iAfun_ = nullptr; // no linear constraints (introduce later)
  jAvar_ = nullptr;
  A_     = nullptr;

  iGfun_ = new integer[lenG_];
  jGvar_ = new integer[lenG_];

  x_ = new double[n_];    // controls
  xlow_ = new double[n_]; // lower bounds
  xupp_ = new double[n_]; // upper bounds
  xmul_ = new double[n_]; // initial guess Lagrange multipliers

  xstate_ = new integer[n_]; // state of variables

  F_ = new double[neF_];
  Flow_ = new double[neF_];
  Fupp_ = new double[neF_];
  Fmul_ = new double[neF_];
  Fstate_ = new integer[neF_];

  xnames_ = new char[nxnames_ * 8];
  Fnames_ = new char[nxnames_ * 8];


  // ---------------------------------------------------------------
  // Set F bounds (objective + nonlinear constraint)
  if(opt_prob_.compare("Rosenbrock")) {

    // -------------------------------------------------------------
    // Objective bounds Rosenbrock test case
    Flow_[0] = -infinity_;
    Fupp_[0] = infinity_;

    // Constraint bounds
    Flow_[1] = 0;
    Fupp_[1] = 1;

    // Fill in lower & upper bounds for x
    for (int i = 0; i < n_; i++) {
      xlow_[i] = -infinity_;
      xupp_[i] = infinity_;
    }

    // Initial point
    for (int i = 0; i < n_; i++)
      x_[i] = 0.0e1;

  } else if(opt_prob_.compare("Wplc_WL")) {

    // -------------------------------------------------------------
    // Later: Set F bounds according to uuid
    Flow_[0] = -infinity_;
    Fupp_[0] = infinity_;

    // Constraint bounds => lw
    Flow_[1] = settings_->constraints()[0].min_length;
    Fupp_[1] = settings_->constraints()[0].max_length;

    // Constraint bounds => iwd
//    Flow_[2] = settings_->constraints()[0].min_distance;
//    Fupp_[2] = infinity_;

    // Later: Set bounds for x according to spline var ordering
    for (int i = 0; i < n_; i++) {
      xlow_[i] = -infinity_;
      xupp_[i] = infinity_;
    }

    // Initial point
    for (int i = 0; i < n_; i++)
      x_[i] = 0.0e1;

    SNOPTHandler_->setParameter((char*)"Minimize");
  }

  // ---------------------------------------------------------------
  // If we provide an initial guess then the states should be zero
  for (int i = 0; i < n_; i++)
    xstate_[i] = 0;

  // ---------------------------------------------------------------
  // Initial guess for the controls
  bool desire_values_for_x      = true;
  bool desire_values_for_dual   = false;
  bool desire_values_for_lambda = false;

  // ---------------------------------------------------------------
  // Fmul is the vector with the estimation of the Lagrange
  // Multipliers. It will be always zero except in very rare cases of
  // benchmarking performance with them set to some initial guess.
  for (int i = 0; i < neF_; i++) {
    Fmul_[i] = 0;
  }

  // ---------------------------------------------------------------
  // The nonzero structure of the Jacobian
  neG_ = lenG_;
  neA_ = lenA_;

  // ---------------------------------------------------------------
  // Fill in first the objective
  for (int i = 0; i < n_; i++)
  {
    iGfun_[i] = 0;
    jGvar_[i] = i;

    // ilc: loop over # of constraints
    for (int ilc = 0; ilc < lenA_; ilc++)
    {
      iAfun_[i + ilc * n_] = m_ + 1 + ilc; // <- sure about this m_?
      jAvar_[i + ilc * n_] = i;
      A_[i + ilc * n_] = 0; // <- coeff. of linear constraints

      // A_[i + ilc * n_] = ((ConstraintFunctional*)func_list[ilc])->ad_value.derivative(i);
      // ad_value.derivative(i) =>
      // get the derivative at column i (0 is returned if it does not exist)
    }
  }

  // ---------------------------------------------------------------
  if (m_ != 0)
  {
    // Then fill in the constraints
    for (int j = 1; j <= m_; j++)
    {
      for (int i = 0; i < n_; i++)
      {
        iGfun_[i + j * n_] = j;
        jGvar_[i + j * n_] = i;
      }
    }
  }

  // ---------------------------------------------------------------
  // dbg
  Matrix<integer, 1, Dynamic> XiAfun_, XiAvar_;
  XiAvar_.fill((integer)jAvar_);
  XiAfun_.fill((integer)iAfun_);

  Matrix<integer, 1, Dynamic> XiGfun_, XiGvar_;
  XiGvar_.fill((integer)jGvar_);
  XiGfun_.fill((integer)iGfun_);

  cout << "XiAfun_.rows(): " << XiAfun_.rows() << endl;
  cout << "XiAfun_: " << XiAfun_ << endl;

  cout << "XiAvar_.rows(): " << XiAvar_.rows() << endl;
  cout << "XiGfun_.rows(): " << XiGfun_.rows() << endl;
  cout << "XiGvar_.rows(): " << XiGvar_.rows() << endl;

  // ---------------------------------------------------------------
  // Set SNOPT dims
  SNOPTHandler_->setProblemSize( n_, neF_ );
  SNOPTHandler_->setObjective  ( objRow_ );
  SNOPTHandler_->setA          ( lenA_, iAfun_, jAvar_, A_ );
  SNOPTHandler_->setG          ( lenG_, iGfun_, jGvar_ );
  SNOPTHandler_->setX          ( x_, xlow_, xupp_, xmul_, xstate_ );
  SNOPTHandler_->setF          ( F_, Flow_, Fupp_, Fmul_, Fstate_ );
  SNOPTHandler_->setXNames     ( xnames_, nxnames_ );
  SNOPTHandler_->setFNames     ( Fnames_, nFnames_ );
  SNOPTHandler_->setNeA        ( neA_ );
  SNOPTHandler_->setNeG        ( neG_ );

  // ---------------------------------------------------------------
  // Sets the usrfun that supplies G and F.
  if(opt_prob_ == "Rosenbrock") {

    SNOPTHandler_->setUserFun( Rosenbrock_ );
    SNOPTHandler_->setProbName( "Rosenbrock" );

  } else if(opt_prob_ == "Wplc_WL" ||
      opt_prob_ == "Wplc_IWD" ||
      opt_prob_ == "Wplc_WL+IWD" ||
      opt_prob_ == "Wplc_WL+IWD+BND" ) {

    SNOPTHandler_->setUserFun( SNOPTusrFG_ );
    SNOPTHandler_->setProbName( opt_prob_.c_str() );

  }

  SNOPTHandler_->setParameter((char*)"Minimize");
  if (!SNOPTHandler_->has_snopt_option_file)
    setOptionsForSNOPT();

  // ---------------------------------------------------------------
  integer Cold = 0, Basis = 1, Warm = 2;

  vector<double> xsol;
  vector<double> fsol;
  SNOPTHandler_->solve( Cold, xsol, fsol);

  // ---------------------------------------------------------------
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
  delete[] Fstate_;

  delete[] xnames_;
  delete[] Fnames_;
}



// -----------------------------------------------------------------
//void SNOPTSolver::callSNOPT() {
//
////  SNOPTHandler snoptHandler = initSNOPTHandler();
////  setOptionsForSNOPT(snoptHandler);
//
//  int i;
//  // Allocate and initialize;
//  int n;
//  int m;
//  int nnz_jac_g;
//  int nnz_h_lag;
////  Ipopt::TNLP::IndexStyleEnum index_style;
//
//  n = 2;
//  m = 1; // number of nonlinear constraints
//  integer neF     = m + 1;
//  integer lenA    = 4;
//  integer nxnames = 1;
//  integer nFnames = 1;
//
//  // the dimension of the arrays iGfun jGvar
//  integer lenG    = n         // derivatives of the objective
//      + m * n;  // derivatives of the constraints
//
//  integer ObjRow  = 0;
//
//  // we add nothing to the objective for output purposes
//  double ObjAdd   = 0;
//
//  // we do not have linear constraints
//  integer* iAfun  = NULL;
//  integer* jAvar  = NULL;
//  double* A       = NULL;
//
//  // define linear constraints
////  if ( numberOfLinearConstraints )
////  {
////    neF  += numberOfLinearConstraints;
////    lenA += numberOfLinearConstraints * n;
////    iAfun = new integer[lenA];
////    jAvar = new integer[lenA];
////    A     = new double[lenA];
////  }
//
//  // the indices (row, column) of the Jacobian (objective,
//  // constraints) combined
//  neF+=2; // Two linear constraints
//  //lenG = 3;
//  lenG--; // We only have 1 derivative for the nonlinear
//  // constraint (it doesn't depend upon both variables)
//
//  integer* iGfun  = new integer[lenG];
//  integer* jGvar  = new integer[lenG];
//
//  iAfun = new integer[lenA];
//  jAvar = new integer[lenA];
//  A     = new double[lenA];
//  iAfun[0] = 1;
//  jAvar[0] = 0;
//  iAfun[1] = 1;
//  jAvar[1] = 1;
//  iAfun[2] = 2;
//  jAvar[2] = 0;
//  iAfun[3] = 2;
//  jAvar[3] = 1;
//  A[0] = 1.0;
//  A[1] = 1.2;
//  A[2] = 0.9;
//  A[3] = 3.0;
//  // the controls
//  double* x       = new double[n];
//
//
//  // controls lower and upper bounds
//  double* xlow    = new double[n];
//  double* xupp    = new double[n];
//
//  // the initial guess for Lagrange multipliers
//  double* xmul    = new double[n];
//
//  // the state of the variables (whether the optimal is likely to be on
//  // the boundary or not)
//  integer* xstate = new integer[n];
//
//  double* F       = new double[neF];
//  double* Flow    = new double[neF];
//  double* Fupp    = new double[neF];
//  double* Fmul    = new double[neF];
//  integer* Fstate = new integer[neF];
//
//  char* xnames    = new char[nxnames * 8];
//  char* Fnames    = new char[nFnames * 8];
//
//  //==================================================================
//  // Computes the nonlinear objective and constraint terms for the
//  // problem featured of interest. The problem is considered to be
//  // written as:
//  //
//  //       Minimize     Fobj(x)
//  //          x
//  //
//  //    subject to:
//  //
//  //        bounds      l_x <=   x  <= u_x
//  //   constraints      l_F <= F(x) <= u_F
//  //
//  // The triples (g(k),iGfun(k),jGvar(k)), k = 1:neG, define
//  // the sparsity pattern and values of the nonlinear elements
//  // of the Jacobian.
//  //==================================================================
//
//  // Set the upper and lower bounds:
//  // ===============================
//
//  // -infinity < Objective < infinity
//
//  // this is the value SNOPT considers as infinity
//  const double infinity = 1e20;
//  Flow[0] = -infinity;    Fupp[0] = infinity;
//  Flow[1] = -2;           Fupp[1] = 4;
//  Flow[2] = -3;           Fupp[2] = 10;
//  Flow[3] = 0;           Fupp[3] = 1;
//
//
//  xlow[0] = -2;     xupp[0] = 2;
//  xlow[1] = -4;     xupp[1] = 4;
//
//  integer a = 0;
//
//  iGfun[a] = (integer)0;
//  jGvar[a] = (integer)0;
//  a ++;
//
//  iGfun[a] = (integer)0;
//  jGvar[a] = (integer)1;
//  a ++;
//
//  iGfun[a] = (integer)3;
//  jGvar[a] = (integer)0;
//  //a ++;
//
//  //iGfun[a] = (integer)3;
//  //jGvar[a] = (integer)1;
//
////  OptimizationProblem->get_bounds_info(n, xlow, xupp,
////                                       m + numberOfLinearConstraints,
////                                       &Flow[1], &Fupp[1]);
//
//
//
//  // When we have an initial guess the states should be zero
//  for (i = 0; i < n; i++){
//    Fstate[i] = 0;
//    xstate[i] = 0;
//    x[i] = 0.0;
//
//
//    //trying:
//    xmul[i] = 0;
//  }
//
//  for (int h = 0; h < neF; h++){
//    F[h] = 0.0;
//  }
//
//
//  // initial guess for the controls
//  bool desire_values_for_x      = true;
//  bool desire_values_for_dual   = false;
//  bool desire_values_for_lambda = false;
//
//  // ipopts initial values for bound multipliers z_L, z_U
//  // which are not needed here and are not set anyway
//  double* z_L    = 0;
//  double* z_U    = 0;
//  double* lambda = 0;
//
////  OptimizationProblem->get_starting_point(n, desire_values_for_x, x,
////                                          desire_values_for_dual, z_L, z_U,
////                                          m, desire_values_for_lambda, lambda);
//
//  // neA and neG must be defined when using derivatives
//  integer neA, neG;
//
//  // Fmul is the vector with the estimation of the Lagrange
//  // Multipliers. It will be always zero except in very rare cases of
//  // benchmarking performance with them set to some initial guess.
//  for (i = 0; i < neF; i++)
//    Fmul[i] = 0.0;
//
//  // This is the nonzero structure of the Jacobian
//
//  neG = lenG;
//  neA = lenA;
//
////  FunctionList func_list;
////  if ( optdata.numberOfLinearConstraints )
////  {
////    for (int ic = 0; ic < optdata.vConstraints.size(); ic++)
////      if ( !((ConstraintFunctional*)optdata.vConstraints[ic])->is_nonlinear )
////        func_list.push_back(optdata.vConstraints[ic]);
////
////    rControlCenter_.numberOfTimeSteps = rControlCenter_.numberOfInputRegions;
////    for (int ireg = 0; ireg < rControlCenter_.numberOfInputRegions; ireg++)
////    {
////      rControlCenter_.vWhichInputRegionPerTimeStep.push_back(ireg);
////      rControlCenter_.vvWhichTimeStepsPerInputRegion.push_back(vector<int>(1, ireg));
////    }
////
////    vector<double> vvalues;
////    func_eval.evaluateFunctionList(func_list, vvalues);
////  }
//
//  // first the objective
////  for (i = 0; i < n; i++)
////  {
////    iGfun[i] = 0;
////    jGvar[i] = i;
////
////    for (int ilc = 0; ilc < optdata.numberOfLinearConstraints; ilc++)
////    {
////      iAfun[i + ilc * n] = m + 1 + ilc;
////      jAvar[i + ilc * n] = i;
////      A[i + ilc * n] = ((ConstraintFunctional*)func_list[ilc])->ad_value.derivative(i);
////    }
////  }
////
////  if (m != 0)
////  {
////    // and then the constraints
////    for (int j = 1; j <= m; j++)
////    {
////      for (i = 0; i < n; i++)
////      {
////        iGfun[i + j * n] = j;
////        jGvar[i + j * n] = i;
////      }
////    }
////  }
//
//  // As SNOPTOptimizer::computeJac() is not used to compute the
//  // derivatives,  neA and neG must be set here to match the
//  // arrays  A, iAfun, jAvar, iGfun, jGvar.
//  // Load the data for ToyProb ...
//
//
//  SNOPTHandler_->setProblemSize( n, neF );
//  SNOPTHandler_->setObjective  ( ObjRow );
//  SNOPTHandler_->setA          ( lenA, iAfun, jAvar, A );
//  SNOPTHandler_->setG          ( lenG, iGfun, jGvar );
//  SNOPTHandler_->setX          ( x, xlow, xupp, xmul, xstate );
//  SNOPTHandler_->setF          ( F, Flow, Fupp, Fmul, Fstate );
//  SNOPTHandler_->setXNames     ( xnames, nxnames );
//  SNOPTHandler_->setFNames     ( Fnames, nFnames );
//  SNOPTHandler_->setNeA         ( neA );
//  SNOPTHandler_->setNeG         ( neG );
//
//  // Sets the usrfun that supplies G and F.
//  SNOPTHandler_->setUserFun( SNOPTusrFG_ );
//
//  SNOPTHandler_->setProbName( "SNOPTSolver" );
//
//  if (!SNOPTHandler_->has_snopt_option_file)
//    setOptionsForSNOPT(snoptHandler);
//
//  // Minimize/maximize option
////  if (((ObjectiveFunctional*)optdata.vObjectives[OBJ])->is_minimized)
////    optimizeWithSNOPT.setParameter((char*)"Minimize");
////  else
//  SNOPTHandler_->setParameter((char*)"Maximize");
//
//  integer Cold = 0, Basis = 1, Warm = 2;
//
//  vector<double> xsol;
//  vector<double> fsol;
//  SNOPTHandler_->solve( Cold, xsol, fsol);
//
//
//  cout << "xsol: " << endl;
//  for (int j = 0; j < n; j++){
//    cout << xsol[j] << endl;
//  }
//  cout << endl <<"Objective values:" << endl;
//  for (int j = 0; j < neF; j++){
//    cout << fsol[j] << endl;
//  }
//
//
////  ParameterMapping& maps = ParameterMapping::reference();
////  if (maps.isReducingControlSpaceViaMultiScale)
////    maps.objectiveFinalValue = fsol[0];
//
////  finalReport("    Optimal objective value: ", xsol, fsol[0]);
//
//  delete[] iGfun;
//  delete[] jGvar;
//
//
//
//  delete[] x;
//  delete[] xlow;
//  delete[] xupp;
//  delete[] xmul;
//  delete[] xstate;
//
//  delete[] F;
//  delete[] Flow;
//  delete[] Fupp;
//  delete[] Fmul;
//  delete[] Fstate;
//
//  delete[] xnames;
//  delete[] Fnames;
//
////  optdata.pOptimizationProblem = NULL;
//}



// -----------------------------------------------------------------
void SNOPTSolver::setOptionsForSNOPT() {

  if (settings_->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Set options for SNOPT.---" << endl;

  //SNOPTHandler_->setParameter("Backup basis file              0");
//  SNOPTHandler_->setRealParameter("Central difference interval", 2 * derivativeRelativePerturbation);

  //SNOPTHandler_->setIntParameter("Check frequency",           60);
  //SNOPTHandler_->setParameter("Cold Start                     Cold");

  //SNOPTHandler_->setParameter("Crash option                   3");
  //SNOPTHandler_->setParameter("Crash tolerance                0.1");
  //SNOPTHandler_->setParameter("Derivative level               3");

//  if ( (optdata.optimizationType == HISTORY_MATCHING) || hasNonderivativeLinesearch )
//    SNOPTHandler_->setParameter((char*)"Nonderivative linesearch");
//  else
  //SNOPTHandler_->setParameter((char*)"Derivative linesearch");
//  SNOPTHandler_->setIntParameter("Derivative option", 1);

//  SNOPTHandler_->setRealParameter("Difference interval", optdata.derivativeRelativePerturbation);

  //SNOPTHandler_->setParameter("Dump file                      0");
  //SNOPTHandler_->setParameter("Elastic weight                 1.0e+4");
  //SNOPTHandler_->setParameter("Expand frequency               10000");
  //SNOPTHandler_->setParameter("Factorization frequency        50");
  //SNOPTHandler_->setRealParameter("Function precision", sim_data.tuningParam.tstep.minDeltat);
  //SNOPTHandler_->setParameter("Hessian full memory");
  //SNOPTHandler_->setParameter("Hessian limited memory");

//  SNOPTHandler_->setIntParameter("Hessian frequency", optdata.frequencyToResetHessian);
  //SNOPTHandler_->setIntParameter("Hessian updates", 0);
  //SNOPTHandler_->setIntParameter("Hessian flush", 1);  // Does NOT work in the current version of SNOPT!!!

  //SNOPTHandler_->setParameter("Insert file                    0");
//  SNOPTHandler_->setRealParameter("Infinite bound", optdata.defaultControlUpperBound);

  //SNOPTHandler_->setParameter("Iterations limit");
  //SNOPTHandler_->setRealParameter("Linesearch tolerance",0.9);
  //SNOPTHandler_->setParameter("Load file                      0");
  //SNOPTHandler_->setParameter("Log frequency                  100");
  //SNOPTHandler_->setParameter("LU factor tolerance            3.99");
  //SNOPTHandler_->setParameter("LU update tolerance            3.99");
  //SNOPTHandler_->setParameter("LU partial pivoting");
  //SNOPTHandler_->setParameter("LU density tolerance           0.6");
  //SNOPTHandler_->setParameter("LU singularity tolerance       3.2e-11");

  //target nonlinear constraint violation
//  SNOPTHandler_->setRealParameter("Major feasibility tolerance", optdata.constraintTolerance);
//  SNOPTHandler_->setIntParameter("Major Iterations Limit", optdata.maxNumberOfIterations);

  //target complementarity gap
//  SNOPTHandler_->setRealParameter("Major optimality tolerance", optdata.convergenceTolerance);

  //SNOPTHandler_->setParameter("Major Print level  11111"); //  000001"
//  SNOPTHandler_->setRealParameter("Major step limit", optdata.majorStepLimit);
  //SNOPTHandler_->setIntParameter("Minor iterations limit", 200); // 200

  //for satisfying the QP bounds
//  SNOPTHandler_->setRealParameter("Minor feasibility tolerance", optdata.constraintTolerance);
  SNOPTHandler_->setIntParameter("Minor print level", 10);
  //SNOPTHandler_->setParameter("New basis file                 0");
  //SNOPTHandler_->setParameter("New superbasics limit          99");
  //SNOPTHandler_->setParameter("Objective Row");
  //SNOPTHandler_->setParameter("Old basis file                 0");
  //SNOPTHandler_->setParameter("Partial price                  1");
  //SNOPTHandler_->setParameter("Pivot tolerance                3.7e-11");
  //SNOPTHandler_->setParameter("Print frequency                100");
  //SNOPTHandler_->setParameter("Proximal point method          1");
  //SNOPTHandler_->setParameter("QPSolver Cholesky");
  //SNOPTHandler_->setParameter("Reduced Hessian dimension");
  //SNOPTHandler_->setParameter("Save frequency                 100");
  SNOPTHandler_->setIntParameter("Scale option", 1);
  //SNOPTHandler_->setParameter("Scale tolerance                0.9");
  SNOPTHandler_->setParameter((char*)"Scale Print");
  SNOPTHandler_->setParameter((char*)"Solution  Yes");
  //SNOPTHandler_->setParameter("Start Objective Check at Column 1");
  //SNOPTHandler_->setParameter("Start Constraint Check at Column 1");
  //SNOPTHandler_->setParameter("Stop Objective Check at Column");
  //SNOPTHandler_->setParameter("Stop Constraint Check at Column");
  //SNOPTHandler_->setParameter("Sticky parameters               No");
  //SNOPTHandler_->setParameter("Summary frequency               100");
  //SNOPTHandler_->setParameter("Superbasics limit");
  //SNOPTHandler_->setParameter("Suppress parameters");
  //SNOPTHandler_->setParameter((char*)"System information  No");
  //SNOPTHandler_->setParameter("Timing level                    3");
  //SNOPTHandler_->setRealParameter("Unbounded objective value   1.0e+15");
  //SNOPTHandler_->setParameter("Unbounded step size             1.0e+18");
  //SNOPTHandler_->setIntParameter("Verify level", -1); //-1
  //SNOPTHandler_->setRealParameter("Violation limit", 1e-8); //1e-8

//  if (settings_->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
//    cout << "[opt]Set options for SNOPT.---" << endl;

}



// ------------------------------------------------------------------------------
/*****************************************************
ADGPRS, version 1.0, Copyright (c) 2010-2015 SUPRI-B
Author(s): Oleg Volkov          (ovolkov@stanford.edu)
           Vladislav Bukshtynov (bukshtu@stanford.edu)
******************************************************/

bool SNOPTSolver::loadSNOPT(const string libname)
{

//#ifdef NDEBUG
  if (LSL_isSNOPTLoaded()) {
    printf("\x1b[33mSnopt loaded.\n\x1b[0m");
    return true;
  }

  char buf[256];
  int rc;
  if ( libname.empty() ) {
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

}
}
