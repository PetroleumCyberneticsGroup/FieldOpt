/******************************************************************************
Copyright (C) 2020 I Gusti Agung Gede Angga <i.g.a.g.angga@ntnu.no>

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
#include "NPV_ET_V1.h"

#include <iostream>
#include <iomanip>
#include "weightedsum.h"
#include <stdlib.h>
#include <cmath>
#include "Model/model.h"
#include "Model/wells/well.h"
#include <Utilities/printer.hpp>

using std::cout;
using std::endl;
using std::fixed;
using std::setprecision;
using std::scientific;

namespace Optimization {
    namespace Objective {

            NPV_ET_V1::NPV_ET_V1(Settings::Optimizer *settings,
                                           Simulation::Results::Results *results,
                                           Model::Model *model) {

            settings_ = settings;
            results_ = results;
            model_ = model;

            components_ = new QList<NPV_ET_V1::Component *>();
            carboncomponents_ = new QList<NPV_ET_V1::Component *>();

            for (int i = 0; i < settings->objective().NPV_sum.size(); ++i) {

                auto *comp = new NPV_ET_V1::Component();
                if (settings->objective().NPV_sum[i].property.compare(0, 4, "EXT-") == 0) {
                    comp->is_json_component = true;
                    Printer::ext_info("Adding external NPV component.", "Optimization", "NPV_ET_V1");
                    comp->property_name = settings->objective().NPV_sum[i].property.substr(4, std::string::npos);
                    comp->interval = settings->objective().NPV_sum[i].interval;
                } else {
                    comp->is_json_component = false;
                    comp->property_name = settings->objective().NPV_sum.at(i).property;
                    comp->property = results_->GetPropertyKeyFromString(QString::fromStdString(comp->property_name));
                }

                comp->coefficient = settings->objective().NPV_sum.at(i).coefficient;
                if (settings->objective().NPV_sum.at(i).usediscountfactor == true) {
                    comp->interval = settings->objective().NPV_sum.at(i).interval;
                    comp->discount = settings->objective().NPV_sum.at(i).discount;
                    comp->usediscountfactor = settings->objective().NPV_sum.at(i).usediscountfactor;
                } else {
                    comp->interval = "None";
                    comp->discount = 0;
                    comp->usediscountfactor = false;
                }

                components_->append(comp);
            }

            for (int i = 0; i < settings->objective().NPVCarbonComponents.size(); ++i){
                auto *carboncomp = new NPV_ET_V1::Component();
                carboncomp->property_name = settings->objective().NPVCarbonComponents.at(i).property.toStdString();
                carboncomp->property = results_->GetPropertyKeyFromString(QString::fromStdString(carboncomp->property_name));
                carboncomp->is_well_property = settings->objective().NPVCarbonComponents.at(i).is_well_prop;
                if (carboncomp->is_well_property == true){
                    carboncomp->well = settings->objective().NPVCarbonComponents.at(i).well;
                    carboncomp->well_tvd = settings->objective().NPVCarbonComponents.at(i).well_tvd;
                }
                carboncomponents_->append(carboncomp);
            }

            well_economy_ = model->wellCostConstructor();

            p_atm = settings->objective().p_atm;                                    // unit: barsa
            rho_wi = settings->objective().rho_wi;                                  // unit: kg/m3
            g = settings->objective().g;                                            // unit: m/s2
            h_req = settings->objective().h_req;                                    // unit: m
            q_req = settings->objective().q_req;                                    // unit: m3/d
            table_opt_q_PS = settings->objective().table_opt_q_PS;                  // unit: m3/d
            table_opt_np = settings->objective().table_opt_np;                      // unit: -
            table_opt_ns = settings->objective().table_opt_ns;                      // unit: -
            table_opt_h_PS = settings->objective().table_opt_h_PS;                  // unit: m
            table_opt_eff_hydraulic_PS = settings->objective().table_opt_eff_hydraulic_PS;      // unit: fraction
            table_opt_P_PS = settings->objective().table_opt_P_PS;                  // unit: MW
            penalty_constant = settings->objective().penalty_constant;              // unit: USD
            p_PS_inlet = settings->objective().p_PS_inlet;                          // unit: barsa
            unit_cost_pump = settings->objective().unit_cost_pump;                  // unit: USD/pump
            e_water_treatment = settings->objective().e_water_treatment;            // unit: MWh/m3 produced water
            P_turbine_full_load = settings->objective().P_turbine_full_load;        // unit: MW/turbine
            unit_cost_turbine = settings->objective().unit_cost_turbine;            // unit: USD/turbine
            eff_turbine_full_load = settings->objective().eff_turbine_full_load;    // unit: fraction
            c_energy_fuel = settings->objective().c_energy_fuel;                    // unit: kWh/kg fuel
            p_gas = settings->objective().p_gas;                                    // unit: USD/MMBtu
            c_CO2_fuel = settings->objective().c_CO2_fuel;                          // unit: kg CO2/ kg fuel
            r_CO2 = settings->objective().r_CO2;                                    // unit: USD/ton CO2
            }

        double NPV_ET_V1::calc_iwhp (double ibhp, double itvd) const
        {
            // Assumptions:
            //    No pressure loss due to friction
            // Units:
            //    iwhp          : barsa
            //    ibhp          : barsa
            //    rho_wi        : kg/m3
            //    g             : m/s2
            //    itvd          : m

            double iwhp = ibhp - rho_wi * g * itvd / pow(10.0,5.0);
            return iwhp;
        }

        std::vector<double> NPV_ET_V1::calc_opt_PS (double q_required, double h_required) const
        {
            int h_idx;

            if (h_req[0] == h_required) {
                h_idx = 0;
            }

            else if ((h_req[0] < h_required) && (h_required <= h_req[h_req.size() - 1])) {
                for (int i = 1; i < h_req.size(); i++) {
                    if ((h_req[i - 1] < h_required) && (h_required <= h_req[i])) {
                        h_idx = i;
                        break;
                    }
                }
            }

            else if (h_req[h_req.size() - 1] < h_required) {
                h_idx = h_req.size() - 1;
            }

            int q_idx;

            if (q_req[0] == q_required) {
                q_idx = 0;
            }

            else if ((q_req[0] < q_required) && (q_required <= q_req[q_req.size() - 1])) {
                for (int j = 1; j < q_req.size(); j++) {
                    if ((q_req[j - 1] < q_required) && (q_required <= q_req[j])) {
                        q_idx = j;
                        break;
                    }
                }
            }

            else if (q_req[q_req.size() - 1] < q_required) {
                q_idx = q_req.size() - 1;
            }

            double opt_q_PS = table_opt_q_PS[h_idx][q_idx];
            double opt_np = table_opt_np[h_idx][q_idx];
            double opt_ns = table_opt_ns[h_idx][q_idx];
            double opt_h_PS = table_opt_h_PS[h_idx][q_idx];
            double opt_eff_hydraulic_PS = table_opt_eff_hydraulic_PS[h_idx][q_idx];
            double opt_P_PS = table_opt_P_PS[h_idx][q_idx];

            std::vector<double> opt_PS;
            opt_PS.push_back(opt_q_PS);
            opt_PS.push_back(opt_np);
            opt_PS.push_back(opt_ns);
            opt_PS.push_back(opt_h_PS);
            opt_PS.push_back(opt_eff_hydraulic_PS);
            opt_PS.push_back(opt_P_PS);

            return opt_PS;
        }

        double NPV_ET_V1::calc_eff_turbine_part_load (double P_turbine) const
        {
            // Units:
            //    eff_turbine_part_load     : fraction
            //    P_turbine                 : MW
            // Note: based on Kehlhofer

            double P_ratio = P_turbine / P_turbine_full_load;

            double eff_ratio = 1.445 * pow(P_ratio, 5) - 5.106 * pow(P_ratio, 4) + 7.218 * pow(P_ratio, 3)
                    - 5.723 * pow(P_ratio, 2) + 3.165 * P_ratio + 0.001;

            double eff_turbine_part_load = eff_ratio * eff_turbine_full_load;

            return eff_turbine_part_load;
        }

        double NPV_ET_V1::calc_ET() const
        {
            auto time = results_->GetValueVector(results_->Time);

            std::vector<std::vector<double>> ibhp;
            std::vector<double> itvd;
            std::vector<string> iname;
            std::vector<double> fwir;
            std::vector<double> fwpr;
            for (int j = 0; j < carboncomponents_->size(); j++) {
                if(carboncomponents_->value(j)->is_well_property == true) {
                    ibhp.push_back(results_->GetValueVector(Simulation::Results::Results::Property::WellBottomHolePressure,
                                                                carboncomponents_->value(j)->well));
                    itvd.push_back(carboncomponents_->value(j)->well_tvd);
                    iname.push_back(carboncomponents_->value(j)->well.toStdString());
                }
                else if (carboncomponents_->at(j)->property_name == "WaterInjectionRate") {
                    fwir = results_->GetValueVector(carboncomponents_->at(j)->property);
                }
                else if (carboncomponents_->at(j)->property_name == "WaterProductionRate") {
                    fwpr = results_->GetValueVector(carboncomponents_->at(j)->property);
                }
            }

            std::vector<std::vector<double>> iwhp;
            for (int well_idx = 0; well_idx < iname.size(); well_idx++) {
                std::vector<double> row;
                for (int time_idx = 0; time_idx < time.size(); time_idx++) {
                    double col = calc_iwhp(ibhp[well_idx][time_idx], itvd[well_idx]);

                    if (col >= p_atm) {
                        row.push_back(col);
                    } else if (col < p_atm) {
                        row.push_back(p_atm);
                    }
                }
                iwhp.push_back(row);
            }

            std::vector<double> p_manifold;
            for (int time_idx = 0; time_idx < time.size(); time_idx++) {
                double temp = 0;
                for (int well_idx = 0; well_idx < iname.size(); well_idx++) {
                    if (iwhp[well_idx][time_idx] > temp) {
                        temp = iwhp[well_idx][time_idx];
                    }
                }
                p_manifold.push_back(temp);
            }

            std::vector<double> p_PS_outlet = p_manifold;

            std::vector<double> h_required;
            for (int time_idx = 0; time_idx < time.size(); time_idx++) {
                double temp = (p_PS_outlet[time_idx] - p_PS_inlet) * pow(10, 5) / (rho_wi * g);
                h_required.push_back(temp);
            }

            std::vector<double> q_required = fwir;

            std::vector<double> opt_q_PS;
            std::vector<double> opt_np;
            std::vector<double> opt_ns;
            std::vector<double> opt_h_PS;
            std::vector<double> opt_eff_hydraulic_PS;
            std::vector<double> opt_P_PS;
            for (int time_idx = 0; time_idx < time.size(); time_idx++) {
                std::vector<double> opt_PS = calc_opt_PS(q_required[time_idx], h_required[time_idx]);
                opt_q_PS.push_back(opt_PS[0]);
                opt_np.push_back(opt_PS[1]);
                opt_ns.push_back(opt_PS[2]);
                opt_h_PS.push_back(opt_PS[3]);
                opt_eff_hydraulic_PS.push_back(opt_PS[4]);
                opt_P_PS.push_back(opt_PS[5]);
            }

            double penalty = 0;
            for (int time_idx = 0; time_idx < time.size(); time_idx++) {
                if ((opt_q_PS[time_idx] == 0) && (opt_np[time_idx] == 0) && (opt_ns[time_idx] == 0)) {
                    penalty = penalty_constant;
                    break;
                }
            }

            double max_n_pump_parallel = 0;
            double max_n_pump_series = 0;
            for (int time_idx = 0; time_idx < time.size(); time_idx++) {
                if (opt_np[time_idx] > max_n_pump_parallel) {
                    max_n_pump_parallel = opt_np[time_idx];
                }
                if (opt_ns[time_idx] > max_n_pump_series) {
                    max_n_pump_series = opt_ns[time_idx];
                }
            }

            double cost_pump = max_n_pump_parallel * max_n_pump_series * unit_cost_pump;

            std::vector<double> P_water_treatment;
            for (int time_idx = 0; time_idx < time.size(); time_idx++) {
                double temp = e_water_treatment * fwpr[time_idx] / 24;
                P_water_treatment.push_back(temp);
            }

            std::vector<double> P_demand;
            for (int time_idx = 0; time_idx < time.size(); time_idx++) {
                double temp = opt_P_PS[time_idx] + P_water_treatment[time_idx];
                P_demand.push_back(temp);
            }

            std::vector<double> n_turbine;
            for (int time_idx = 0; time_idx < time.size(); time_idx++) {
                double temp = ceil((P_demand[time_idx] / P_turbine_full_load));
                n_turbine.push_back(temp);
            }

            double max_n_turbine = 0;
            for (int time_idx = 0; time_idx < time.size(); time_idx++) {
                if (n_turbine[time_idx] > max_n_turbine) {
                    max_n_turbine = n_turbine[time_idx];
                }
            }

            double cost_turbine = max_n_turbine * unit_cost_turbine;

            std::vector<double> P_turbine;
            for (int time_idx = 0; time_idx < time.size(); time_idx++) {
                double temp;
                if (n_turbine[time_idx] == 0) {
                    temp = 0;
                } else {
                    temp = P_demand[time_idx] / n_turbine[time_idx];
                }
                P_turbine.push_back(temp);
            }

            std::vector<double> eff_turbine_part_load;
            for (int time_idx = 0; time_idx < time.size(); time_idx++) {
                double temp = calc_eff_turbine_part_load(P_turbine[time_idx]);
                eff_turbine_part_load.push_back(temp);
            }

            std::vector<double> q_mass_fuel;
            for (int time_idx = 0; time_idx < time.size(); time_idx++) {
                double temp = (P_demand[time_idx] / (eff_turbine_part_load[time_idx] * c_energy_fuel)) * 24;
                q_mass_fuel.push_back(temp);
            }

            std::vector<double> G_mass_fuel;
            G_mass_fuel.push_back(0);
            for (int time_idx = 1; time_idx < time.size(); time_idx++) {
                double temp = G_mass_fuel[time_idx - 1] + q_mass_fuel[time_idx] * (time[time_idx] - time[time_idx - 1]);
                G_mass_fuel.push_back(temp);
            }

            double cost_fuel = G_mass_fuel[time.size() - 1] * c_energy_fuel * p_gas * 1000 / 293.071;

            std::vector<double> q_mass_CO2;
            for (int time_idx = 0; time_idx < time.size(); time_idx++) {
                double temp = q_mass_fuel[time_idx] * c_CO2_fuel;
                q_mass_CO2.push_back(temp);
            }

            std::vector<double> G_mass_CO2;
            G_mass_CO2.push_back(0);
            for (int time_idx = 1; time_idx < time.size(); time_idx++) {
                double temp = G_mass_CO2[time_idx - 1] + q_mass_CO2[time_idx] * (time[time_idx] - time[time_idx - 1]);
                G_mass_CO2.push_back(temp);
            }

            double t_CO2 = G_mass_CO2[time.size() - 1] * r_CO2;

            double ET = penalty + cost_pump + cost_turbine + cost_fuel + t_CO2;

            std::cout << "penalty: .........................." << penalty << endl;
            std::cout << "cost_pump: ........................" << cost_pump << endl;
            std::cout << "cost_turbine: ....................." << cost_turbine << endl;
            std::cout << "cost_fuel: ........................" << cost_fuel << endl;
            std::cout << "t_CO2: ............................" << t_CO2 << endl;
            std::cout << "ET: ..............................." << ET << endl;

            return ET;
        }

        double NPV_ET_V1::calc_NPV() const
        {
            try {
                double NPV = 0;

                auto report_times = results_->GetValueVector(results_->Time);
                std::vector<std::vector<double>> NPV_times;
                std::vector<std::vector<double>> discount_factor_list;

                for (int k = 0; k < components_->size(); ++k) {
                    auto curr_comp = components_->at(k);
                    std::vector<double> NPV_times_temp;
                    std::vector<double> discount_factor_temp;

                    if (curr_comp->is_json_component) { continue; }

                    if (curr_comp->interval == "Yearly") {
                        double discount_factor;
                        for (int i = 0; i < report_times.size(); i++) {
                            if (i < report_times.size() - 1 &&  (report_times[i+1] - report_times[i]) > 365) {
                                std::stringstream ss;
                                ss << "Skipping assumed pre-simulation time step " << report_times[i]
                                   << ". Next time step: " << report_times[i+1]
                                   << ". Ignore if this is time 0 in a restart case.";
                                Printer::ext_warn(ss.str(), "Optimization", "NPV_ET_V1");
                                continue;
                            }
                            if (std::fmod(report_times.at(i), 365) == 0) {
                                discount_factor = 1 / (pow(1 + curr_comp->discount, report_times.at(i) / 365));
                                discount_factor_temp.push_back(discount_factor);
                                NPV_times_temp.push_back(i);
                            }
                        }
                    }

                    else if (curr_comp->interval == "Monthly") {
                        double discount_factor;
                        double discount_rate = curr_comp->discount;
                        double monthly_discount_rate = components_->at(k)->yearlyToMonthly(discount_rate);
                        for (int i = 0; i < report_times.size(); i++) {
                            if (std::fmod(report_times.at(i), 30) == 0) {
                                discount_factor = 1 / (pow(1 + monthly_discount_rate, report_times.at(i) / 30));
                                discount_factor_temp.push_back(discount_factor);
                                NPV_times_temp.push_back(i);
                            }
                        }
                    }

                    NPV_times.push_back(NPV_times_temp);
                    discount_factor_list.push_back(discount_factor_temp);
                }

                for (int i = 0; i < components_->size(); ++i) {
                    auto curr_comp = components_->at(i);

                    if (curr_comp->is_json_component) { continue; }

                    if (curr_comp->usediscountfactor) {
                        for (int j = 1; j < NPV_times[i].size(); ++j) {
                            auto va = curr_comp->resolveValueDiscount(results_, NPV_times[i][j]);
                            auto vb = curr_comp->resolveValueDiscount(results_, NPV_times[i][j - 1]);
                            auto prod_difference = va - vb;
                            NPV += prod_difference * curr_comp->coefficient * discount_factor_list[i][j];
                        }
                    }

                    else {
                        NPV += curr_comp->resolveValue(results_) * curr_comp->coefficient;
                    }
                }

                if (well_economy_->use_well_cost) {
                    for (auto well: well_economy_->wells_pointer) {
                        if (well_economy_->separate) {
                            NPV -= well_economy_->costXY * well_economy_->well_xy[well->name().toStdString()];
                            NPV -= well_economy_->costZ * well_economy_->well_z[well->name().toStdString()];
                        }

                        else {
                            NPV -= well_economy_->cost * well_economy_->well_lengths[well->name().toStdString()];
                        }
                    }
                }

                for (int j = 0; j < components_->size(); ++j) {
                    if (components_->at(j)->is_json_component) {
                        if (components_->at(j)->interval == "Single" || components_->at(j)->interval == "None") {
                            double extval = components_->at(j)->coefficient
                                            * results_->GetJsonResults().GetSingleValue(components_->at(j)->property_name);
                            NPV += extval;
                        }

                        else {
                            Printer::ext_warn("Unable to parse external component.", "Optimization", "NPV_ET_V1");
                        }
                    }
                }

                std::cout << "NPV: .............................." << NPV << endl;
                return NPV;
            }

            catch (std::exception const &ex) {
                Printer::error("Failed to compute NPV: " + string(ex.what()) + ". Returning 0.0");
                return 0.0;
            }
        }

        double NPV_ET_V1::value() const {
            try {
                double ET = calc_ET();

                double NPV = calc_NPV();

                double OFV = NPV - ET;

                std::cout << "OFV: .............................." << OFV << endl;

                return OFV;
            }
            catch (...) {
                Printer::error("Failed to compute NPV_ET_V1. Returning 0.0");

                return 0.0;
            }
        }

        double NPV_ET_V1::Component::resolveValue(Simulation::Results::Results *results) {
            return results->GetValue(property);
        }
        double NPV_ET_V1::Component::resolveValueDiscount(Simulation::Results::Results *results, double time_step) {
            int time_step_int = (int) time_step;
            return results->GetValue(property, time_step_int);
        }
        double NPV_ET_V1::Component::yearlyToMonthly(double discount_rate) {
            return pow((1 + discount_rate), (1.0 / 12)) - 1;
        }

    }

}


