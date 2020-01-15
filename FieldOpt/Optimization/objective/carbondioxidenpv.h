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

#ifndef FIELDOPT_carbondioxidenpv_H
#define FIELDOPT_carbondioxidenpv_H

#include "objective.h"
#include "Settings/model.h"
#include "Simulation/results/results.h"

namespace Optimization {
    namespace Objective {

        class carbondioxidenpv : public Objective {
        public:
/*!
 * \brief carbondioxidenpv
 * \param settings The Settings object from which to create the objective.
 * \param results The Results object from which to get property values.
 */
            carbondioxidenpv(Settings::Optimizer *settings,
                Simulation::Results::Results *results,
                Model::Model *model);

            double value() const;

            double resolveCarbonDioxideCost() const;

        private:
/*!
 * \brief The Component class is used for internal representation of the components of
 * NPV.
 */
            struct Component {
            public:
                std::string property_name;
                double coefficient;
                Simulation::Results::Results::Property property;
                int time_step;
                std::string well;
                double resolveValue(Simulation::Results::Results *results);
                double resolveValueDiscount(Simulation::Results::Results *results, double time_step);
                double yearlyToMonthly(double discount_factor);
                std::string interval;
                double discount;
                bool usediscountfactor;
                bool is_json_component;
            };

            QList<Component *> *components_; //!< List of gamma, k pairs.
            Simulation::Results::Results *results_;  //!< Object providing access to simulator results.
            Settings::Optimizer *settings_;
            Model::Model::Economy *well_economy_;
            double carbonCost;
        };

    }
}
#endif //FIELDOPT_carbondioxidenpv_H
