//
// Created by pcg1 on 12.01.18.
//

#include "DFO.h"
#include "GradientEnhancedModel.h"

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


/// Test functions - start
double matyasFunction(Eigen::VectorXd x) {
  double val = 0.26 * (x(0) * x(0) + x(1) * x(1)) - 0.46 * x(0) * x(1);
  return val;
}

Eigen::VectorXd matyasFunctionWithGradients(Eigen::VectorXd x) {
  double val = 0.26 * (x(0) * x(0) + x(1) * x(1)) - 0.46 * x(0) * x(1);
  Eigen::VectorXd ret(1+2);
  ret[0] = val;
  ret[1] = 0.52*x(0) - 0.46 * x(1);
  ret[2] = 0.52*x(1) - 0.46 * x(0);
  return ret;
}

Eigen::VectorXd matyasFunctionWithGradients1(Eigen::VectorXd x) {
  double val = 0.26 * (x(0) * x(0) + x(1) * x(1)) - 0.46 * x(0) * x(1);
  Eigen::VectorXd ret(1+1);
  ret[0] = val;
  ret[1] = 0.52*x(0) - 0.46 * x(1);
  return ret;
}

Eigen::VectorXd test(Eigen::VectorXd x){
  Eigen::VectorXd ret(1+4);
  ret.setOnes();
  ret[0] = x.norm();
  return ret;
}

double sphere(Eigen::VectorXd x) {
  double val = 0;
  for (int i = 0; i < x.rows(); i++) {
    val += x(i) * x(i);
  }
  return val;
}
/// Test functions - end

