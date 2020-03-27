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
/*
            std::vector<double> calcPM(std::vector<double> WBHP) const;
            double calcPdis(QList<double> pm) const;
            double calcEffHydraulic(double qwi_per_pump) const;
            double calcPowPerPump(double pdis, double qwi_per_pump, double eff_hydraulic) const;
            QList<double> calcPowWt(std::vector<double> FWPR) const;
            double calcNTurbine(double pow_demand) const;
            double calcCO2EmRate(double pow_generated) const;
            QList<double> calcCum(vector<double, allocator<double>> time, QList<double> rate) const;
*/
            std::vector<double> calc_Pwh(std::vector<double> wbhp, double well_tvd) const;
            double calc_eff_hydraulic(double qwi_per_pump) const;
            double calc_pow_per_pump(double Pdis, double qwi_per_pump, double eff_hydraulic) const;




            double resolveCarbonDioxideCost(vector<double, allocator<double>>) const;
/*
        private:
            double rho_wi_;
            double g_;
            double reservoir_depth_;
            double npump_wi_;
            double psuc_;
            double eff_mechanical_;
            double max_pow_per_pump_;
            double cost_per_pump_;
            double enrg_const_wt_;
            double cost_per_turbine_;
            double unit_cost_wt_;
            double pow_supply_per_turbine_;
            double co2_em_per_enrg_unit_;
            double co2_tax_rate_;
*/
        private:
            double rho_wi_;
            double g_;
            // double tvd_;
            double npump_;
            double Psuc_;
            double eff_mech_;
            double max_qwi_per_pump_;
            double pen_max_qwi_per_pump_;
            double enrg_consump_water_treatment_;
            double pow_gen_per_turbine_;
            double cost_per_turbine_;
            double CO2_emit_per_unit_enrg_;
            double CO2_tax_rate_;
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
#endif //FIELDOPT_carbondioxidenpv_H
