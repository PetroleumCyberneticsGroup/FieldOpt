//
// Created by bellout on 3/22/18.
//

// ---------------------------------------------------------
#ifndef FIELDOPT_IWD_CONSTRAINT_H
#define FIELDOPT_IWD_CONSTRAINT_H

// ---------------------------------------------------------
// FIELDOPT: SNOPT
#include "../optimizers/SNOPTSolver.h"


//namespace Optimization {
//namespace Optimizers {
//class SNOPTSolver;
//}
//}

// -----------------------------------------------------------------
class Logger;

// ---------------------------------------------------------
#include <Reservoir/grid/grid.h>
#include "constraint.h"
//#include "well_spline_length.h"
//#include "interwell_distance.h"

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// ---------------------------------------------------------
class IWDConstraint : public Constraint {
 public:

  // -------------------------------------------------------
  // Constructor
  IWDConstraint(Settings::Optimizer* settings,
  Model::Properties::VariablePropertyContainer *variables);

  // -------------------------------------------------------
  string name() override { return "IWDConstraint"; }

  // -------------------------------------------------------
  // Input var to SNOPTSolver
  Settings::Optimizer *settings_;
  Model::Properties::VariablePropertyContainer *variables_;
  Case *current_case_;
  Reservoir::Grid::Grid *grid_;
  Logger* logger_;

  Optimization::Optimizers::SNOPTSolver* SNOPTSolver_;

  // -------------------------------------------------------
  // Constraint interface
 public:
  bool CaseSatisfiesConstraint(Case *c) override;
  void SnapCaseToConstraints(Case *c) override;

 private:
  // -------------------------------------------------------
  int max_iterations_;
//  QList<WellSplineLength *> length_constraints_;
//  InterwellDistance *distance_constraint_;

};

}
}

#endif //FIELDOPT_IWD_CONSTRAINT_H
