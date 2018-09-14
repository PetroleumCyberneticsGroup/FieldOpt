//
// Created by bragesk on 11.09.18.
//

#ifndef FIELDOPT_NPV_H
#define FIELDOPT_NPV_H

#include "objective.h"
#include "Settings/model.h"
#include "Simulation/results/results.h"

namespace Optimization {
    namespace Objective {

        class NPV : public Objective
        {
        public:
/*!
 * \brief WeightedSum
 * \param settings The Settings object from which to create the objective.
 * \param results The Results object from which to get property values.
 */
            NPV(Settings::Optimizer *settings,
                Simulation::Results::Results *results);

            double value() const;

        private:
/*!
 * \brief The Component class is used for internal representation of the components of
 * NPV.
 */
            class Component {
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
