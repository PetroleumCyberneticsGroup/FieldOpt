//
// Created by pcg1 on 12.01.18.
//

#ifndef FIELDOPT_DFO_H
#define FIELDOPT_DFO_H

#include "Optimization/optimizer.h"

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
 private:
  //void iterate() override;
  void iterate() {
    cout << "just did an ITERATE" << endl;
  };
  bool is_successful_iteration(){};

  void handleEvaluatedCase(Case *c) override {
    cout << "just handled an evaluated case!" << endl;
  };
  TerminationCondition IsFinished() {
    return TerminationCondition::NOT_FINISHED;
  };


 protected:
  //void handleEvaluatedCase(Case *c) override;

 private:
  double trust_radius_;
  QList<QUuid> realvar_uuid_;
  Settings::Optimizer *settings_;



};

}
}

#endif //FIELDOPT_DFO_H