namespace Optimization {
namespace Optimizers {

DFO::DFO(Settings::Optimizer *settings,
         Optimization::Case *base_case,
         Model::Properties::VariablePropertyContainer *variables,
         Reservoir::Grid::Grid *grid,
         Logger *logger)
    : Optimizer(settings, base_case, variables, grid, logger),
      DFO_model_(settings->parameters().number_of_interpolation_points,
                 settings->parameters().number_of_variables,
                 2,
                 base_case->GetRealVarVector(),
                 settings->parameters().initial_trust_region_radius,
                 settings->parameters().required_poisedness,
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


  // Set up the DFO model.
  Eigen::VectorXd initialStartPoint = base_case->GetRealVarVector();

  /*
  vector<double> xsol;
  vector<double> fsol;
  Subproblem mySub(settings);
  Eigen::VectorXd ttt(2);
  ttt.setZero();
  mySub.Solve(xsol, fsol, (char*)"Minimize",ttt,ttt);
  std::cout << "\n\n\n\n ----------------------------------------------------------------------------\n\n\n\n";
*/

  //DFO_model_ = DFO_Model(number_of_interpolation_points_, number_of_variables_, initialStartPoint, initial_trust_region_radius_, required_poisedness_,settings_);
  //Eigen::VectorXd tt(number_of_variables_);
  //tt = DFO_model_.FindLocalOptimum();






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

}
/*
Optimization::Optimizer::TerminationCondition DFO::IsFinished() {
    return TerminationCondition::NOT_FINISHED;
}

 */
void DFO::handleEvaluatedCase(Optimization::Case *c) {

}

void DFO::iterate() {
  int ng = 1;
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
  Eigen::MatrixXd functionValsAndGrad(1+ng,number_of_interpolation_points_);
  functionValsAndGrad.setZero();
  function_evaluations.setZero();
  Eigen::VectorXd functionValAndGrad(1+ng);
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
  Eigen::VectorXd weights(3);
  weights << 1,0.8,0.3;
  GradientEnhancedModel enhancedModel(number_of_variables_,number_of_interpolation_points_, ng, weights, 0.1);


  while (notConverged) {
    if (iterations_ > 1000){
      DFO_model_.printQuadraticModel();
    }
    std::cout << "\033[1;34;m " << " ---------- New iterate " << iterations_ << " ---------- " << "\033[0m"
              << std::endl;
    std::cout << "\033[1;34;m " << "Y = \n" << "\033[0m" << *refY << "\n";
    if (iterations_ != 0 && iterations_ != 1 && iterations_ != 2) {
      std::cout << "\033[1;34;m " << "Ybest = \n" << "\033[0m" << DFO_model_.GetBestPoint() << "\n";

    }
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
        //DFO_model_.SetFunctionValue(i + 1, function_evaluations[i]);
        DFO_model_.SetFunctionValueAndDerivatives(i + 1, functionValsAndGrad.col(i));
      }
      //std::cout << "function values \n" << *refFuncVals << "\nend" << std::endl;
      new_points = DFO_model_.findLastSetOfInterpolationPoints();
    } else if (DFO_model_.isModelInitialized() == false) {
      if (iterations_ == 1) {
        for (int i = 0; i < number_of_new_interpolation_points; ++i) {
          //DFO_model_.SetFunctionValue(i + 1, function_evaluations[i]);
          DFO_model_.SetFunctionValueAndDerivatives(i + 1, functionValsAndGrad.col(i));
        }
      }

      if (number_of_new_interpolation_points != number_of_interpolation_points_) {
        for (int i = number_of_new_interpolation_points; i < number_of_interpolation_points_; ++i) {
          //DFO_model_.SetFunctionValue(i + 1, function_evaluations[i - number_of_new_interpolation_points]);
          DFO_model_.SetFunctionValueAndDerivatives(i+1, functionValsAndGrad.col(i - number_of_new_interpolation_points));
        }
      }
      //std::cout << "function values \n" << *refFuncVals << "\nend" << std::endl;


      DFO_model_.initializeModel();

      UpdateLastAction(INITIALIZED_MODEL);

      DFO_model_.printQuadraticModel();
      //DFO_model_.shiftCenterPointOfQuadraticModel(-DFO_model_.getCenterPoint());
      //DFO_model_.printQuadraticModel();
      std::cout << "------------Gradients ---------" << std::endl;
      std::cout << *refDerivatives << std::endl;

      Eigen::MatrixXd hess(number_of_variables_, number_of_variables_);
      Eigen::VectorXd grad(number_of_variables_);
      double constant;
      Eigen::MatrixXd ycop = (*refY).block(0,1,2,3);
      Eigen::MatrixXd dercopy = (*refDerivatives).block(0,1,2,3);
      Eigen::VectorXd funccopy = (*refFuncVals).tail(3);
      Eigen::VectorXd gradCp = (*refDerivatives).col(0);
      enhancedModel.ComputeModel( (*refY), (*refDerivatives), gradCp /*DFO_model_.GetGradient()*/, (*refFuncVals),DFO_model_.getCenterPoint(), DFO_model_.GetBestPoint(),
                                 DFO_model_.GetTrustRegionRadius(), r);
      //enhancedModel.ComputeModel( ycop,dercopy, DFO_model_.GetGradient(), funccopy,DFO_model_.getCenterPoint(), DFO_model_.GetBestPoint(),
      //DFO_model_.GetTrustRegionRadius(), r);
      enhancedModel.GetModel(constant, grad,hess);
      std::cout << "------------Enhanced model? ---------" << std::endl;
      std::cout << "c = " << constant << std::endl;
      std::cout << "gradient = " << std::endl << grad << std::endl;
      std::cout << "hessian = " << std::endl << hess << std::endl;
    }
/*
    if (last_action_ == MODEL_IMPROVEMENT_POINT_FOUND) {
      DFO_model_.update(new_point, function_evaluation, index_of_new_point, DFO_Model::INCLUDE_NEW_POINT);
      UpdateLastAction(MODEL_IMPROVEMENT_POINT_ADDED);
    }

    step1:
    if (last_action_ == INITIALIZED_MODEL || last_action_ == TRUST_REGION_RADIUS_UPDATE_STEP
        || last_action_ == MODEL_IMPROVEMENT_POINT_ADDED) {
      if (last_action_ == TRUST_REGION_RADIUS_UPDATE_STEP || last_action_ == MODEL_IMPROVEMENT_POINT_ADDED) {
        DFO_model_.SetTrustRegionRadius(trust_region_radius_inc);
      }
      Eigen::VectorXd gradient = DFO_model_.GetGradient();
      //std::cout <<"gradient of the model: \n"<<gradient<<"\n\n";
      if (gradient.norm() > epsilon_c) {
        UpdateLastAction(CRITICALITY_STEP_FINISHED);
      } else {

        // Kan finne punkt som er utenfor r*delta...



        //std::cout << "new point: \n" << new_point  << "\nindex of new point \n" << index_of_new_point << "\n\n";
        //DFO_model_.SetTrustRegionRadius(trust_region_radius_inc);

        //See if possible to do a cheap replacement (i.e., no need to optimize all lagrange polynomials twice).
        int index_far_away_point = DFO_model_.findPointFarthestAwayFromOptimum();
        double distance = (DFO_model_.GetPoint(index_far_away_point) - DFO_model_.GetBestPoint()).norm();
        bool cheapImprovementPossible = false;
        if (distance > r * DFO_model_.GetTrustRegionRadius()) {
          cheapImprovementPossible =
              DFO_model_.FindPointToReplaceWithPointOutsideScaledTrustRegion(index_far_away_point, new_point);
        }

*/
        /*
        else{
          DFO_model_.findWorstPointInInterpolationSet(new_point, index_of_new_point); //Check if it is lambda-poised.
          if (index_of_new_point == -1){
            UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_FINISHED);
          }
          else{
            UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND);
          }
        }
        */
    /*
        if (cheapImprovementPossible) {
          std::cout << *refY << std::endl;
          std::cout << "Cheap improvement is possible" << std::endl;
          index_of_new_point = index_far_away_point;
          UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND);
        } else {
          DFO_model_.SetTrustRegionRadiusForSubproblem(r * DFO_model_.GetTrustRegionRadius());
          //DFO_model_.findWorstPointInInterpolationSet(new_point, index_of_new_point); //Check if it is lambda-poised.
          bool isPoisedExceptBestPoint =  DFO_model_.FindPointToIncreasePoisedness(new_point, index_of_new_point);
          if (isPoisedExceptBestPoint){
            UpdateLastAction(CRITICALITY_STEP_FINISHED);
          }
          else if (index_of_new_point != -1 || trust_region_radius_inc > u * gradient.norm()) {
            criticality_step_iteration = 0;

            //DFO_model_.findWorstPointInInterpolationSet(new_point,index_of_new_point);
            UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND);

            if (index_of_new_point == -1) {
              UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_FINISHED);
            }
          } else {
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

      UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_POINT_ADDED);

      //See if possible to do a cheap replacement (i.e., no need to optimize all lagrange polynomials twice).
      int index_far_away_point = DFO_model_.findPointFarthestAwayFromOptimum();
      double distance = (DFO_model_.GetPoint(index_far_away_point) - DFO_model_.GetBestPoint()).norm();
      bool cheapImprovementPossible = false;
      if (distance > r * DFO_model_.GetTrustRegionRadius()) {
        cheapImprovementPossible =
            DFO_model_.FindPointToReplaceWithPointOutsideScaledTrustRegion(index_far_away_point, new_point);
      }

      if (cheapImprovementPossible) {
        index_of_new_point = index_far_away_point;
        UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND);

      } else {
        DFO_model_.SetTrustRegionRadiusForSubproblem(r * DFO_model_.GetTrustRegionRadius());
        //DFO_model_.FindPointToIncreasePoisedness(new_point, index_of_new_point);
        bool isPoisedExceptBestPoint =  DFO_model_.FindPointToIncreasePoisedness(new_point, index_of_new_point);
        if (isPoisedExceptBestPoint){
          UpdateLastAction(CRITICALITY_STEP_FINISHED);
        }
        //DFO_model_.findWorstPointInInterpolationSet(new_point, index_of_new_point); //Check if it is lambda-poised.
        else if (index_of_new_point == -1) {
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

    }
    if (last_action_ == MODEL_IMPROVEMENT_ALGORITHM_FINISHED) {
      //We are back to the criticality step.
      trust_region_radius_tilde = pow(w, criticality_step_iteration) * trust_region_radius_inc;

      //Do "convergence test" for the criticality step.
      Eigen::VectorXd gradient = DFO_model_.GetGradient();
      double norm_of_gradient = gradient.norm();
      std::cout << "The norm of the gradient of the model is: " << norm_of_gradient << std::endl;
      if (trust_region_radius_tilde <= u * (norm_of_gradient)) {
        //"converged"
        double temp = max(trust_region_radius_tilde, beta * gradient.norm());
        double new_trust_region_radius = min(temp, trust_region_radius_inc);
        DFO_model_.SetTrustRegionRadius(new_trust_region_radius);

        UpdateLastAction(CRITICALITY_STEP_FINISHED);
      } else { // Reduce the radius and run the model improvement algorithm.
        // Maybe the set is already "well poised" in the new radius???
        std::cout << "decreasing the radius:" << std::endl;
        do {
          criticality_step_iteration++;
          trust_region_radius_tilde = pow(w, criticality_step_iteration) * trust_region_radius_inc;

          std::cout << r * trust_region_radius_tilde << std::endl;
          DFO_model_.SetTrustRegionRadiusForSubproblem(r * trust_region_radius_tilde);
          //DFO_model_.findWorstPointInInterpolationSet(new_point, index_of_new_point);
          DFO_model_.FindPointToIncreasePoisedness(new_point, index_of_new_point);
        } while ((index_of_new_point == -1) && (trust_region_radius_tilde > u * (norm_of_gradient)));

        if (index_of_new_point != -1) {
          UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND);
        }
        else if (trust_region_radius_tilde <= u * (norm_of_gradient)) {
          double temp = max(trust_region_radius_tilde, beta * gradient.norm());
          double new_trust_region_radius = min(temp, trust_region_radius_inc);
          DFO_model_.SetTrustRegionRadius(new_trust_region_radius);
          UpdateLastAction(CRITICALITY_STEP_FINISHED);
        } else {
          std::cout << "whhaaaaat" << std::endl;
          std::cin.get();
        }

        //UpdateLastAction(MODEL_IMPROVEMENT_ALGORITHM_POINT_FOUND);
      }
    }

    if (last_action_ == CRITICALITY_STEP_FINISHED) {
      //Now at step2.
      DFO_model_.SetTrustRegionRadiusForSubproblem(DFO_model_.GetTrustRegionRadius());
      new_point = DFO_model_.FindLocalOptimum();
      UpdateLastAction(TRIAL_POINT_FOUND);

      double maxDistance = DFO_model_.findLargestDistanceBetweenPointsAndOptimum();
      if ((new_point - DFO_model_.GetBestPoint()).norm() < tau * maxDistance) {
        UpdateLastAction(TRIAL_POINT_IS_NOT_NEW_OPTIMUM);
      }
    }
    else if (last_action_ == TRIAL_POINT_FOUND) {
      DFO_model_.SetTrustRegionRadiusForSubproblem(r * DFO_model_.GetTrustRegionRadius());
      int t = DFO_model_.findPointToReplaceWithNewOptimum(new_point);
      rho = (DFO_model_.GetFunctionValue(DFO_model_.getBestPointIndex()) - function_evaluation)
          / (DFO_model_.evaluateQuadraticModel(DFO_model_.GetBestPoint())
              - DFO_model_.evaluateQuadraticModel(new_point));
      Eigen::VectorXd dummyVec(number_of_variables_);
      dummyVec.setZero();
      int dummyInt = 0;
      std::cout << "eta1 = " << eta1 << "\trho = " << rho << "\n";
      DFO_model_.findWorstPointInInterpolationSet(dummyVec, dummyInt); //Check if it is lambda-poised.
      if ((rho >= eta1) || (dummyInt == -1 && rho > 0)) {
        isTrialPointNewOptimum = true;
        DFO_model_.update(new_point, function_evaluation, t, DFO_Model::INCLUDE_NEW_OPTIMUM);
        UpdateLastAction(NEW_POINT_INCLUDED);
        //DFO_model_.printQuadraticModel();
      } else {
        isTrialPointNewOptimum = false;
        UpdateLastAction(TRIAL_POINT_IS_NOT_NEW_OPTIMUM);
        //int t = DFO_model_.findPointFarthestAwayFromOptimum();
        if ((DFO_model_.GetPoint(t) - DFO_model_.GetBestPoint()).norm() > r * DFO_model_.GetTrustRegionRadius() ||
            abs(DFO_model_.ComputeLagrangePolynomial(t, new_point)) > 1) {
          DFO_model_.update(new_point, function_evaluation, t, DFO_Model::INCLUDE_NEW_POINT);
          //last_action_ = NEW_POINT_INCLUDED; //Doing this destroys the AND functionality!
        }
      }
    }
    // Ta hensyn til at Y kan ta inn new point OG I TILLEGG, kjøre "do the model improvement step"
    //
    if (last_action_ == TRIAL_POINT_IS_NOT_NEW_OPTIMUM) {
      /// Do the model improvement step.

      Eigen::VectorXd
          pointsSortedByDistanceFromOptimum = DFO_model_.GetInterpolationPointsSortedByDistanceFromBestPoint();
      DFO_model_.SetTrustRegionRadiusForSubproblem(DFO_model_.GetTrustRegionRadius());
      index_of_new_point = -1;

      Eigen::MatrixXd Ydelete(number_of_variables_, number_of_interpolation_points_);
      for (int j = 0; j < number_of_new_interpolation_points; ++j) {
        Eigen::VectorXd sd = (*refY).col(pointsSortedByDistanceFromOptimum[j] - 1);
        eigen_col(Ydelete, sd, j);
      }
      //std::cout << "Points sorted by distance \n" << Ydelete << std::endl;

      for (int i = 0; i < number_of_interpolation_points_; ++i) {
        int t = pointsSortedByDistanceFromOptimum[i];
        DFO_model_.SetTrustRegionRadiusForSubproblem(DFO_model_.GetTrustRegionRadius());
        new_point = DFO_model_.FindLocalOptimumOfAbsoluteLagrangePolynomial(t);
        if (
            ((DFO_model_.GetPoint(t) - DFO_model_.GetBestPoint()).norm() > r * DFO_model_.GetTrustRegionRadius()) ||
                (abs(DFO_model_.ComputeLagrangePolynomial(t, new_point)) > required_poisedness_)
            ) {

          index_of_new_point = t;
          /// Get the function evaluation. The point should be replaced. OBS OBS
          UpdateLastAction(MODEL_IMPROVEMENT_POINT_FOUND);
          rho = 0;
          break;
        }
      }
      if (rho != 0) {
        std::cin.get();
      }
    }

    if (last_action_ == TRIAL_POINT_IS_NOT_NEW_OPTIMUM || last_action_ == NEW_POINT_INCLUDED
        || last_action_ == MODEL_IMPROVEMENT_POINT_FOUND) {
      // Update the trust-region radius
      if (rho >= eta1) {
        double tmp = std::min(gamma_inc * DFO_model_.GetTrustRegionRadius(), trust_region_radius_max);
        trust_region_radius_inc = 0.2 * DFO_model_.GetTrustRegionRadius() + 0.8 * tmp;
      } else {
        //Check CFL.
        Eigen::VectorXd tmp(number_of_variables_);
        int tmp2;
        DFO_model_.SetTrustRegionRadiusForSubproblem(r * DFO_model_.GetTrustRegionRadius());
        DFO_model_.findWorstPointInInterpolationSet(tmp, tmp2); //Check if it is lambda-poised. (CFL)
        if (tmp2 == -1) {
          // model is CFL -> reduce radius
          trust_region_radius_inc = gamma * DFO_model_.GetTrustRegionRadius();
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
          trust_region_radius_inc =  0.9*DFO_model_.GetTrustRegionRadius();
        }
      }
      if (last_action_ == TRIAL_POINT_IS_NOT_NEW_OPTIMUM || last_action_ == NEW_POINT_INCLUDED) {
        UpdateLastAction(TRUST_REGION_RADIUS_UPDATE_STEP);
        goto step1; // To avoid having fieldopt cancel the optimization because the queue (of new cases) is empty.
      }
    }



*/
    /*

              Get the function evaluation(s)

    */

