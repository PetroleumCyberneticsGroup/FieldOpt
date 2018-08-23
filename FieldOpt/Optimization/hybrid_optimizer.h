/******************************************************************************
   Copyright (C) 2015-2018 Einar J.M. Baumann <einar.baumann@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef FIELDOPT_HYBRID_OPTIMIZER_H
#define FIELDOPT_HYBRID_OPTIMIZER_H

#include "optimizer.h"

namespace Optimization {

/*!
 * The HybridOptimizer class encapsulates two other _component_ Optimizer objects in order to create
 * a hybridized optimizer.
 *
 * Each of the two other algorithms act as they normally would, but their behaviour is
 * masked from the runner by this class.
 *
 * When a new case is requested from the HybridOptimizer class, it passes the request down
 * to one of the components. Which one is called depends one which one is currently active.
 *
 * For now, switching between the two components is only done through termination conditions.
 * When the termination condition for the first component is reached, the HybridOptimizer
 * will instantiate the second optimization algorithm and request new cases from it instead.
 *
 * The termination condition for the HybridOptimizer class is: Component_1.IsFinished() && Component_2.IsFinished()
 */
class HybridOptimizer : public Optimizer {
 public:
  HybridOptimizer(
      Settings::Optimizer *settings,
      Case *base_case,
      Model::Properties::VariablePropertyContainer *variables,
      Reservoir::Grid::Grid *grid,
      Logger *logger);

  TerminationCondition IsFinished() override;
 protected:
  void handleEvaluatedCase(Case *c) override;
  void iterate() override;

 private:
  int active_component_; //!< Currently active component (0=primary; 1=secondary)
  Optimizer *primary_; //!< Primary optimizer (will be called first).
  Optimizer *secondary_; //!< Secondary optimizer (will be called after the termination of the first component is reached).

  Settings::Optimizer *primary_settings_;
  Settings::Optimizer *secondary_settings_;

  Model::Properties::VariablePropertyContainer *variables_;
  Reservoir::Grid::Grid *grid_;

  /*!
   * Initialize a component algorithm.
   * @param component Which component to initialize (0=primary, 1=secondary).
   */
  void initializeComponent(int component);

};

}

#endif //FIELDOPT_HYBRID_OPTIMIZER_H
