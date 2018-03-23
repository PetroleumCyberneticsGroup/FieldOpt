//
// Created by pcg1 on 12.01.18.
//


#ifndef FIELDOPT_DFO_H
#define FIELDOPT_DFO_H

// ---------------------------------------------------------
#include "Optimization/optimizer.h"
#include "Subproblem.h"

// ---------------------------------------------------------
namespace Optimization {
namespace Optimizers {

// ---------------------------------------------------------
/*!
 * @brief This is a fantastic description of the DFO method,
 * with references.
 */
class DFO : public Optimizer {
 public:
  DFO(Settings::Optimizer *settings,
      Case *base_case,
      Model::Properties::VariablePropertyContainer *variables,
      Reservoir::Grid::Grid *grid,
      Logger *logger);

  // -------------------------------------------------------
  //TerminationCondition IsFinished() override;
  QString GetStatusStringHeader() const {};
  QString GetStatusString() const {};

 private:
  // -------------------------------------------------------
  Model::Properties::VariablePropertyContainer *varcont_;
  //void iterate() override;

  // -------------------------------------------------------
  void iterate() {
    iterations_++;
    //cout << "just did an ITERATE" << endl;
  };

  // -------------------------------------------------------
  bool is_successful_iteration(){};

  // -------------------------------------------------------
  void handleEvaluatedCase(Case *c) override {
    //cout << "just handled an evaluated case!" << endl;
  };

  // -------------------------------------------------------
  TerminationCondition IsFinished() {
    cout << "JUST CALLED ISFINISHED" << endl;

    //if (iterations_!=0)
      //return TerminationCondition::NOT_FINISHED;
    return TerminationCondition::MAX_EVALS_REACHED;
  };

 protected:

  // -------------------------------------------------------
  //void handleEvaluatedCase(Case *c) override;

 private:

  // -------------------------------------------------------
  double trust_radius_;
  QList<QUuid> realvar_uuid_;
  Settings::Optimizer *settings_;
  int iterations_;

};

}
}

#endif //FIELDOPT_DFO_H