    if (iterations_ == 0 || (iterations_ == 1 && DFO_model_.isModelInitialized() == false)) {
      /// Get the function evaluations for the first set of interpolation points.
      for (int i = 0; i < new_points.cols(); ++i) {
        //function_evaluations[i] = sphere(new_points.col(i) + DFO_model_.getCenterPoint());
        functionValsAndGrad.col(i) = matyasFunctionWithGradients1(new_points.col(i) + DFO_model_.getCenterPoint());
      }
    } else {
      if (index_of_new_point < 0) {
        if ((last_action_ == TRIAL_POINT_IS_NOT_NEW_OPTIMUM || last_action_ == TRIAL_POINT_FOUND)) {

        } else {
          std::cout
              << "\n \n Should not happen !!! \n A main iterate() finished without providing: \n new point \n index of points to replace \n "
              << std::endl;
          std::cout << "last_action_ is set to: \t" << last_action_ << std::endl;
          std::cin.get();
        }
      }
      /// Get one new point.
      //function_evaluation = sphere(new_point + DFO_model_.getCenterPoint());
      functionValAndGrad = matyasFunctionWithGradients1(new_point + DFO_model_.getCenterPoint());
      if (last_action_ == TRIAL_POINT_FOUND) {
        if (function_evaluation < DFO_model_.GetBestFunctionValueAllTime()) {
          cout << "\033[1;36;mThe new function evaluation is: \033[0m" << function_evaluation << endl << endl;
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


}
}

