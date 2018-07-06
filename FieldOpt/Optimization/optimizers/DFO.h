//
// Created by pcg1 on 12.01.18.
//

#ifndef FIELDOPT_DFO_H
#define FIELDOPT_DFO_H

#include "Optimization/optimizer.h"
#include "Subproblem.h"
#include "DFO_Model.h"
#include <ncurses.h>
#include <fstream>
#include <iostream>

namespace Optimization {
namespace Optimizers {

/*!
 * @brief This is a fantastic description of the DFO method, with references.
 */
class DFO : public Optimizer {
 public:
  DFO(Settings::Optimizer *settings,
      Case *base_case,
      Model::Properties::VariablePropertyContainer *variables,
      Reservoir::Grid::Grid *grid,
      Logger *logger);

  //TerminationCondition IsFinished() override;
  QString GetStatusStringHeader() const {};
  QString GetStatusString() const {};
  QList<Case *> ConvertPointsToCases(Eigen::MatrixXd points);

  void UpdateLastAction(int a);
  void set_next_step(int a);

  Eigen::VectorXd ScaleVariablesFromAlgorithmToApplication(Eigen::VectorXd point);
  Eigen::VectorXd ScaleVariablesFromApplicationToAlgorithm(Eigen::VectorXd point);
  void WritePointToFile(Eigen::VectorXd point, int t);
  void WritePointTypeToFile(int t);
  void CreateFiles();


 private:
  std::string filenamePoint;
  std::string filenameType;
  std::string filenameTrr;

  std::string color_from= "31";
  std::string color_to = "33";
  Model::Properties::VariablePropertyContainer *varcont_;
  DFO_Model DFO_model_;
  void iterate() override;
  int number_of_interpolation_points_;
  int number_of_variables_;
  Optimization::Case *base_case_;
  int last_action_;
  int next_step_;
  std::string getActionName(int a);
  std::string get_action_name(int a);
  Eigen::VectorXd weights_distance_from_optimum_lsq_;


  /*
  void iterate() {
    iterations_++;
    //cout << "just did an ITERATE" << endl;
  };
   */
  bool is_successful_iteration(){};


  void handleEvaluatedCase(Case *c);

  TerminationCondition IsFinished() {
    cout << "JUST CALLED ISFINISHED" << endl;

    //if (iterations_!=0)
      //return TerminationCondition::NOT_FINISHED;
    return TerminationCondition::NOT_FINISHED;
  };


 protected:
  //void handleEvaluatedCase(Case *c) override;

 private:
  int previous_iterate_type_;
  double initial_trust_region_radius_;
  double required_poisedness_;
  QList<QUuid> realvar_uuid_;
  Settings::Optimizer *settings_;
  int iterations_;



};

}
}

#endif //FIELDOPT_DFO_H
