//
// Created by bellout on 2/10/18.
//

#include "SNOPTSolver.h"
#include <iostream>
#include <iomanip>

namespace Optimization {
namespace Optimizers {

SNOPTSolver::SNOPTSolver(Settings::Optimizer *settings,
                         Case *base_case,
                         Model::Properties::VariablePropertyContainer *variables,
                         Reservoir::Grid::Grid *grid,
                         Logger *logger)
    : Optimizer(settings, base_case, variables, grid, logger) {

  if (settings->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Init. SNOPTSolver.-------" << endl;

  settings_ = settings;
  loadSNOPT();
  callSNOPT();
}

SNOPTSolver::~SNOPTSolver(){}


SNOPTHandler SNOPTSolver::initSNOPTHandler(){
  string prnt_file, smry_file, optn_file;
  optn_file = settings_->parameters().thrdps_optn_file.toStdString() + ".opt.optn";
  smry_file = settings_->parameters().thrdps_smry_file.toStdString() + ".opt.summ";
  prnt_file = settings_->parameters().thrdps_prnt_file.toStdString() + ".opt.prnt";

  SNOPTHandler snoptHandler(prnt_file.c_str(),
                            smry_file.c_str(),
                            optn_file.c_str());
  if (settings_->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Init. SNOPTHandler.------" << endl;

  return snoptHandler;
}



int SNOPTusrFG_( integer    *Status, integer *n,    double x[],
integer    *needF,  integer *neF,  double F[],
    integer    *needG,  integer *neG,  double G[],
char       *cu,     integer *lencu,
integer    iu[],    integer *leniu,
double     ru[],    integer *lenru )
{

    int  nf = *neF;
    //double x2 = x[1];
    //cout << x1 << "\t" << x2 << endl;
    cout << "[SNOPTusrFG_] \t The x vector is: " <<  "\t ";
    for (int i = 0; i < *n; i++ ){
        cout << x[i] << "\t";
    }
    cout << endl;


     //==================================================================
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
//==================================================================

//  OptimizationData& optdata =  OptimizationData::reference();
//
//  if (( optdata.numberOfSimulations >= optdata.maxNumberOfSimulations ) &&
//      ( optdata.maxNumberOfSimulations != 0 ))
//  {
//    *Status = -2;
//    return 0;
//  }

// number of constraints; neF is the total number of constraints
// plus the objective
//  int m = *neF - 1 - optdata.numberOfLinearConstraints;
int m = *neF - 1;

// If the values for the objective and/or the constraints are desired
if ( *needF > 0)
{
    F[0] = - (x[0]-1.2)*(x[0]-1.2) - (x[1]-3.1)*(x[1]-3.1);

// the value of the objective goes to the first entry of F
//    if (FAILED == optdata.pOptimizationProblem->eval_f(*n, x, true, F[0]))
//    {
//      *Status = -1;
//      return 0;
//    }

// the values of the constraints follow that of the objective
if ( m ){

//      optdata.pOptimizationProblem->eval_g(*n, x, false, m, &F[1]);
}
}

if ( *needG > 0)
{
    G[0] = -2*(x[0]-1.2);
    G[1] = -2*(x[1]-3.1);

// we have as many derivatives as the number of the controls, n
//    optdata.pOptimizationProblem->eval_grad_f(*n, x, false, G);

// and the derivatives of the constraints follow
if ( m ){
//    G[1] = 100*4*x1*x1*x1;//-4*(x2-0.7);
//      optdata.pOptimizationProblem->eval_jac_g(*n, x, false, m, *neG, 0, 0, &G[*n]);
}

}

return 0;
}



void SNOPTSolver::callSNOPT()
{
  SNOPTHandler snoptHandler = initSNOPTHandler();
  setOptionsForSNOPT(snoptHandler);

  int i;
  // Allocate and initialize;
  int n;
  int m;
  int nnz_jac_g;
  int nnz_h_lag;
//  Ipopt::TNLP::IndexStyleEnum index_style;

    n = 2;
  m = 0; // number of nonlinear constraints
  integer neF     = m + 1;
  integer lenA    = 0;
  integer nxnames = 1;
  integer nFnames = 1;

  // the dimension of the arrays iGfun jGvar
  integer lenG    = n         // derivatives of the objective
      + m * n;  // derivatives of the constraints

  integer ObjRow  = 0;

  // we add nothing to the objective for output purposes
  double ObjAdd   = 0;

  // we do not have linear constraints
  integer* iAfun  = NULL;
  integer* jAvar  = NULL;
  double* A       = NULL;

  // define linear constraints
//  if ( numberOfLinearConstraints )
//  {
//    neF  += numberOfLinearConstraints;
//    lenA += numberOfLinearConstraints * n;
//    iAfun = new integer[lenA];
//    jAvar = new integer[lenA];
//    A     = new double[lenA];
//  }

  // the indices (row, column) of the Jacobian (objective,
  // constraints) combined
  integer* iGfun  = new integer[lenG];
  integer* jGvar  = new integer[lenG];

  // the controls
  double* x       = new double[n];


    // controls lower and upper bounds
  double* xlow    = new double[n];
  double* xupp    = new double[n];

  // the initial guess for Lagrange multipliers
  double* xmul    = new double[n];

  // the state of the variables (whether the optimal is likely to be on
  // the boundary or not)
  integer* xstate = new integer[n];

  double* F       = new double[neF];
  double* Flow    = new double[neF];
  double* Fupp    = new double[neF];
  double* Fmul    = new double[neF];
  integer* Fstate = new integer[neF];

  char* xnames    = new char[nxnames * 8];
  char* Fnames    = new char[nFnames * 8];

  //==================================================================
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
  //==================================================================

  // Set the upper and lower bounds:
  // ===============================

  // -infinity < Objective < infinity

  // this is the value SNOPT considers as infinity
  const double infinity = 1e20;
  Flow[0] = -infinity;
  Fupp[0] = infinity;

    xlow[0] = -2;
    xupp[0] = 2;
    xlow[1] = -4;
    xupp[1] = 4;

    iGfun[0] = 0;
    jGvar[0] = 0;

    iGfun[1] = 1;
    jGvar[1] = 0;

//  OptimizationProblem->get_bounds_info(n, xlow, xupp,
//                                       m + numberOfLinearConstraints,
//                                       &Flow[1], &Fupp[1]);



  // When we have an initial guess the states should be zero
  for (i = 0; i < n; i++){
      Fstate[i] = 0;
      xstate[i] = 0;
      x[i] = 0.0;
      F[i] = 0.0;
  }


  // initial guess for the controls
  bool desire_values_for_x      = true;
  bool desire_values_for_dual   = false;
  bool desire_values_for_lambda = false;

  // ipopts initial values for bound multipliers z_L, z_U
  // which are not needed here and are not set anyway
  double* z_L    = 0;
  double* z_U    = 0;
  double* lambda = 0;

//  OptimizationProblem->get_starting_point(n, desire_values_for_x, x,
//                                          desire_values_for_dual, z_L, z_U,
//                                          m, desire_values_for_lambda, lambda);

  // neA and neG must be defined when using derivatives
  integer neA, neG;

  // Fmul is the vector with the estimation of the Lagrange
  // Multipliers. It will be always zero except in very rare cases of
  // benchmarking performance with them set to some initial guess.
  for (i = 0; i < neF; i++)
    Fmul[i] = 0;

  // This is the nonzero structure of the Jacobian

  neG = lenG;
  neA = lenA;

//  FunctionList func_list;
//  if ( optdata.numberOfLinearConstraints )
//  {
//    for (int ic = 0; ic < optdata.vConstraints.size(); ic++)
//      if ( !((ConstraintFunctional*)optdata.vConstraints[ic])->is_nonlinear )
//        func_list.push_back(optdata.vConstraints[ic]);
//
//    rControlCenter_.numberOfTimeSteps = rControlCenter_.numberOfInputRegions;
//    for (int ireg = 0; ireg < rControlCenter_.numberOfInputRegions; ireg++)
//    {
//      rControlCenter_.vWhichInputRegionPerTimeStep.push_back(ireg);
//      rControlCenter_.vvWhichTimeStepsPerInputRegion.push_back(vector<int>(1, ireg));
//    }
//
//    vector<double> vvalues;
//    func_eval.evaluateFunctionList(func_list, vvalues);
//  }

  // first the objective
//  for (i = 0; i < n; i++)
//  {
//    iGfun[i] = 0;
//    jGvar[i] = i;
//
//    for (int ilc = 0; ilc < optdata.numberOfLinearConstraints; ilc++)
//    {
//      iAfun[i + ilc * n] = m + 1 + ilc;
//      jAvar[i + ilc * n] = i;
//      A[i + ilc * n] = ((ConstraintFunctional*)func_list[ilc])->ad_value.derivative(i);
//    }
//  }
//
//  if (m != 0)
//  {
//    // and then the constraints
//    for (int j = 1; j <= m; j++)
//    {
//      for (i = 0; i < n; i++)
//      {
//        iGfun[i + j * n] = j;
//        jGvar[i + j * n] = i;
//      }
//    }
//  }

  // As SNOPTOptimizer::computeJac() is not used to compute the
  // derivatives,  neA and neG must be set here to match the
  // arrays  A, iAfun, jAvar, iGfun, jGvar.
  // Load the data for ToyProb ...


  snoptHandler.setProblemSize( n, neF );
  snoptHandler.setObjective  ( ObjRow );
  snoptHandler.setA          ( lenA, iAfun, jAvar, A );
  snoptHandler.setG          ( lenG, iGfun, jGvar );
  snoptHandler.setX          ( x, xlow, xupp, xmul, xstate );
  snoptHandler.setF          ( F, Flow, Fupp, Fmul, Fstate );
  snoptHandler.setXNames     ( xnames, nxnames );
  snoptHandler.setFNames     ( Fnames, nFnames );
  snoptHandler.setNeA         ( neA );
  snoptHandler.setNeG         ( neG );

  // Sets the usrfun that supplies G and F.
  snoptHandler.setUserFun( SNOPTusrFG_ );

  snoptHandler.setProbName( "SNOPTSolver" );

  if (!snoptHandler.has_snopt_option_file)
    setOptionsForSNOPT(snoptHandler);

  // Minimize/maximize option
//  if (((ObjectiveFunctional*)optdata.vObjectives[OBJ])->is_minimized)
//    optimizeWithSNOPT.setParameter((char*)"Minimize");
//  else
  snoptHandler.setParameter((char*)"Maximize");

  integer Cold = 0, Basis = 1, Warm = 2;

  vector<double> xsol;
  vector<double> fsol;
  snoptHandler.solve( Cold, xsol, fsol);


    cout << "xsol: " << endl;
    for (int j = 0; j < n; j++){
        cout << xsol[j] << endl;
    }
    cout << endl <<"Objective values:" << endl;
    for (int j = 0; j < neF; j++){
        cout << fsol[j] << endl;
    }


//  ParameterMapping& maps = ParameterMapping::reference();
//  if (maps.isReducingControlSpaceViaMultiScale)
//    maps.objectiveFinalValue = fsol[0];

//  finalReport("    Optimal objective value: ", xsol, fsol[0]);

  delete[] iGfun;
  delete[] jGvar;



  delete[] x;
  delete[] xlow;
  delete[] xupp;
  delete[] xmul;
  delete[] xstate;

  delete[] F;
  delete[] Flow;
  delete[] Fupp;
  delete[] Fmul;
  delete[] Fstate;

  delete[] xnames;
  delete[] Fnames;

//  optdata.pOptimizationProblem = NULL;
}




void SNOPTSolver::setOptionsForSNOPT(SNOPTHandler& snoptHandler)
{

  if (settings_->verb_vector()[6] >= 1) // idx:6 -> opt (Optimization)
    cout << "[opt]Set options for SNOPT.---" << endl;

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
  snoptHandler.setParameter((char*)"Derivative linesearch");
  snoptHandler.setIntParameter("Derivative option", 1);
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
  //snoptHandler.setParameter("LU partial pivoting");
  //snoptHandler.setParameter("LU density tolerance           0.6");
  //snoptHandler.setParameter("LU singularity tolerance       3.2e-11");

  //target nonlinear constraint violation
//  snoptHandler.setRealParameter("Major feasibility tolerance", optdata.constraintTolerance);
//  snoptHandler.setIntParameter("Major Iterations Limit", optdata.maxNumberOfIterations);

  //target complementarity gap
//  snoptHandler.setRealParameter("Major optimality tolerance", optdata.convergenceTolerance);

  //snoptHandler.setParameter("Major Print level  000001");
//  snoptHandler.setRealParameter("Major step limit", optdata.majorStepLimit);
  snoptHandler.setIntParameter("Minor iterations limit", 200);

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
  snoptHandler.setParameter((char*)"Scale Print");
  snoptHandler.setParameter((char*)"Solution  yes");
  //snoptHandler.setParameter("Start Objective Check at Column 1");
  //snoptHandler.setParameter("Start Constraint Check at Column 1");
  //snoptHandler.setParameter("Stop Objective Check at Column");
  //snoptHandler.setParameter("Stop Constraint Check at Column");
  //snoptHandler.setParameter("Sticky parameters               No");
  //snoptHandler.setParameter("Summary frequency               100");
  //snoptHandler.setParameter("Superbasics limit");
  //snoptHandler.setParameter("Suppress parameters");
  snoptHandler.setParameter((char*)"System information  No");
  //snoptHandler.setParameter("Timing level                    3");
  //snoptHandler.setRealParameter("Unbounded objective value   1.0e+15");
  //snoptHandler.setParameter("Unbounded step size             1.0e+18");
  snoptHandler.setIntParameter("Verify level", -1);
  snoptHandler.setRealParameter("Violation limit", 1e-8);

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
