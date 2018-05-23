//
// Created by pcg1 on 12.01.18.
//

#include "DFO.h"
#include "GradientEnhancedModel.h"
#include <iostream>
#include <iomanip>

#define MODEL_IMPROVEMENT_ALGORITHM 0
#define FOUND_NEW_OPTIMUM_CANDIDATE 1
#define CRITICALITY_STEP 2
#define MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND 3
#define MODEL_IMPROVEMENT_ALGORITHM_FINISHED 4
#define INITIALIZED_MODEL 5
#define TRUST_REGION_RADIUS_UPDATE_STEP 6
#define TRUST_REGION_RADIUS_UPDATE 7
#define CRITICALITY_STEP_FINISHED 8
#define TRIAL_POINT_FOUND 9
#define TRIAL_POINT_IS_NOT_NEW_OPTIMUM 10
#define NEW_POINT_INCLUDED 11
#define MODEL_IMPROVEMENT_POINT_FOUND 12
#define MODEL_IMPROVEMENT_POINT_ADDED 13
#define MODEL_IMPROVEMENT_ALGORITHM_POINT_ADDED 14
#define BAD_TRIAL_POINT_FOUND 15
#define INTERPOLATION_SET_IS_ALREADY_POISED 16
#define MODEL_IMPROVEMENT_ALGORITHM_MULTIPLE_POINTS_FOUND 17
#define MODEL_IMPROVEMENT_ALGORITHM_MULTIPLE_POINTS_ADDED 18
#define MODEL_IMPROVEMENT_FORCED_POINT_START 19
#define MODEL_IMPROVEMENT_FORCED_POINT_FOUND 20
#define MODEL_IMPROVEMENT_FORCED_POINTS_FOUND 21
#define MODEL_IMPROVEMENT_FORCED_POINT_ADDED 22
#define MODEL_IMPROVEMENT_FORCED_POINTS_ADDED 22

/// Test functions - start
double matyasFunction(Eigen::VectorXd x) {
  double val = 0.26 * (x(0) * x(0) + x(1) * x(1)) - 0.46 * x(0) * x(1);
  return val;
}

Eigen::VectorXd matyasFunctionWithGradients(Eigen::VectorXd x) {
  double val = 0.26 * (x(0) * x(0) + x(1) * x(1)) - 0.46 * x(0) * x(1);
  Eigen::VectorXd ret(1 + 2);
  ret[0] = val;
  ret[1] = 0.52 * x(0) - 0.46 * x(1);
  ret[2] = 0.52 * x(1) - 0.46 * x(0);
  return ret;
}

Eigen::VectorXd matyasFunctionWithGradients1(Eigen::VectorXd x) {
  double val = 0.26 * (x(0) * x(0) + 0.5 * x(1) * x(1)) - 0.46 * x(0) * x(1) + 2 * x(1) + 5.5 - 3.2 * x(0);
  Eigen::VectorXd ret(1 + 1);
  ret[0] = val;
  ret[1] = 0.52 * x(1) - 0.46 * x(0) + 2;
  return ret;
}

Eigen::VectorXd test(Eigen::VectorXd x) {
  Eigen::VectorXd ret(1 + 4);
  ret.setOnes();
  ret[0] = x.norm();
  return ret;
}

double sphere(Eigen::VectorXd x) {
  double val = 0;
  for (int i = 0; i < x.rows(); i++) {
    val += (x(i) - 10) * (x(i) - 10);
//    val += (x(i)  - 23 ) * (x(i) - 4);
  }
  return val;
}

Eigen::VectorXd sphereGradients(Eigen::VectorXd x, int ng) {
  Eigen::VectorXd ret(1 + ng);
  double val = 0;
  for (int i = 0; i < x.rows(); i++) {
    val += x(i) * x(i);
  }
  ret[0] = val;
  int n = x.rows();
  int j = 1;
  for (int i = (n - ng); i < n; ++i) {
    ret[j] = 2 * x(i);
    j++;
  }
  return ret;
}

/// Test functions - end



