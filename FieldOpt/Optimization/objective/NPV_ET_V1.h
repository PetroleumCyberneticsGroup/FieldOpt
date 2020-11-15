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

#ifndef FIELDOPT_NPV_ET_V1_H
#define FIELDOPT_NPV_ET_V1_H

#include "objective.h"
#include "Settings/model.h"
#include "Simulation/results/results.h"

namespace Optimization {
namespace Objective {

class NPV_ET_V1 : public Objective {
        public:
/*!
 * \brief carbondioxidenpv
 * \param settings The Settings object from which to create the objective.
 * \param results The Results object from which to get property values.
 */
            NPV_ET_V1(Settings::Optimizer *settings,
                             Simulation::Results::Results *results,
                             Model::Model *model);

            double calc_iwhp (double ibhp, double itvd) const;

            std::vector<double> calc_opt_PS (double q_required, double h_required) const;

            double calc_eff_turbine_part_load (double P_turbine) const;

            QHash<QString, double> calc_ET() const;

            double calc_NPV() const;

            double value() const;

            QHash<QUuid, double> mpso_id_ofv(const QHash<QUuid, double> &mpso_id_r_CO2) const;

        private:
            double p_atm;
            double rho_wi;
            double g;
            std::vector<double> h_req;
            std::vector<double> q_req;
            std::vector<std::vector<double>> table_opt_q_PS;
            std::vector<std::vector<double>> table_opt_np;
            std::vector<std::vector<double>> table_opt_ns;
            std::vector<std::vector<double>> table_opt_h_PS;
            std::vector<std::vector<double>> table_opt_eff_hydraulic_PS;
            std::vector<std::vector<double>> table_opt_P_PS;
            double penalty_constant;
            double p_PS_inlet;
            double unit_cost_pump;
            double e_water_treatment;
            double P_turbine_full_load;
            double unit_cost_turbine;
            double eff_turbine_full_load;
            double c_energy_fuel;
            double p_gas;
            double c_CO2_fuel;
            double r_CO2;

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
                double resolveValue(Simulation::Results::Results *results);
                double resolveValueDiscount(Simulation::Results::Results *results, double time_step);
                double yearlyToMonthly(double discount_factor);
//                std::vector<double> resolveValueVector(Simulation::Results::Results *results, vector<double, allocator<double>> report_times);

                std::string interval;
                double discount;
                bool usediscountfactor;
                bool is_json_component;
                bool is_well_property;
                QString well;
                double well_tvd;
            };

            QList<Component *> *components_; //!< List of gamma, k pairs.
            QList<Component *> *carboncomponents_;
            Simulation::Results::Results *results_;  //!< Object providing access to simulator results.
            Settings::Optimizer *settings_;
            Model::Model *model_;
            Model::Model::Economy *well_economy_;
            //double carbonCost;
        };

    }
}
#endif //FIELDOPT_NPV_ET_V1_H
