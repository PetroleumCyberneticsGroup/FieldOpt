/******************************************************************************
Copyright (C) 2015-2017 Brage S. Kristoffersen <brage_sk@hotmail.com>

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

#ifndef FIELDOPT_NPV_H
#define FIELDOPT_NPV_H

#include "objective.h"
#include "Settings/model.h"
#include "Simulation/results/results.h"

namespace Optimization {
namespace Objective {

class NPV : public Objective {
 public:
/*!
 * \brief NPV
 * \param settings The Settings object from which to create the objective.
 * \param results The Results object from which to get property values.
 */
  NPV(Settings::Optimizer *settings,
      Simulation::Results::Results *results);

  double value(Model::Model::economy) const;

 private:
/*!
 * \brief The Component class is used for internal representation of the components of
 * NPV.
 */
  struct Component {
   public:
    QString property_name;
    double coefficient;
    Simulation::Results::Results::Property property;
    int time_step;
    bool is_well_property;
    int report_time;
    QString well;
    double resolveValue(Simulation::Results::Results *results);
    double resolveValueDiscount(Simulation::Results::Results *results, double time_step);
    double yearlyToMonthly(double discount_factor);
    QString interval;
    double discount;
    bool usediscountfactor;
  };

  QList<Component *> *components_; //!< List of gamma, k pairs.
  Simulation::Results::Results *results_;  //!< Object providing access to simulator results.
  Settings::Optimizer *settings_;
};

}
}
#endif //FIELDOPT_NPV_H