namespace Optimization {
namespace Optimizers {
double matyasFunction2D(Eigen::VectorXd x);
double rosenbrockFunction2D(Eigen::VectorXd x);
double polynomial3(Eigen::VectorXd x);
double polynomial32(Eigen::VectorXd x);
double polynomial5(Eigen::VectorXd x);
double polynomial6(Eigen::VectorXd x);
double evaluateFunction(Eigen::VectorXd x, Eigen::VectorXd y0, int select);
double evaluateFunction(Eigen::VectorXd x, Eigen::VectorXd y0, int select) {
  Eigen::VectorXd y = x + y0;
  switch (select) {
    case 0:return matyasFunction2D(y);
      break;
    case 1:return rosenbrockFunction2D(y);
      break;
    case 2:return polynomial6(y);
      break;
    case 3:return polynomial5(y);
      break;
    case 4:return polynomial3(y);
      break;
    case 5:return polynomial32(y);
      break;
    default:std::cout << "Invalid function selection" << std::endl;
      break;
  }

}

DFO::DFO(Settings::Optimizer *settings,
         Optimization::Case *base_case,
         Model::Properties::VariablePropertyContainer *variables,
         Reservoir::Grid::Grid *grid,
         Logger *logger)
    : Optimizer(settings, base_case, variables, grid, logger),
      DFO_model_(settings->parameters().number_of_interpolation_points,
                 settings->parameters().number_of_variables,
                 settings->parameters().number_of_variables_with_gradients,
                 base_case->GetRealVarVector(),
                 settings->parameters().initial_trust_region_radius,
                 settings->parameters().required_poisedness,
                 settings->parameters().weight_model_determination_minimum_change_hessian,
                 settings->parameters().weights_distance_from_optimum_lsq,
                 settings) {
  // Set parameters and stuff here
  if (settings->parameters().initial_trust_region_radius > 0.0)
    initial_trust_region_radius_ = settings->parameters().initial_trust_region_radius;
  else
    initial_trust_region_radius_ = 600;

  if (settings->parameters().number_of_interpolation_points > 0)
    number_of_interpolation_points_ = settings->parameters().number_of_interpolation_points;
  else
    number_of_interpolation_points_ = 21;

  if (settings->parameters().number_of_variables > 0)
    number_of_variables_ = settings->parameters().number_of_variables;
  else
    number_of_variables_ = 10;

  if (settings->parameters().required_poisedness > 0)
    required_poisedness_ = settings->parameters().required_poisedness;
  else
    required_poisedness_ = 5;

  settings_ = settings;
  varcont_ = variables;
  iterations_ = 0;
  previous_iterate_type_ = 0;
  base_case_ = new Case(base_case);
  last_action_ = -1;

  weights_distance_from_optimum_lsq_ =
      Eigen::VectorXd::Zero(settings->parameters().weights_distance_from_optimum_lsq.size());
  int j = 0;
  for (auto i = settings->parameters().weights_distance_from_optimum_lsq.begin();
       i != settings->parameters().weights_distance_from_optimum_lsq.end(); ++i) {
    weights_distance_from_optimum_lsq_[j] = *i;
    j++;
    if (j >= 3) {
      break;
    }
  }

  // Set up the DFO model.
  Eigen::VectorXd initialStartPoint = base_case->GetRealVarVector();
  /// Testing snopt solver
/*
  std::cout << std::scientific;
  std::cout << std::setprecision(15);
  vector<double> xsolmin;
  vector<double> xsolmax;
  vector<double> fsolmin;
  vector<double> fsolmax;
  Subproblem mySub(settings);
  double trr = 0.000000084360314/2.0;
  double c = 0.000001268519113;
  Eigen::MatrixXd h(number_of_variables_, number_of_variables_);
  h << 0.000000000000748, -0.000000000000000,
  -0.000000000000000,  0.000000000000748;
  Eigen::VectorXd g(number_of_variables_);
  g << 5.495226660131417, -5.495226829650895;
  Eigen::VectorXd ybr(number_of_variables_);
  ybr << -0.000000175677456, 0.000000175677456;
  Eigen::VectorXd sp(number_of_variables_);
  sp = ybr;
  mySub.setConstant(c); mySub.setGradient(g); mySub.setHessian(h);
  mySub.SetTrustRegionRadius(trr);
  Eigen::VectorXd cp(number_of_variables_);
  cp << 1000.000000160129503, 999.999999839866064;
  Eigen::VectorXd ttt(2);
  ttt.setZero();
  mySub.Solve(xsolmin, fsolmin, (char*)"Minimize",cp,ybr,ybr);
  mySub.Solve(xsolmax, fsolmax, (char*)"Maximize",cp,ybr,ybr);

  Eigen::VectorXd xmin(number_of_variables_);
  Eigen::VectorXd xmax(number_of_variables_);
  for (int i = 0; i < xsolmax.size(); ++i) {
    xmin[i] = xsolmin[i];
    xmax[i] = xsolmax[i];
  }
  std::cout << "\n\n\n\n ----------------------------------------------------------------------------\n\n\n\n";
  std::cout << "ybr: \n" << ybr <<"\n";
  std::cout << "xsolmin: \n" << xmin << "\n" << "xsolMax: \n" << xmax <<"\n";
  std::cout << "minval = " << fsolmin[0] << "\t maxval = " << fsolmax[0] <<"\n";

  trr = 20000;
  c = 2;
  h << 255, -853,
      -853,  55555;
  g << 4000, -3600;
  ybr << 1000, 2000;
  sp = ybr;
  mySub.setConstant(c); mySub.setGradient(g); mySub.setHessian(h);
  mySub.SetTrustRegionRadius(trr);
  cp << 1000.000000160129503, 999.999999839866064;
  mySub.Solve(xsolmin, fsolmin, (char*)"Minimize",cp,ybr,ybr);
  mySub.Solve(xsolmax, fsolmax, (char*)"Maximize",cp,ybr,ybr);

  for (int i = 0; i < xsolmax.size(); ++i) {
    xmin[i] = xsolmin[i];
    xmax[i] = xsolmax[i];
  }
  std::cout << "\n\n\n\n ----------------------------------------------------------------------------\n\n\n\n";
  std::cout << "ybr: \n" << ybr <<"\n";
  std::cout << "xsolmin: \n" << xmin << "\n" << "xsolMax: \n" << xmax <<"\n";
  std::cout << "minval = " << fsolmin[0] << "\t maxval = " << fsolmax[0] <<"\n";


  trr = 9.38361523181344736778e-08;
  c = 1.73332985074814979819e-16;
  h <<  3.16185306180997827141e-01, -3.16185306180997827141e-01,
      -3.16185306180997827141e-01,  3.16185306180997827141e-01;
  g << -3.03179261538150991457e-06,
  3.03363980306762893570e-06;
  ybr << 0.00000000000000000000e+00, 0.00000000000000000000e+00;
  sp = ybr;
  mySub.setConstant(c); mySub.setGradient(g); mySub.setHessian(h);
  mySub.SetTrustRegionRadius(trr);
  cp << 9.99999997200520738261e+02, 9.99999997206447460485e+02;
  mySub.Solve(xsolmin, fsolmin, (char*)"Minimize",cp,ybr,ybr);
  mySub.Solve(xsolmax, fsolmax, (char*)"Maximize",cp,ybr,ybr);

  for (int i = 0; i < xsolmax.size(); ++i) {
    xmin[i] = xsolmin[i];
    xmax[i] = xsolmax[i];
  }
  std::cout << "\n\n\n\n ----------------------------------------------------------------------------\n\n\n\n";
  std::cout << "ybr: \n" << ybr <<"\n";
  std::cout << "xsolmin: \n" << xmin << "\n" << "xsolMax: \n" << xmax <<"\n";
  std::cout << "minval = " << fsolmin[0] << "\t maxval = " << fsolmax[0] <<"\n";
*/


  //DFO_model_ = DFO_Model(number_of_interpolation_points_, number_of_variables_, initialStartPoint, initial_trust_region_radius_, required_poisedness_,settings_);
  //Eigen::VectorXd tt(number_of_variables_);
  //tt = DFO_model_.FindLocalOptimum();
/*
  DFO_model_.findFirstSetOfInterpolationPoints();

  Eigen::VectorXd* refFvals = DFO_model_.getFvalsReference(); // a temporarily (and bad coding practice) solution
  Eigen::MatrixXd* Yref = DFO_model_.getYReference(); // a temporarily (and bad coding practice) solution
  int v = 0;
  std::cout << (*refFvals).rows() << " " << (*refFvals).cols() << "\n ";
  for (int i = 0; i < 2*number_of_variables_+1; i++) {
    ++v;
    (*refFvals)[i] = evaluateFunction((*Yref).col(i), DFO_model_.getCenterPoint(), 0); // Fill up the function evaluations.
  }

  if (number_of_interpolation_points_ >= 2 * number_of_variables_ + 1) {
    DFO_model_.findLastSetOfInterpolationPoints();
    for (int i = 2 * number_of_variables_ + 1; i < number_of_interpolation_points_; i++) {
      ++v;
      (*refFvals)[i] = evaluateFunction((*Yref).col(i), DFO_model_.getCenterPoint(), 0);
    }
  }
  DFO_model_.initializeModel();
*/
// Shift the center point of the model if the best point is not the center point.
  //int bestPointIndex = DFO_model_.getBestPointIndex();
  //Eigen::VectorXd bestPoint(number_of_variables_);
  //bestPoint = (*Yref).col(bestPointIndex - 1);

  //if (bestPointIndex != 1) {
  //  DFO_model_.shiftCenterPointOfQuadraticModel(bestPoint);
  //}

  // Model Improvement algorithm
  /*
   double upperBound = 0;
   double lowerBound = 0;
   int k = 0;
   while (1) {
     int yk = 0;
     Eigen::VectorXd d(number_of_variables_);
     d.setZero();
     DFO_model_.findWorstPointInInterpolationSet(d, yk);

     if (yk == -1) {
       std::cout << "breaking. The required poisedness is acheived. k = " << k << std::endl;
       break;
     }
     else {
       ++v;
       double funcVal = evaluateFunction(d, DFO_model_.getCenterPoint(), 0);
       DFO_model_.update(d, funcVal, yk, DFO_Model::IMPROVE_POISEDNESS);

       //myModel.printParametersMatlabFriendly();
       k++;

       if (k == 2000) {
         std::cout << "breaking because of too many iterations., k = " << k << std::endl;
         break;
       }
     }
   }

   std::cout << *(Yref) << std::endl << std::endl;
   DFO_model_.printParametersMatlabFriendly();
   std::cout << "%Number of function evaluation is: v = " << v << std::endl;

   //GradientEnhancedModel  enhancedModel(n,m,ng,weights_derivatives,weight_objective_minimum_change);
   GradientEnhancedModel  enhancedModel(number_of_variables_,number_of_interpolation_points_,settings->parameters().number_of_variables_with_gradients,settings->parameters().weights_distance_from_optimum_lsq,settings->parameters().weight_model_determination_minimum_change_hessian);

   std::cin.get(); // To avoid exiting terminal.
 */

  //Subproblem mySub(settings);
/*
  vector<double> xsol;
  vector<double> fsol;
  //mySub.Solve(xsol, fsol, (char*)"Maximize");
  mySub.Solve(xsol, fsol, (char*)"Minimize");
  int numVars = 10;
  cout << "xsol: " << endl;
  for (int j = 0; j < numVars; j++) {
    cout << xsol[j] << endl;
  }
  cout << endl << "Objective values:" << endl;
  for (int j = 0; j < 2; j++) {
    cout << fsol[j] << endl;
  }
  cout << "stop " << endl;
*/
  /*
   for (int p = 0; p < 10; p++){
   static std::random_device rd;
   static std::mt19937 gen(p);
   static std::uniform_real_distribution<> dis(-30, 30);
   // Testing with 20 (wells) * 6 (variables pr wells) = 120 variables.
   int numVars = 10;
   Eigen::MatrixXd H(numVars,numVars);
   int k = 0;
   for (int j = 0; j < numVars; ++j) {
     for (int i = 0; i < j; ++i){
       H(i,j) = dis(gen);
       if (i != j){
         H(j,i) = H(i,j);
       }
     }
   }


   Eigen::VectorXd g(numVars);
   g.setRandom();
   mySub.setConstant(1.2);
   mySub.setGradient(g);
   mySub.setHessian(H);
   vector<double> xsol;
   vector<double> fsol;
   //mySub.Solve(xsol, fsol, (char*)"Maximize");
   mySub.Solve(xsol, fsol, (char*)"Minimize");

   cout << "xsol: " << endl;
   for (int j = 0; j < numVars; j++) {
     cout << xsol[j] << endl;
   }
   cout << endl << "Objective values:" << endl;
   for (int j = 0; j < 2; j++) {
     cout << fsol[j] << endl;
   }
   }
   cout << "stop " << endl;
 */

  /*
  cout << "[DFO constructor] ..." << endl;


  //cout << settings_->verb_vector()[0] << endl;
  realvar_uuid_ = GetTentativeBestCase()->GetRealVarIdVector();


  Eigen::VectorXd vars = base_case->GetRealVarVector();

  cout<< "realvar \n" << vars << endl;


  vars[0] += 1;
  vars[1] += 1;
  vars[2] += 1;
  vars[3] += 1;
  vars[4] += 1;
  vars[5] += 1;
  QString old = base_case->GetId().toString();
  Optimization::Case *newCase = new Case(base_case);
  QString neww = newCase->GetId().toString();

  cout << "The ID of the base case is: " << old.toStdString() << endl;
  cout << "The ID of the copied case is: " << neww.toStdString() << endl;

  if (old == neww){
    cout << "WTF \n";
  }
  if (old == old)
    cout << "GOOD " << endl;
  if (neww == neww)
    cout << "GOOD 2" << endl;
  if (old.toStdString() == old.toStdString())
    cout << "GOOD 3" << endl;

  if (base_case->GetId() == base_case->GetId())
    cout << "GOOD 4\n";

  if (base_case->GetId() == newCase->GetId())
    cout << "THIS IS BAD\n";

  if (base_case != newCase)
    cout << "INteresting. comparing cases!" << endl;

  auto fval0 = base_case->objective_function_value();

  newCase->SetRealVarValues(vars);
  newCase->set_objective_function_value(std::numeric_limits<double>::max());
  //auto fval1 = newCase->objective_function_value();

  cout << fval0 << "\t \t" << "oooouups" << endl;

  case_handler_->AddNewCase(newCase);
*/




 //std::cout << "Reached the end of DFO constructor\n";
 //std::cin.get();

}
/*
Optimization::Optimizer::TerminationCondition DFO::IsFinished() {
    return TerminationCondition::NOT_FINISHED;
}

 */
void DFO::handleEvaluatedCase(Optimization::Case *c) {

}

void DFO::iterate() {

  int number_of_crit_step_finished_with_bad_poisedness = 0;

  MatrixXd aa(2, 2);
  aa << 1.1, 2.1, 1.2, 2.2;
  std::cout << "aa\n" << aa << "\n";
  aa.col(0).swap(aa.col(1));
  std::cout << "aa\n" << aa << "\n";

  int ng = settings_->parameters().number_of_variables_with_gradients;
  int alpha = settings_->parameters().weight_model_determination_minimum_change_hessian;

  std::string color_from = "31";
  std::string color_to = "33";

  /// Extract information from cases
  QList<Case *> newCases = case_handler_->RecentlyEvaluatedCases();
  /*
  Need the fval... the rest is already known.
  */

  // This loop is used for testing. Instead of running simulations, a simple analytical function is used.
  // Most of the content in this loop should be in "iterate()", when the testing is done.
  double r =
      settings_->parameters().r; // expansion factor of the trust-region radius to accept more points. (even though they are outside the "true" trust region)
  double w = settings_->parameters().w; // decreasing factor of the trust-region radius during the criticality step.
  double u = settings_->parameters().u;
  double beta = settings_->parameters().beta;
  double epsilon_c = settings_->parameters().epsilon_c;
  double tau = settings_->parameters().tau;
  double eta1 = settings_->parameters().eta1;
  double gamma = settings_->parameters().gamma;
  double gamma_inc = settings_->parameters().gamma_inc;
  double trust_region_radius_inc = settings_->parameters().initial_trust_region_radius;
  double trust_region_radius_max = settings_->parameters().max_trust_region_radius;

  bool notConverged = true;
  double rho = 0;
  int criticality_step_iteration = 0;
  double trust_region_radius_tilde = 0;

  Eigen::VectorXd function_evaluations(number_of_interpolation_points_);
  Eigen::MatrixXd functionValsAndGrad(1 + ng, number_of_interpolation_points_);
  functionValsAndGrad.setZero();
  function_evaluations.setZero();
  Eigen::VectorXd functionValAndGrad(1 + ng);
  double function_evaluation;

  // These are the ones that are used the most. Used in all other places than "Step 4 - Model Improvement".
  Eigen::VectorXd new_point(number_of_variables_);
  int index_of_new_point = -1;

  // Only used in the initialization process.
  int number_of_new_interpolation_points = 0;

  Eigen::VectorXd *refFuncVals = DFO_model_.getFvalsReference();
  Eigen::MatrixXd *refY = DFO_model_.getYReference();
  Eigen::MatrixXd *refDerivatives = DFO_model_.getDerivativeReference();
  bool isTrialPointNewOptimum = false;

  //Eigen::MatrixXd derivatives = Eigen::MatrixXd::Zero(2, number_of_interpolation_points_);
  //GradientEnhancedModel enhancedModel(number_of_variables_,number_of_interpolation_points_, ng, weights_distance_from_optimum_lsq_, alpha);

  Eigen::MatrixXd new_points_criticality;
  Eigen::VectorXi new_indicies_criticality;
  std::cout << std::setprecision(20);
  //std::cout << std::scientific;
  //std::cout.setstate(std::ios_base::failbit);
  bool isCFL = false;
  while (notConverged) {
    if (iterations_ == 104){
      std::cout << "iterate 104. stop \n";

    }
    if (iterations_ > 1000) {
      DFO_model_.printQuadraticModel();
    }
    std::cout << "\033[1;34;m " << " ---------- New iterate " << iterations_ << " ---------- " << "\033[0m"
              << std::endl;
    std::cout << "\033[1;34;m " << "Fvals = \n" << "\033[0m" << *refFuncVals << "\n";
    std::cout << "\033[1;34;m " << "Y = \n" << "\033[0m" << *refY << "\n";
    if (iterations_ != 0 && iterations_ != 1 && iterations_ != 2) {
      std::cout << "\033[1;34;m " << "Best point index = \n" << "\033[0m" << DFO_model_.getBestPointIndex() << "\n";

    }
    std::cout << "\033[1;34;m " << "y0 = \n" << "\033[0m" << DFO_model_.getCenterPoint() << "\n";
    std::cout << "\033[1;34;m " << "Trust region radius is: " << "\033[0m" << DFO_model_.GetTrustRegionRadius()
              << std::endl;
    //DFO_model_.SetTrustRegionRadius(5);
    // DFO_model_.printQuadraticModel();
    Eigen::MatrixXd new_points;

    if (iterations_ == 0) {
      new_points = DFO_model_.findFirstSetOfInterpolationPoints();
      number_of_new_interpolation_points = new_points.cols();
    } else if (DFO_model_.isInitialInterpolationPointsFound() == false) {
      //std::cout << "function evaluations from _simulator_ \n" << function_evaluations << "\n";
      for (int i = 0; i < number_of_new_interpolation_points; ++i) {
        DFO_model_.SetFunctionValue(i + 1, function_evaluations[i]);
        //DFO_model_.SetFunctionValueAndDerivatives(i + 1, functionValsAndGrad.col(i));
      }
      //std::cout << "function values \n" << *refFuncVals << "\nend" << std::endl;
      new_points = DFO_model_.findLastSetOfInterpolationPoints();
    } else if (DFO_model_.isModelInitialized() == false) {
      if (iterations_ == 1) {
        for (int i = 0; i < number_of_new_interpolation_points; ++i) {
          DFO_model_.SetFunctionValue(i + 1, function_evaluations[i]);
          //DFO_model_.SetFunctionValueAndDerivatives(i + 1, functionValsAndGrad.col(i));
        }
      }

      if (number_of_new_interpolation_points != number_of_interpolation_points_) {
        for (int i = number_of_new_interpolation_points; i < number_of_interpolation_points_; ++i) {
          DFO_model_.SetFunctionValue(i + 1, function_evaluations[i - number_of_new_interpolation_points]);
          //DFO_model_.SetFunctionValueAndDerivatives(i+1, functionValsAndGrad.col(i - number_of_new_interpolation_points));
        }
      }
      //std::cout << "function values \n" << *refFuncVals << "\nend" << std::endl;


      DFO_model_.initializeModel();

      UpdateLastAction(INITIALIZED_MODEL);

      DFO_model_.printParametersMatlabFriendly();
      //DFO_model_.shiftCenterPointOfQuadraticModel(-DFO_model_.getCenterPoint());
      //DFO_model_.printQuadraticModel();
      std::cout << "------------Gradients ---------" << std::endl;
      std::cout << *refDerivatives << std::endl;

      Eigen::MatrixXd hess(number_of_variables_, number_of_variables_);
      Eigen::VectorXd grad(number_of_variables_);
      double constant;
      Eigen::MatrixXd ycop = (*refY).block(0, 1, 2, number_of_interpolation_points_ - 1);
      Eigen::MatrixXd dercopy = (*refDerivatives).block(0, 1, ng, number_of_interpolation_points_ - 1);
      Eigen::VectorXd funccopy = (*refFuncVals).tail(3);
      Eigen::VectorXd gradCp = (*refDerivatives).col(0).tail(ng);
      int index_of_center_point = 0;
      //enhancedModel.ComputeModel( (*refY), (*refDerivatives), gradCp /*DFO_model_.GetGradient()*/, (*refFuncVals),DFO_model_.getCenterPoint(), DFO_model_.GetBestPoint(),
      //                           DFO_model_.GetTrustRegionRadius(), r, index_of_center_point);
      //enhancedModel.ComputeModel( ycop,dercopy, gradCp, funccopy,DFO_model_.getCenterPoint(), DFO_model_.GetBestPoint(),
      //DFO_model_.GetTrustRegionRadius(), r, index_of_center_point);
      //enhancedModel.GetModel(constant, grad,hess);
      std::cout << "------------Enhanced model? ---------" << std::endl;
      std::cout << "c = " << constant << std::endl;
      std::cout << "gradient = " << std::endl << grad << std::endl;
      std::cout << "hessian = " << std::endl << hess << std::endl;
    }

    if (last_action_ == MODEL_IMPROVEMENT_POINT_FOUND) {
      DFO_model_.update(new_point, function_evaluation, index_of_new_point, DFO_Model::INCLUDE_NEW_POINT);

      UpdateLastAction(MODEL_IMPROVEMENT_POINT_ADDED);
      //if (index_of_new_point == DFO_model_.getBestPointIndex()) {
      //  DFO_model_.shiftCenterPointOfQuadraticModel(DFO_model_.GetBestPoint());
      //}
      std::cout << "\033[1;34;m " << "Fvals = \n" << "\033[0m" << *refFuncVals << "\n";
      std::cout << "\033[1;34;m " << "Y = \n" << "\033[0m" << *refY << "\n";
    }

    step1:
    if (last_action_ == INITIALIZED_MODEL || last_action_ == TRUST_REGION_RADIUS_UPDATE_STEP
        || last_action_ == MODEL_IMPROVEMENT_POINT_ADDED) {
      if (last_action_ == TRUST_REGION_RADIUS_UPDATE_STEP || last_action_ == MODEL_IMPROVEMENT_POINT_ADDED) {
        DFO_model_.SetTrustRegionRadius(trust_region_radius_inc);
      }
      Eigen::VectorXd gradient = DFO_model_.GetGradientAtPoint(DFO_model_.GetBestPoint());
      std::cout << "norm of the gradient of the model: \n" << gradient.norm() << "\n\n";
      if (gradient.norm() > epsilon_c) {

        Eigen::VectorXd dummyVec(number_of_variables_);
        dummyVec.setZero();
        int dummyInt = 0;
        DFO_model_.SetTrustRegionRadiusForSubproblem(r * DFO_model_.GetTrustRegionRadius());
        //DFO_model_.findWorstPointInInterpolationSet(dummyVec, dummyInt); //Check if it is lambda-poised.
        DFO_model_.isPoised(dummyVec, dummyInt, r * DFO_model_.GetTrustRegionRadius());
        if (dummyInt != -1) {
          number_of_crit_step_finished_with_bad_poisedness++;
        } else {
          number_of_crit_step_finished_with_bad_poisedness = 0;
        }
        UpdateLastAction(CRITICALITY_STEP_FINISHED);
      } else {

        // Kan finne punkt som er utenfor r*delta...



        //std::cout << "new point: \n" << new_point  << "\nindex of new point \n" << index_of_new_point << "\n\n";
        //DFO_model_.SetTrustRegionRadius(trust_region_radius_inc);

        //See if possible to do a cheap replacement (i.e., no need to optimize all lagrange polynomials twice).
        int index_far_away_point = DFO_model_.findPointFarthestAwayFromOptimum();
        double distance = DFO_model_.norm((DFO_model_.GetPoint(index_far_away_point) - DFO_model_.GetBestPoint()));
        bool cheapImprovementPossible = false;
        if (distance > r * DFO_model_.GetTrustRegionRadius()) {
          cheapImprovementPossible =
              DFO_model_.FindPointToReplaceWithPointOutsideScaledTrustRegion(index_far_away_point, new_point);
        }
        else {
          DFO_model_.SetTrustRegionRadiusForSubproblem(r * DFO_model_.GetTrustRegionRadius());
          DFO_model_.findWorstPointInInterpolationSet(new_point, index_of_new_point); //Check if it is lambda-poised.
          if (index_of_new_point == -1) {
            UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_FINISHED);
          } else {
            UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND);
          }
        }

        if (cheapImprovementPossible  && false) {
          std::cout << *refY << std::endl;
          std::cout << "Cheap improvement is possible" << std::endl;
          index_of_new_point = index_far_away_point;
          UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND);
        } else {
          DFO_model_.SetTrustRegionRadiusForSubproblem(r * DFO_model_.GetTrustRegionRadius());
          DFO_model_.findWorstPointInInterpolationSet(new_point, index_of_new_point); //Check if it is lambda-poised.
          //bool isPoisedExceptBestPoint =  DFO_model_.FindPointToIncreasePoisedness(new_point, index_of_new_point);
          //DFO_model_.findWorstPointInInterpolationSetByLU(new_point, index_of_new_point);

          if (((gradient.norm() <= epsilon_c) && (index_of_new_point != -1))
              || ((gradient.norm() <= epsilon_c) && (trust_region_radius_inc > u * gradient.norm()))) {
            criticality_step_iteration = 0;

            //DFO_model_.findWorstPointInInterpolationSet(new_point,index_of_new_point);
            UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND);

            if (index_of_new_point == -1) {
              UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_FINISHED);
            }
          } else {
            Eigen::VectorXd dummyVec(number_of_variables_);
            dummyVec.setZero();
            int dummyInt = 0;
            DFO_model_.SetTrustRegionRadiusForSubproblem(r * DFO_model_.GetTrustRegionRadius());
            //DFO_model_.findWorstPointInInterpolationSet(dummyVec, dummyInt); //Check if it is lambda-poised.
            DFO_model_.isPoised(dummyVec, dummyInt, r * DFO_model_.GetTrustRegionRadius());
            if (dummyInt != -1) {
              number_of_crit_step_finished_with_bad_poisedness++;
            } else {
              number_of_crit_step_finished_with_bad_poisedness = 0;
            }
            UpdateLastAction(CRITICALITY_STEP_FINISHED);
            //DFO_model_.SetTrustRegionRadius(trust_region_radius_inc);
          }
        }
      }
    }
