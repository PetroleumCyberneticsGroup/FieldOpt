//
// Created by joakim on 12.06.18.
//

#include "VirtualSimulator.h"
VirtualSimulator::VirtualSimulator(std::string problemFile) {
  problem = problemFile;
  nl.import_nl(problem);

  std::vector<casadi::MX> f1 = {nl.f};
  std::vector<casadi::MX> f2 = {casadi::MX::vertcat(nl.x)};
  f = casadi::Function("obj", f2, f1);
  fj = f.factory("jacf", {f.name_in()},{"jac:o0:i0","o0"});

  auto gcat = {casadi::MX::vertcat(nl.g)};
  g = casadi::Function("obj", f2, gcat);
  gj = g.factory("jacg", {f.name_in()},{"jac:o0:i0","o0"});
  input = {casadi::DM(nl.x_init)}; // set the correct size of input.

  m_ = GetNumberOfConstraints();
}
VirtualSimulator::VirtualSimulator() {

}

Eigen::VectorXd VirtualSimulator::evaluateConstraints(Eigen::VectorXd point) {
  input[0] = convertEigenMatrixToCasadi(point);
  auto out =  g(input);
  //std::cout  << "from vs: \n" << out[0] << "\n";
  Eigen::VectorXd output;
  if (m_ > 0){
    output = convertCasadiMatrixToEigen(out[0]);

  }
  else{
    output = Eigen::MatrixXd::Zero(0,0);
  }
  return output;
}

Eigen::MatrixXd VirtualSimulator::evaluateConstraintGradients(Eigen::VectorXd point) {
  input[0] = convertEigenMatrixToCasadi(point);
  auto out_gj = gj(input);
  Eigen::MatrixXd output = convertCasadiMatrixToEigen(out_gj[0]);
  return output;
}

double VirtualSimulator::evaluateFunction(Eigen::VectorXd point) {
  input[0] = convertEigenMatrixToCasadi(point);
  auto out =  f(input);
  Eigen::VectorXd output = convertCasadiMatrixToEigen(out[0]);
  return output[0];
}

Eigen::VectorXd VirtualSimulator::evaluateFunctionGradients(Eigen::VectorXd point) {
  input[0] = convertEigenMatrixToCasadi(point);
  auto out_fj = fj(input);
  Eigen::VectorXd output = (convertCasadiMatrixToEigen(out_fj[0])).transpose();

  return output;
}

casadi::DM VirtualSimulator::convertEigenMatrixToCasadi(Eigen::MatrixXd point){
  casadi::DM casadiVector;
  size_t rows = point.rows();
  size_t cols = point.cols();

  casadiVector.resize(rows,cols);
  casadiVector = casadi::DM::zeros(rows,cols);
  std::memcpy(casadiVector.ptr(), point.data(), sizeof(double)*rows*cols);

  return casadiVector;
}

Eigen::MatrixXd VirtualSimulator::convertCasadiMatrixToEigen(casadi::DM casadiMatrix) {
  auto casadiMatrixDense = casadi::DM::densify(casadiMatrix);
  Eigen::MatrixXd eigenMatrix;
  size_t rows = casadiMatrixDense.size1();
  size_t cols = casadiMatrixDense.size2();
  eigenMatrix.resize(rows,cols);
  eigenMatrix.setZero(rows,cols);
  std::memcpy(eigenMatrix.data(), casadiMatrixDense.ptr(), sizeof(double)*rows*cols);

  return eigenMatrix;
}
int VirtualSimulator::GetNumberOfVariables() {
  return (nl.x_init).size();;
}
int VirtualSimulator::GetNumberOfConstraints() {
  return (nl.g_ub).size();;
}
Eigen::VectorXd VirtualSimulator::GetLowerBoundsForVariables() {
  Eigen::VectorXd lb(GetNumberOfVariables());
  for (int i = 0; i<lb.rows(); ++i){
    lb(i) = nl.x_lb[i];
  }
  return lb;
}

Eigen::VectorXd VirtualSimulator::GetUpperBoundsForVariables() {
  Eigen::VectorXd ub(GetNumberOfVariables());
  for (int i = 0; i<ub.rows(); ++i){
    ub(i) = nl.x_ub[i];
  }
  return ub;
}
Eigen::VectorXd VirtualSimulator::GetLowerBoundsForConstraints() {
  //std::cout << "Lower bounds (straight from casadi):\n" << nl.g_lb <<"\n";
  Eigen::VectorXd lb(GetNumberOfConstraints());
  for (int i = 0; i<lb.rows(); ++i){
    lb(i) = nl.g_lb[i];
  }
  //std::cout << "and as an eigen vector:\n" << lb <<"\n";

  return lb;
}

Eigen::VectorXd VirtualSimulator::GetUpperBoundsForConstraints() {
  //std::cout << "Upper bounds (straight from casadi):\n" << nl.g_ub <<"\n";
  Eigen::VectorXd ub(GetNumberOfConstraints());
  for (int i = 0; i<ub.rows(); ++i){
    ub(i) = nl.g_ub[i];
  }
  //std::cout << "and as an eigen vector:\n" << ub <<"\n";
  return ub;
}
bool VirtualSimulator::IsFeasiblePoint(Eigen::VectorXd point) {
  if (GetNumberOfConstraints() <= 0){
    return true;
  }
  else{
  Eigen::VectorXd constraints = evaluateConstraints(point);
  for (int i = 0; i < GetNumberOfConstraints(); ++i){
    if (constraints[i] < nl.g_lb[i]*0.9 || constraints[i] > 1.1*nl.g_ub[i]){
      return false;
    }
  }
  }
  return true;
}
Eigen::VectorXd VirtualSimulator::GetInitialPoint() {
  Eigen::VectorXd x_init(GetNumberOfVariables());
  for (int i = 0; i<x_init.rows(); ++i){
    x_init(i) = nl.x_init[i];
  }
  return x_init;
}
Eigen::VectorXd VirtualSimulator::Solve() {

  // Set options
  casadi::Dict opts;
  opts["expand"] = true;
  //  opts["max_iter"] = 10;
  //  opts["verbose"] = true;
  //  opts["linear_solver"] = "ma57";
  //  opts["hessian_approximation"] = "limited-memory";
  //  opts["derivative_test"] = "second-order";

  // Allocate NLP solver and buffers

  casadi::Function solver = casadi::nlpsol("nlpsol", "ipopt", nl, opts);

  std::map<std::string, casadi::DM> arg, res;

  // Solve NLP
  arg["lbx"] = nl.x_lb;
  arg["ubx"] = nl.x_ub;
  arg["lbg"] = nl.g_lb;
  arg["ubg"] = nl.g_ub;
  arg["x0"] = nl.x_init;
  res = solver(arg);

  for (auto&& s : res) {
    std::cout << std::setw(10) << s.first << ": " << std::vector<double>(s.second) << std::endl;
  }
  //std::cout << res["x"] << "\n";
  double fval = (convertCasadiMatrixToEigen(res["f"])(0));
  Eigen::VectorXd ans = convertCasadiMatrixToEigen(res["x"]);

  std::cout << "========================= THE ANSWERS ======================\n";
  std::cout << "fval = " << fval <<"\n";
  std::cout << "x = \n" << ans << "\n";
  return ans;


}




// convert constraint values from casadi vector into
