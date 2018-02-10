//
// Created by bellout on 2/10/18.
//

#include "FieldOpt-3rdPartySolvers/handlers/SNOPTLoader.h"
#include "SNOPTSolver.h"
#include <iostream>
#include <iomanip>

namespace Optimization {
namespace Optimizers {

SNOPTSolver::SNOPTSolver(){}

SNOPTSolver::~SNOPTSolver(){}

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

void SNOPTSolver::callSNOPT()
{
  int i;
  string prnt_file, smry_file, optn_file;
  prnt_file = ".opt.prnt";
  smry_file = ".opt.summ";
  optn_file = ".opt.optn";

  SNOPTHandler optimizeWithSNOPT(prnt_file.c_str(),
                                 smry_file.c_str(),
                                 optn_file.c_str());

  // Allocate and initialize;
  int n;
  int m;
  int nnz_jac_g;
  int nnz_h_lag;
//  Ipopt::TNLP::IndexStyleEnum index_style;

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

//  OptimizationProblem->get_bounds_info(n, xlow, xupp,
//                                       m + numberOfLinearConstraints,
//                                       &Flow[1], &Fupp[1]);

  // When we have an initial guess the states should be zero
  for (i = 0; i < n; i++)
    xstate[i] = 0;

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

  optimizeWithSNOPT.setProblemSize( n, neF );
  optimizeWithSNOPT.setObjective  ( ObjRow );
  optimizeWithSNOPT.setA          ( lenA, iAfun, jAvar, A );
  optimizeWithSNOPT.setG          ( lenG, iGfun, jGvar );
  optimizeWithSNOPT.setX          ( x, xlow, xupp, xmul, xstate );
  optimizeWithSNOPT.setF          ( F, Flow, Fupp, Fmul, Fstate );
  optimizeWithSNOPT.setXNames     ( xnames, nxnames );
  optimizeWithSNOPT.setFNames     ( Fnames, nFnames );
  optimizeWithSNOPT.setNeA         ( neA );
  optimizeWithSNOPT.setNeG         ( neG );

  // Sets the usrfun that supplies G and F.
  optimizeWithSNOPT.setUserFun( SNOPTusrFG_ );

  optimizeWithSNOPT.setProbName( "SNOPTSolver" );

  if (!optimizeWithSNOPT.has_snopt_option_file)
    setOptionsForSNOPT(optimizeWithSNOPT);

  // Minimize/maximize option
//  if (((ObjectiveFunctional*)optdata.vObjectives[OBJ])->is_minimized)
//    optimizeWithSNOPT.setParameter((char*)"Minimize");
//  else
    optimizeWithSNOPT.setParameter((char*)"Maximize");

  integer Cold = 0, Basis = 1, Warm = 2;

  vector<double> xsol;
  vector<double> fsol;
  optimizeWithSNOPT.solve( Cold, xsol, fsol);

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


void SNOPTSolver::setOptionsForSNOPT(SNOPTHandler& optimizeWithSNOPT)
{

  //optimizeWithSNOPT.setParameter("Backup basis file              0");
//  optimizeWithSNOPT.setRealParameter("Central difference interval", 2 * derivativeRelativePerturbation);

  //optimizeWithSNOPT.setIntParameter("Check frequency",           60);
  //optimizeWithSNOPT.setParameter("Cold Start                     Cold");
  //optimizeWithSNOPT.setParameter("Crash option                   3");
  //optimizeWithSNOPT.setParameter("Crash tolerance                0.1");
  //optimizeWithSNOPT.setParameter("Derivative level               3");

//  if ( (optdata.optimizationType == HISTORY_MATCHING) || hasNonderivativeLinesearch )
//    optimizeWithSNOPT.setParameter((char*)"Nonderivative linesearch");
//  else
    optimizeWithSNOPT.setParameter((char*)"Derivative linesearch");
  optimizeWithSNOPT.setIntParameter("Derivative option", 1);
//  optimizeWithSNOPT.setRealParameter("Difference interval", optdata.derivativeRelativePerturbation);

  //optimizeWithSNOPT.setParameter("Dump file                      0");
  //optimizeWithSNOPT.setParameter("Elastic weight                 1.0e+4");
  //optimizeWithSNOPT.setParameter("Expand frequency               10000");
  //optimizeWithSNOPT.setParameter("Factorization frequency        50");
  //optimizeWithSNOPT.setRealParameter("Function precision", sim_data.tuningParam.tstep.minDeltat);
  //optimizeWithSNOPT.setParameter("Hessian full memory");
  //optimizeWithSNOPT.setParameter("Hessian limited memory");

//  optimizeWithSNOPT.setIntParameter("Hessian frequency", optdata.frequencyToResetHessian);
  //optimizeWithSNOPT.setIntParameter("Hessian updates", 0);
  //optimizeWithSNOPT.setIntParameter("Hessian flush", 1);  // Does NOT work in the current version of SNOPT!!!

  //optimizeWithSNOPT.setParameter("Insert file                    0");
//  optimizeWithSNOPT.setRealParameter("Infinite bound", optdata.defaultControlUpperBound);

  //optimizeWithSNOPT.setParameter("Iterations limit");
  //optimizeWithSNOPT.setRealParameter("Linesearch tolerance",0.9);
  //optimizeWithSNOPT.setParameter("Load file                      0");
  //optimizeWithSNOPT.setParameter("Log frequency                  100");
  //optimizeWithSNOPT.setParameter("LU factor tolerance            3.99");
  //optimizeWithSNOPT.setParameter("LU update tolerance            3.99");
  //optimizeWithSNOPT.setParameter("LU partial pivoting");
  //optimizeWithSNOPT.setParameter("LU density tolerance           0.6");
  //optimizeWithSNOPT.setParameter("LU singularity tolerance       3.2e-11");

  //target nonlinear constraint violation
//  optimizeWithSNOPT.setRealParameter("Major feasibility tolerance", optdata.constraintTolerance);
//  optimizeWithSNOPT.setIntParameter("Major Iterations Limit", optdata.maxNumberOfIterations);

  //target complementarity gap
//  optimizeWithSNOPT.setRealParameter("Major optimality tolerance", optdata.convergenceTolerance);

  //optimizeWithSNOPT.setParameter("Major Print level  000001");
//  optimizeWithSNOPT.setRealParameter("Major step limit", optdata.majorStepLimit);
  optimizeWithSNOPT.setIntParameter("Minor iterations limit", 200);

  //for satisfying the QP bounds
//  optimizeWithSNOPT.setRealParameter("Minor feasibility tolerance", optdata.constraintTolerance);
  optimizeWithSNOPT.setIntParameter("Minor print level", 0);
  //optimizeWithSNOPT.setParameter("New basis file                 0");
  //optimizeWithSNOPT.setParameter("New superbasics limit          99");
  //optimizeWithSNOPT.setParameter("Objective Row");
  //optimizeWithSNOPT.setParameter("Old basis file                 0");
  //optimizeWithSNOPT.setParameter("Partial price                  1");
  //optimizeWithSNOPT.setParameter("Pivot tolerance                3.7e-11");
  //optimizeWithSNOPT.setParameter("Print frequency                100");
  //optimizeWithSNOPT.setParameter("Proximal point method          1");
  //optimizeWithSNOPT.setParameter("QPSolver Cholesky");
  //optimizeWithSNOPT.setParameter("Reduced Hessian dimension");
  //optimizeWithSNOPT.setParameter("Save frequency                 100");
  optimizeWithSNOPT.setIntParameter("Scale option", 1);
  //optimizeWithSNOPT.setParameter("Scale tolerance                0.9");
  optimizeWithSNOPT.setParameter((char*)"Scale Print");
  optimizeWithSNOPT.setParameter((char*)"Solution  yes");
  //optimizeWithSNOPT.setParameter("Start Objective Check at Column 1");
  //optimizeWithSNOPT.setParameter("Start Constraint Check at Column 1");
  //optimizeWithSNOPT.setParameter("Stop Objective Check at Column");
  //optimizeWithSNOPT.setParameter("Stop Constraint Check at Column");
  //optimizeWithSNOPT.setParameter("Sticky parameters               No");
  //optimizeWithSNOPT.setParameter("Summary frequency               100");
  //optimizeWithSNOPT.setParameter("Superbasics limit");
  //optimizeWithSNOPT.setParameter("Suppress parameters");
  optimizeWithSNOPT.setParameter((char*)"System information  No");
  //optimizeWithSNOPT.setParameter("Timing level                    3");
  //optimizeWithSNOPT.setRealParameter("Unbounded objective value   1.0e+15");
  //optimizeWithSNOPT.setParameter("Unbounded step size             1.0e+18");
  optimizeWithSNOPT.setIntParameter("Verify level", -1);
  optimizeWithSNOPT.setRealParameter("Violation limit", 1e-8);

}




int SNOPTusrFG_( integer    *Status, integer *n,    double x[],
                 integer    *needF,  integer *neF,  double F[],
                 integer    *needG,  integer *neG,  double G[],
                 char       *cu,     integer *lencu,
                 integer    iu[],    integer *leniu,
                 double     ru[],    integer *lenru )
{
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
  if ( *needF > 0 )
  {
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

  if ( *needG > 0 )
  {

    // we have as many derivatives as the number of the controls, n
//    optdata.pOptimizationProblem->eval_grad_f(*n, x, false, G);

    // and the derivatives of the constraints follow
    if ( m ){
//      optdata.pOptimizationProblem->eval_jac_g(*n, x, false, m, *neG, 0, 0, &G[*n]);
    }

  }

  return 0;
}

}
}