//    else if(last_action_ == CRITICALITY_STEP){
//
//    }
    else if (last_action_ == MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND) {
      //We are in the middle of trying to increase the poisedness of the interpolation set.
      //Meaning that we must update the model with the new point and keep on checking the poisedness of the interpolation set.

      DFO_model_.update(new_point,
                        function_evaluation,
                        index_of_new_point,
                        DFO_Model::IMPROVE_POISEDNESS); //add the point
      //if (index_of_new_point == DFO_model_.getBestPointIndex()) {
      //  DFO_model_.shiftCenterPointOfQuadraticModel(DFO_model_.GetBestPoint());
      //}

      UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_POINT_ADDED);

      //See if possible to do a cheap replacement (i.e., no need to optimize all lagrange polynomials twice).
      int index_far_away_point = DFO_model_.findPointFarthestAwayFromOptimum();
      double distance = DFO_model_.norm((DFO_model_.GetPoint(index_far_away_point) - DFO_model_.GetBestPoint()));
      bool cheapImprovementPossible = false;
      if (distance > r * DFO_model_.GetTrustRegionRadius()) {
        cheapImprovementPossible =
            DFO_model_.FindPointToReplaceWithPointOutsideScaledTrustRegion(index_far_away_point, new_point);
      }

      if (cheapImprovementPossible && false) {
        index_of_new_point = index_far_away_point;
        UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND);

      } else {
        //DFO_model_.FindPointToIncreasePoisedness(new_point, index_of_new_point);
        //bool isPoisedExceptBestPoint =  DFO_model_.FindPointToIncreasePoisedness(new_point, index_of_new_point);
        //DFO_model_.findWorstPointInInterpolationSetByLU(new_point, index_of_new_point);
        DFO_model_.SetTrustRegionRadiusForSubproblem(r * DFO_model_.GetTrustRegionRadius());
        DFO_model_.findWorstPointInInterpolationSet(new_point, index_of_new_point); //Check if it is lambda-poised.
        if (index_of_new_point == -1) {
          UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_FINISHED);
        } else {
          UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND);

        }
      }


      //DFO_model_.SetTrustRegionRadiusForSubproblem( r*pow(w,criticality_step_iteration)*DFO_model_.GetTrustRegionRadius() );
      //rho_tilde = omega^i-1, rho_inc.
      //if (index != -1 or rho_inc > u*||g_inc||) {
      // use alg4

      //}

    } else if (last_action_ == MODEL_IMPROVEMENT_ALGORITHM_MULTIPLE_POINTS_FOUND) {
      for (int i = 0; i < new_points_criticality.cols(); ++i) {
        if (new_indicies_criticality[i] != -1) {
          DFO_model_.update(new_points_criticality.col(i),
                            function_evaluations[i],
                            new_indicies_criticality[i],
                            DFO_Model::IMPROVE_POISEDNESS);
          //functionValsAndGrad.col(i) = matyasFunctionWithGradients1(new_points.col(i) + DFO_model_.getCenterPoint());
        } else {
          break;
        }
      }
      UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_MULTIPLE_POINTS_ADDED);
      std::cout << "\033[1;34;m " << "Y AFTER PUTTING POINTS INSIDE TRUST REGION RADIUS...." << "\033[0m" << "\n";
      std::cout << "\033[1;34;m " << "Fvals = \n" << "\033[0m" << *refFuncVals << "\n";
      std::cout << "\033[1;34;m " << "Y = \n" << "\033[0m" << *refY << "\n";
      if (iterations_ != 0 && iterations_ != 1 && iterations_ != 2) {
        std::cout << "\033[1;34;m " << "Ybest = \n" << "\033[0m" << DFO_model_.GetBestPoint() << "\n";

      }
      std::cout << "\033[1;34;m " << "Trust region radius is: " << "\033[0m" << DFO_model_.GetTrustRegionRadius()
                << std::endl;


      //See if possible to do a cheap replacement (i.e., no need to optimize all lagrange polynomials twice).
      int index_far_away_point = DFO_model_.findPointFarthestAwayFromOptimum();
      double distance = DFO_model_.norm(DFO_model_.GetPoint(index_far_away_point) - DFO_model_.GetBestPoint());
      bool cheapImprovementPossible = false;
      if (distance > r * DFO_model_.GetTrustRegionRadius()) {
        cheapImprovementPossible =
            DFO_model_.FindPointToReplaceWithPointOutsideScaledTrustRegion(index_far_away_point, new_point);
      }

      if (cheapImprovementPossible && false) {
        index_of_new_point = index_far_away_point;
        UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND);

      } else {
        if (iterations_ == 105){
          std::cout << "stopping \n";
        }
        DFO_model_.SetTrustRegionRadiusForSubproblem(r * DFO_model_.GetTrustRegionRadius());
        DFO_model_.findWorstPointInInterpolationSet(new_point, index_of_new_point); //Check if it is lambda-poised.
        if (index_of_new_point == -1) {
          UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_FINISHED);
        } else {
          UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND);

        }
      }
    }
    if (last_action_ == MODEL_IMPROVEMENT_ALGORITHM_FINISHED) {



      //We are back to the criticality step.
      trust_region_radius_tilde = pow(w, criticality_step_iteration) * trust_region_radius_inc;

      //Do "convergence test" for the criticality step.
      Eigen::VectorXd gradient = DFO_model_.GetGradientAtPoint(DFO_model_.GetBestPoint());
      double norm_of_gradient = gradient.norm();
      std::cout << "The norm of the gradient of the model is: " << norm_of_gradient << std::endl;
      if (trust_region_radius_tilde <= u * (norm_of_gradient)) {
        //"converged" The criticality step algorithm is done
        double temp = max(trust_region_radius_tilde, beta * gradient.norm());
        double new_trust_region_radius = min(temp, trust_region_radius_inc);
        DFO_model_.SetTrustRegionRadius(new_trust_region_radius);
        //if (norm_of_gradient <= epsilon_c){
        //}
        Eigen::VectorXd dummyVec(number_of_variables_);
        dummyVec.setZero();
        int dummyInt = 0;
        DFO_model_.SetTrustRegionRadiusForSubproblem(r * DFO_model_.GetTrustRegionRadius());
        //DFO_model_.findWorstPointInInterpolationSet(dummyVec, dummyInt); //Check if it is lambda-poised.
        DFO_model_.isPoised(dummyVec, dummyInt, r * DFO_model_.GetTrustRegionRadius());
        if (dummyInt != -1) {
          number_of_crit_step_finished_with_bad_poisedness++;
        } else {
          number_of_crit_step_finished_with_bad_poisedness = 0;
        }
        UpdateLastAction(CRITICALITY_STEP_FINISHED);
      } else { // Reduce the radius and run the model improvement algorithm.
        // Maybe the set is already "well poised" in the new radius???
        double maxDistance = DFO_model_.findLargestDistanceBetweenPointsAndOptimum();
        std::cout << "decreasing the radius:" << std::endl;
        do {
          criticality_step_iteration++;
          trust_region_radius_tilde = pow(w, criticality_step_iteration) * trust_region_radius_inc;

          std::cout << r * trust_region_radius_tilde << std::endl;
          DFO_model_.SetTrustRegionRadiusForSubproblem(r * trust_region_radius_tilde);
          //DFO_model_.findWorstPointInInterpolationSet(new_point, index_of_new_point);
          DFO_model_.FindPointToIncreasePoisedness(new_point, index_of_new_point);
          //DFO_model_.findWorstPointInInterpolationSetByLU(new_point, index_of_new_point);

          /// if radius halfed, stop this and get new points, then go back here.
          /// check trust_region_radius_tilde > u * (norm_of_gradient) and proceed accordingly...

          //if (trust_region_radius_tilde < DFO_model_.GetTrustRegionRadius()*0.5){
          int number_of_points_outside_trust_region =
              DFO_model_.GetNumberOfPointsOutsideRadius(r * trust_region_radius_tilde);
          if (number_of_points_outside_trust_region >= 1) {

            bool possible =  DFO_model_.FindReplacementForPointsOutsideRadius(r * trust_region_radius_tilde,
                                                             new_points_criticality,
                                                             new_indicies_criticality);

            if (possible == false){
              std::cout << "waiting for u to press a button\n";

              std::cin.get();
            }
            else{
              int a = 0;
              for (int i = 0; i < new_indicies_criticality.rows(); ++i){
                if (new_indicies_criticality[i]  != -1){
                  a++;
                }
                else{
                  break;
                }
              }
              std::cout << "Number of points outside trust-region radius: " << number_of_points_outside_trust_region <<
                         "\nNumber of points to be replaced:              " << a <<"\n";

            UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_MULTIPLE_POINTS_FOUND);
            DFO_model_.SetTrustRegionRadius(pow(w, criticality_step_iteration-1) * trust_region_radius_inc);
            break;
            }
          }
          //}

          /// should also store which order they should be added....
          /// MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND or MODEL_IMPROVEMENT_ALGORITHM_MULTIPLE_POINTS_FOUND
        } while ((index_of_new_point == -1) && (trust_region_radius_tilde > u * (norm_of_gradient)));

        if (last_action_ == MODEL_IMPROVEMENT_ALGORITHM_MULTIPLE_POINTS_FOUND) {
          //nothing?
          DFO_model_.SetTrustRegionRadius(pow(w, criticality_step_iteration) * trust_region_radius_inc);
        }
        else if (index_of_new_point != -1) {
          UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND);
          DFO_model_.SetTrustRegionRadius(pow(w, criticality_step_iteration) * trust_region_radius_inc);
        }
        else if (trust_region_radius_tilde <= u * (norm_of_gradient)) {
          double temp = max(trust_region_radius_tilde, beta * gradient.norm());
          double new_trust_region_radius = min(temp, trust_region_radius_inc);
          DFO_model_.SetTrustRegionRadius(new_trust_region_radius);
          Eigen::VectorXd dummyVec(number_of_variables_);
          dummyVec.setZero();
          int dummyInt = 0;
          DFO_model_.SetTrustRegionRadiusForSubproblem(r * DFO_model_.GetTrustRegionRadius());
          //DFO_model_.findWorstPointInInterpolationSet(dummyVec, dummyInt); //Check if it is lambda-poised.
          DFO_model_.isPoised(dummyVec, dummyInt, r * DFO_model_.GetTrustRegionRadius());
          if (dummyInt != -1) {
            number_of_crit_step_finished_with_bad_poisedness++;
          } else {
            number_of_crit_step_finished_with_bad_poisedness = 0;
          }
          UpdateLastAction(CRITICALITY_STEP_FINISHED);
        } else {
          std::cout << "whhaaaaat" << std::endl;
          std::cin.get();
        }

        //UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND);
      }
    }

    if (last_action_ == CRITICALITY_STEP_FINISHED) {
      if (number_of_crit_step_finished_with_bad_poisedness >= 10 && false) {
        UpdateLastAction(MODEL_IMPROVEMENT_FORCED_POINT_START);
        DFO_model_.UpdateOptimum();
        DFO_model_.shiftCenterPointOfQuadraticModel(DFO_model_.GetBestPoint());
        number_of_crit_step_finished_with_bad_poisedness = 0;
      } else {

        Eigen::VectorXd dummyVec(number_of_variables_);
        dummyVec.setZero();
        int dummyInt = 0;
        DFO_model_.SetTrustRegionRadiusForSubproblem(r * DFO_model_.GetTrustRegionRadius());
        DFO_model_.findWorstPointInInterpolationSet(dummyVec, dummyInt); //Check if it is lambda-poised.
        Eigen::VectorXd gradient = DFO_model_.GetGradientAtPoint(DFO_model_.GetBestPoint());
        bool isGradientSufficientLarge = (gradient.norm() > epsilon_c);
        std::cout << "\033[1;34;m " << "Norm of gradient at best point = " << "\033[0m" << gradient.norm() << "\n";
                                                // 0.000000000000008
                                                 //0.00000000000018709117
                                               //0.00000000000108749612
                                               //0.00000000000108322515
        if (DFO_model_.GetTrustRegionRadius() <= 0.00000000000001 && gradient.norm() <= 0.1) {
          Eigen::MatrixXd Yabs(number_of_variables_, number_of_interpolation_points_);
          for (int j = 0; j < number_of_interpolation_points_; ++j) {
            Eigen::VectorXd sd = (*refY).col(j) + DFO_model_.getCenterPoint();
            eigen_col(Yabs, sd, j);
          }
          std::cout.clear();
          std::cout << "\033[1;36;mDFO terminated. Trust region radius too small.\033[0m" << std::endl;
          std::cout << "\033[1;36;mDFO terminated. Trust region radius too small.\033[0m" << std::endl;
          std::cout << "\033[1;36;mDFO terminated. Trust region radius too small.\033[0m" << std::endl;
          std::cout << "\033[1;36;mDFO terminated. Trust region radius too small.\033[0m" << std::endl;
          std::cout << "\033[1;36;mDFO terminated. Trust region radius too small.\033[0m" << std::endl;
          std::cout << "\033[1;36;mDFO terminated. Trust region radius too small.\033[0m" << std::endl;
          std::cout << "\033[1;36;mDFO terminated. Trust region radius too small.\033[0m" << std::endl;

          std::cout << "\033[1;34;m " << "Norm of gradient at best point = " << "\033[0m" << gradient.norm() << "\n";
          std::cout << "\033[1;34;m " << "Best point index = " << "\033[0m" << DFO_model_.getBestPointIndex() << "\n";
          std::cout << "\033[1;34;m " << "Fvals = \n" << "\033[0m" << *refFuncVals << "\n";
          std::cout << "\033[1;34;m " << "Y = \n" << "\033[0m" << *refY << "\n";
          std::cout << "\033[1;34;m " << "Y absolute = \n" << "\033[0m" << Yabs << "\n";
          std::cout << "\033[1;34;m " << "Ybest = \n" << "\033[0m" << DFO_model_.GetBestPoint() << "\n";
          std::cout << "\033[1;34;m " << "Trust region radius is: " << "\033[0m" << DFO_model_.GetTrustRegionRadius()
                    << std::endl;
          DFO_model_.printParametersMatlabFriendly();
          Eigen::VectorXd cp(3);
          cp[0] = 10;
          cp[1] = 10;
          cp[2] = 10;
          cp = cp - DFO_model_.getCenterPoint();
          DFO_model_.shiftCenterPointOfQuadraticModel(cp);
          DFO_model_.printParametersMatlabFriendly();

          //std::cout << "EXPLICITLY, error?\n";
          //DFO_model_.calculatePolynomialModelDirectlyFromWinverse();



          std::cin.get();
        }


        //Now at step2.
        DFO_model_.SetTrustRegionRadiusForSubproblem(DFO_model_.GetTrustRegionRadius());
        new_point = DFO_model_.FindLocalOptimum();
        std::cout << "Y\n" << *refY << std::endl;
        std::cout << "New (local) optimum found is:\n" << new_point << "\nThe distance from bestpoint is: "
                  << DFO_model_.norm(new_point - DFO_model_.GetBestPoint()) <<
                  "\nThe radius is: " << DFO_model_.GetTrustRegionRadius() << "\t and 2*radius = "
                  << 2 * DFO_model_.GetTrustRegionRadius() << "\n";

        UpdateLastAction(TRIAL_POINT_FOUND);

        double maxDistance = DFO_model_.findLargestDistanceBetweenPointsAndOptimum();
        //if ((new_point - DFO_model_.GetBestPoint()).norm() < tau * maxDistance) {
        if (DFO_model_.norm((new_point - DFO_model_.GetBestPoint())) < tau * maxDistance) {
          std::cout << "TOO CLOSE\n";

          //Check if some other criterias are satisfied, if so; get the objective function value.
          int pointToReplace = DFO_model_.isPointAcceptable(new_point);
          //pointToReplace = -1;
          if (pointToReplace != -1){
            index_of_new_point = pointToReplace;
            UpdateLastAction(BAD_TRIAL_POINT_FOUND);
          }
          else{
          UpdateLastAction(TRIAL_POINT_IS_NOT_NEW_OPTIMUM);
          }

        }
      }
    } else if (last_action_ == BAD_TRIAL_POINT_FOUND) {
      std::cout << "New trial point (that is TOO CLOSE): \n" << new_point <<"\n";
      if (function_evaluation < DFO_model_.GetBestFunctionValueAllTime()) {
        Eigen::VectorXi sorted_points = DFO_model_.GetInterpolationPointsSortedByDistanceFromBestPoint();
        double distance1 = DFO_model_.norm((new_point - DFO_model_.GetBestPoint()));
        double distance2 = DFO_model_.norm((DFO_model_.GetPoint(sorted_points[number_of_interpolation_points_ - 2]) - DFO_model_.GetBestPoint()));
        if ( distance1 < 0.4 *distance2){
          index_of_new_point = DFO_model_.getBestPointIndex();
          DFO_model_.update(new_point, function_evaluation, index_of_new_point, DFO_Model::INCLUDE_NEW_POINT);
          UpdateLastAction(NEW_POINT_INCLUDED);
        }
        else{
          DFO_model_.update(new_point, function_evaluation, index_of_new_point, DFO_Model::INCLUDE_NEW_POINT);
        UpdateLastAction(NEW_POINT_INCLUDED);
        //DFO_model_.UpdateOptimum();
        DFO_model_.shiftCenterPointOfQuadraticModel(DFO_model_.GetBestPoint());
//        if (index_of_new_point == DFO_model_.getBestPointIndex()){
//        }
        }

      } else {
        UpdateLastAction(TRIAL_POINT_IS_NOT_NEW_OPTIMUM);
      }
      double maxDistance = DFO_model_.findLargestDistanceBetweenPointsAndOptimum();
      if (maxDistance / DFO_model_.GetTrustRegionRadius() <= 0.8) {
        //DFO_model_.SetTrustRegionRadius(DFO_model_.GetTrustRegionRadius()*0.99);
        //DFO_model_.SetTrustRegionRadius(maxDistance + (DFO_model_.GetTrustRegionRadius()-maxDistance)*0.5);
      }
    } else if (last_action_ == TRIAL_POINT_FOUND) {
      DFO_model_.SetTrustRegionRadiusForSubproblem(r * DFO_model_.GetTrustRegionRadius());
      int t = DFO_model_.findPointToReplaceWithNewOptimum(new_point);
      rho = (DFO_model_.GetFunctionValue(DFO_model_.getBestPointIndex()) - function_evaluation)
          / (DFO_model_.evaluateQuadraticModel(DFO_model_.GetBestPoint())
              - DFO_model_.evaluateQuadraticModel(new_point));
      Eigen::VectorXd dummyVec(number_of_variables_);
      dummyVec.setZero();
      int dummyInt = 0;
      std::cout << "eta1 = " << eta1 << "\trho = " << rho << "\n";
      DFO_model_.SetTrustRegionRadiusForSubproblem(r * DFO_model_.GetTrustRegionRadius());
      //DFO_model_.findWorstPointInInterpolationSet(dummyVec, dummyInt); //Check if it is lambda-poised.
      DFO_model_.isPoised(dummyVec, dummyInt, r * DFO_model_.GetTrustRegionRadius());
      if ((rho >= eta1) || (dummyInt == -1 && rho > 0)) {
        isTrialPointNewOptimum = true;
        DFO_model_.update(new_point, function_evaluation, t, DFO_Model::INCLUDE_NEW_OPTIMUM);
        DFO_model_.shiftCenterPointOfQuadraticModel(DFO_model_.GetBestPoint());

        double maxDistance = DFO_model_.findLargestDistanceBetweenPointsAndOptimum();
        if (maxDistance / DFO_model_.GetTrustRegionRadius() <= 0.8) {
          //DFO_model_.SetTrustRegionRadius(DFO_model_.GetTrustRegionRadius()*0.99);
          //DFO_model_.SetTrustRegionRadius(maxDistance + (DFO_model_.GetTrustRegionRadius()-maxDistance)*0.5);
        }

        UpdateLastAction(NEW_POINT_INCLUDED);

        //DFO_model_.printQuadraticModel();
      } else if ((dummyInt == -1)
          && (function_evaluation < DFO_model_.GetFunctionValue(DFO_model_.getBestPointIndex()))) {
        isTrialPointNewOptimum = true;
        DFO_model_.update(new_point, function_evaluation, t, DFO_Model::INCLUDE_NEW_OPTIMUM);
        DFO_model_.shiftCenterPointOfQuadraticModel(DFO_model_.GetBestPoint());
        UpdateLastAction(NEW_POINT_INCLUDED);
      } else {

        isTrialPointNewOptimum = false;
        UpdateLastAction(TRIAL_POINT_IS_NOT_NEW_OPTIMUM);
        //int t = DFO_model_.findPointFarthestAwayFromOptimum();
        if ( DFO_model_.norm(( DFO_model_.GetPoint(t) - DFO_model_.GetBestPoint())) > r * DFO_model_.GetTrustRegionRadius() ||
            abs(DFO_model_.ComputeLagrangePolynomial(t, new_point)) > 1) {
          if (abs(DFO_model_.ComputeLagrangePolynomial(t, new_point)) > 1){
          std::cout << "BEFORE ---------------- BEFORE \n";
          DFO_model_.findWorstPointInInterpolationSet(dummyVec, dummyInt);
          DFO_model_.update(new_point, function_evaluation, t, DFO_Model::INCLUDE_NEW_POINT);
          std::cout << "From " << "\033[1;" + color_from + "m " << "Added the point even though it wasn't best new optimum" << "\033[0m";
          std::cout << " ----------------  \n";
          DFO_model_.findWorstPointInInterpolationSet(dummyVec, dummyInt);
          std::cout << "AFTER ---------------- AFTER \n";
          if (index_of_new_point == DFO_model_.getBestPointIndex()) {
            DFO_model_.shiftCenterPointOfQuadraticModel(DFO_model_.GetBestPoint());
          }
          //last_action_ = NEW_POINT_INCLUDED; //Doing this destroys the AND functionality!
        }
        }
      }
    }

    if (last_action_ == MODEL_IMPROVEMENT_FORCED_POINT_FOUND) {
      DFO_model_.update(new_point, function_evaluation, index_of_new_point,
                        DFO_Model::FORCED_IMPROVE_MODEL); //add the point
      std::cout << "\033[1;" + color_from + "m " << "Added the point." << "\033[0m";

      UpdateLastAction(MODEL_IMPROVEMENT_FORCED_POINT_START);
    } else if (last_action_ == MODEL_IMPROVEMENT_FORCED_POINTS_FOUND) {
      for (int i = 0; i < new_points_criticality.cols(); ++i) {
        if (new_indicies_criticality[i] != -1) {
          DFO_model_.update(new_points_criticality.col(i),
                            function_evaluations[i],
                            new_indicies_criticality[i],
                            DFO_Model::FORCED_IMPROVE_MODEL);
          //functionValsAndGrad.col(i) = matyasFunctionWithGradients1(new_points.col(i) + DFO_model_.getCenterPoint());
        } else {
          break;
        }
      }

      UpdateLastAction(MODEL_IMPROVEMENT_FORCED_POINT_START);
    }

    if (last_action_ == MODEL_IMPROVEMENT_FORCED_POINT_START) {
      DFO_model_.SetTrustRegionRadiusForSubproblem(r * DFO_model_.GetTrustRegionRadius());
      DFO_model_.findWorstPointInInterpolationSet(new_point, index_of_new_point); //Check if it is lambda-poised.
      //Check if all points are inside radius;
      int numberOutside = DFO_model_.GetNumberOfPointsOutsideRadius(r * DFO_model_.GetTrustRegionRadius());
      if (numberOutside > 0) {
        //Put all of them inside.
        bool valid = DFO_model_.FindReplacementForPointsOutsideRadius(r * DFO_model_.GetTrustRegionRadius(),
                                                         new_points_criticality,
                                                         new_indicies_criticality);

        if (valid == false && new_indicies_criticality.rows() == 0){
          UpdateLastAction(CRITICALITY_STEP_FINISHED);
        }
        else{
          UpdateLastAction(MODEL_IMPROVEMENT_FORCED_POINTS_FOUND);
        }
      } else if (index_of_new_point != -1) {
        //Replace that one point...
        UpdateLastAction(MODEL_IMPROVEMENT_FORCED_POINT_FOUND);
      } else {
        //All points are inside the trust region. The required poisedness is reached.
        DFO_model_.printParametersMatlabFriendly();
        UpdateLastAction(CRITICALITY_STEP_FINISHED);
      }

      // Ta hensyn til at Y kan ta inn new point OG I TILLEGG, kjøre "do the model improvement step"
      //
    }
    if (last_action_ == TRIAL_POINT_IS_NOT_NEW_OPTIMUM) {
      /// Do the model improvement step.
/*
      Eigen::VectorXd dummyVec(number_of_variables_);
      dummyVec.setZero();
      int dummyInt = 0;
      DFO_model_.SetTrustRegionRadiusForSubproblem(r * DFO_model_.GetTrustRegionRadius());
      DFO_model_.findWorstPointInInterpolationSet(dummyVec, dummyInt); //Check if it is lambda-poised.
      if (dummyInt == -1) {
        std::cout << "FAKE The model improvement step was performed, BUT the model is CFL. No change done.\n";

      }
      else{
*/
      DFO_model_.modelImprovementStep(new_point,index_of_new_point);

      if (index_of_new_point == -1){
        rho = 0; std::cout << "The model improvement step was performed, BUT the model is CFL. No change done.\n";
        isCFL = true;
      }
      else{
        UpdateLastAction(MODEL_IMPROVEMENT_POINT_FOUND);
        rho = 0;
      }
    }

    if (last_action_ == TRIAL_POINT_IS_NOT_NEW_OPTIMUM || last_action_ == NEW_POINT_INCLUDED
        || last_action_ == MODEL_IMPROVEMENT_POINT_FOUND || last_action_ == INTERPOLATION_SET_IS_ALREADY_POISED) {
      // Update the trust-region radius
      if (rho >= eta1) {
        double tmp = std::min(gamma_inc * DFO_model_.GetTrustRegionRadius(), trust_region_radius_max);
        trust_region_radius_inc = 0.2 * DFO_model_.GetTrustRegionRadius() + 0.8 * tmp;
        rho = 0;
      } else {
        //Check CFL.
        Eigen::VectorXd tmp(number_of_variables_);
        int tmp2;
        //DFO_model_.SetTrustRegionRadiusForSubproblem(r * DFO_model_.GetTrustRegionRadius());
        //DFO_model_.findWorstPointInInterpolationSet(tmp, tmp2); //Check if it is lambda-poised. (CFL)
        DFO_model_.isPoised(tmp, tmp2, r*DFO_model_.GetTrustRegionRadius());
        if (tmp2 == -1 || isCFL == true) {
          // model is CFL -> reduce radius
          trust_region_radius_inc = gamma * DFO_model_.GetTrustRegionRadius();
          isCFL = false;
        } else {
          // model is not fully linear -> keep radius
          /// OBS OBS
          /// OBS OBS
          /// OBS OBS
          /// OBS OBS
          /// OBS OBS
          /// OBS OBS
          /// OBS OBS Find out why the radius doesn't decrease without this 0.9.
          /// Probably something wrong with the criticality stuff. Check. and check again.
          trust_region_radius_inc = 1 * DFO_model_.GetTrustRegionRadius();
        }
      }
      if (last_action_ == INTERPOLATION_SET_IS_ALREADY_POISED) {
        trust_region_radius_inc = gamma * DFO_model_.GetTrustRegionRadius();
        UpdateLastAction(TRUST_REGION_RADIUS_UPDATE_STEP);
        goto step1; // To avoid having fieldopt cancel the optimization because the queue (of new cases) is empty.
      }
      if (last_action_ == TRIAL_POINT_IS_NOT_NEW_OPTIMUM || last_action_ == NEW_POINT_INCLUDED) {
        UpdateLastAction(TRUST_REGION_RADIUS_UPDATE_STEP);
        goto step1; // To avoid having fieldopt cancel the optimization because the queue (of new cases) is empty.
      }
    }

    if (last_action_ == CRITICALITY_STEP_FINISHED){
      goto step1;
    }




    /*

              Get the function evaluation(s)

    */

    if (DFO_model_.isModelInitialized()) {
      DFO_model_.printParametersMatlabFriendly();
      //DFO_model_.printParametersMatlabFriendlyGradientEnhanced();
    }

    if (iterations_ == 0 || (iterations_ == 1 && DFO_model_.isModelInitialized() == false)) {
      /// Get the function evaluations for the first set of interpolation points.
      for (int i = 0; i < new_points.cols(); ++i) {
        function_evaluations[i] = sphere(new_points.col(i) + DFO_model_.getCenterPoint());
        //functionValsAndGrad.col(i) = matyasFunctionWithGradients1(new_points.col(i) + DFO_model_.getCenterPoint());
      }
    } else {
      if (last_action_ == MODEL_IMPROVEMENT_ALGORITHM_MULTIPLE_POINTS_FOUND
          || last_action_ == MODEL_IMPROVEMENT_FORCED_POINTS_FOUND) {
        for (int i = 0; i < new_points_criticality.cols(); ++i) {
          if (new_indicies_criticality[i] != -1) {
            function_evaluations[i] = sphere(new_points_criticality.col(i) + DFO_model_.getCenterPoint());
            //functionValsAndGrad.col(i) = matyasFunctionWithGradients1(new_points.col(i) + DFO_model_.getCenterPoint());
          } else {
            break;
          }

        }
      } else if (index_of_new_point < 0) {
        if ((last_action_ == TRIAL_POINT_IS_NOT_NEW_OPTIMUM || last_action_ == TRIAL_POINT_FOUND)) {

        } else {
          std::cout
              << "\n \n Should not happen !!! \n A main iterate() finished without providing: \n new point \n index of points to replace \n "
              << std::endl;
          std::cout << "last_action_ is set to: \t" << last_action_ << std::endl;
          std::cin.get();
        }
      }

      if (last_action_ != MODEL_IMPROVEMENT_ALGORITHM_MULTIPLE_POINTS_FOUND && last_action_ !=MODEL_IMPROVEMENT_FORCED_POINTS_FOUND) {
        /// Get one new point.
        function_evaluation = sphere(new_point + DFO_model_.getCenterPoint());
        //functionValAndGrad = matyasFunctionWithGradients1(new_point + DFO_model_.getCenterPoint());
        if (last_action_ == TRIAL_POINT_FOUND) {
          if (function_evaluation < DFO_model_.GetBestFunctionValueAllTime()) {
            cout << "\033[1;36;mThe new function evaluation is: \033[0m" << function_evaluation << endl << endl;
          }
        }
      }
    }

    iterations_++;

  }/// End of while loop



  // QList<Case *> new_cases = ConvertPointsToCases(new_points);
  // case_handler_->AddNewCases(new_cases);

}

std::string DFO::getActionName(int a) {
  switch (a) {
    case 3: return "MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND";
    case 4: return "MODEL_IMPROVEMENT_ALGORITHM_FINISHED   ";
    case 5: return "INITIALIZED_MODEL                      ";
    case 6: return "TRUST_REGION_RADIUS_UPDATE_STEP        ";
    case 8: return "CRITICALITY_STEP_FINISHED              ";
    case 9: return "TRIAL_POINT_FOUND                      ";
    case 10: return "TRIAL_POINT_IS_NOT_NEW_OPTIMUM         ";
    case 11: return "NEW_POINT_INCLUDED                     ";
    case 12: return "MODEL_IMPROVEMENT_POINT_FOUND          ";
    case 13: return "MODEL_IMPROVEMENT_POINT_ADDED          ";
    case 14: return "MODEL_IMPROVEMENT_ALGORITHM_POINT_ADDED";
    case 15: return "BAD_TRIAL_POINT_FOUND                  ";
    case 16: return "INTERPOLATION_SET_IS_ALREADY_POISED    ";
    case 17: return "MODEL_IMPR_ALG_MULTIPLE_POINTS_FOUND   ";
    case 18: return "MODEL_IMPR_ALG_MULTIPLE_POINTS_ADDED   ";
    case 19: return "MODEL_IMPROVEMENT_FORCED_POINT_START   ";
    case 20: return "MODEL_IMPROVEMENT_FORCED_POINT_FOUND   ";
    case 21: return "MODEL_IMPROVEMENT_FORCED_POINTS_FOUND  ";
    case 22: return "MODEL_IMPROVEMENT_FORCED_POINT_ADDED   ";
    case 23: return "MODEL_IMPROVEMENT_FORCED_POINTS_ADDED  ";
    default:return "Not a valid state                      ";
  }
}

void DFO::UpdateLastAction(int a) {
  std::cout << "From " << "\033[1;" + color_from + "m " << getActionName(last_action_) << "\033[0m";
  last_action_ = a;
  std::cout << "\tTo " << "\033[1;" + color_to + ";m " << getActionName(last_action_) << "\033[0m" << std::endl;
}

/*
QList<Case *> DFO::ConvertPointsToCases(Eigen::MatrixXd points) {

  QList<Case *> new_cases = QList<Case *>();
  for (int i = 0; i < points.cols(); ++i){
    // First add the center of the model such that we get absolute coordinates and not relative.
    points.col(i) += DFO_model_.getCenterPoint();
    // Create the cases
    auto new_case = new Case(base_case_);
    new_case->SetRealVarValues(points.col(i));
    new_case->set_objective_function_value(std::numeric_limits<double>::max());
    new_cases.append(new_case);

  }
  return new_cases;
}
*/








double matyasFunction2D(Eigen::VectorXd x) {
  double squaredx1 = std::pow(x(0), 2);
  double squaredx2 = std::pow(x(1), 2);
  return 0.26 * (squaredx1 + squaredx2) - 0.48 * x(0) * x(1);
}
double rosenbrockFunction2D(Eigen::VectorXd x) {
  double a = std::pow(x(0), 2);
  double b = std::pow(x(1) - a, 2);
  double c = std::pow(1 - x(0), 2);
  return 100 * b + c;
}
double polynomial6(Eigen::VectorXd y) {
  Eigen::VectorXd grad = Eigen::VectorXd::Zero(y.rows());
  Eigen::MatrixXd hess = Eigen::MatrixXd::Zero(y.rows(), y.rows());
  double c = 140;

  grad << 1, 2, 3, 4, 5, 6;
  hess <<
       3, 0, 0, 5, 0, 0,
      0, 4, 0, 0, 4, 88,
      0, 0, 1, 77, 0, 0,
      5, 0, 77, 2, 0, 6,
      0, 4, 0, 0, 7, 0,
      0, 88, 0, 6, 0, 9;

  return c + grad.transpose() * y + 0.5 * y.transpose() * hess * y;

}
double polynomial5(Eigen::VectorXd y) {
  Eigen::VectorXd grad = Eigen::VectorXd::Zero(y.rows());
  Eigen::MatrixXd hess = Eigen::MatrixXd::Zero(y.rows(), y.rows());
  double c = 140;

  grad << 1, 2, 3, 4, 5;
  hess <<
       3, 0, 0, 5, 0,
      0, 4, 0, 0, 4,
      0, 0, 1, 77, 0,
      5, 0, 77, 2, 0,
      0, 4, 0, 0, 7;

  return c + grad.transpose() * y + 0.5 * y.transpose() * hess * y;

}
double polynomial3(Eigen::VectorXd y) {
  Eigen::VectorXd grad = Eigen::VectorXd::Zero(y.rows());
  Eigen::MatrixXd hess = Eigen::MatrixXd::Zero(y.rows(), y.rows());
  double c = 0;

  grad << 5, 1, 92;
  hess <<
       3, -1, 10,
      -1, 4, 7,
      10, 7, 1;

  return c + grad.transpose() * y + 0.5 * y.transpose() * hess * y;

}
double polynomial32(Eigen::VectorXd y) {
  Eigen::VectorXd grad = Eigen::VectorXd::Zero(y.rows());
  Eigen::MatrixXd hess = Eigen::MatrixXd::Zero(y.rows(), y.rows());
  double c = 0;

  grad << 5, 1, 92;
  hess <<
       3, -1, 10,
      -1, 4, 7,
      10, 7, 1;

  return c + grad.transpose() * y + 0.5 * y.transpose() * hess * y + y(1) * y(2) * y(0) + y(0) * y(0) * y(1) * y(1)
      + y(2) * y(2) * y(2) * y(2) * y(2) + y(1) * y(1) * y(1) * y(1);

}
}
}

