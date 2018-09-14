//
// Created by bragesk on 11.09.18.
//

#include "NPV.h"

#include <iostream>
#include <iomanip>
#include "weightedsum.h"
#include <stdlib.h>
#include <cmath>

using std::cout;
using std::endl;
using std::fixed;
using std::setprecision;
using std::scientific;

namespace Optimization {
    namespace Objective {

        NPV::NPV(Settings::Optimizer *settings,
                 Simulation::Results::Results *results) {
            settings_ = settings;
            results_ = results;
            components_ = new QList<NPV::Component *>();
            auto report_time = results->GetValueVector(results->Time);
            for (int i = 0; i < settings->objective().NPV_sum.size(); ++i) {
                auto *comp = new NPV::Component();
                comp->property_name = settings->objective().NPV_sum.at(i).property;
                comp->property = results_->GetPropertyKeyFromString(comp->property_name);
                comp->coefficient = settings->objective().NPV_sum.at(i).coefficient;
                if(settings->objective().NPV_sum.at(i).usediscountfactor == true){
                    comp->interval = settings->objective().NPV_sum.at(i).interval;
                    comp->discount = settings->objective().NPV_sum.at(i).discount;
                    cout << comp->discount << endl;
                    comp->usediscountfactor = settings->objective().NPV_sum.at(i).usediscountfactor;}
                else {
                    comp->interval = "None";
                    comp->discount = 0;
                    comp->usediscountfactor = false;
                }
                components_->append(comp);
            }


        }

        double NPV::value() const
        {
            double value = 0;

            auto report_times = results_->GetValueVector(results_->Time);
            auto NPV_times = new QList<double>;
            auto discount_factor_list = new QList<double>;
            for (int k = 0; k < components_->size(); ++k){
                if(components_->at(k)->interval=="Yearly"){
                    double discount_factor;
                    int j = 1;
                    for(int i=0; i < report_times.size(); i++){
                        if(std::fmod(report_times.at(i), 365) == 0) {
                            discount_factor = 1/(1*(pow(1+components_->at(k)->discount, j-1)));
                            discount_factor_list->append(discount_factor);
                            NPV_times->append(i);

                            j += 1;
                        }
                    }
                }else if(components_->at(k)->interval=="Monthly") {
                    double discount_factor;
                    double discount_rate = components_->at(k)->discount;
                    double monthly_discount = components_->at(k)->yearlyToMonthly(discount_rate);
                    int j = 1;
                    for (int i = 0; i < report_times.size(); i++) {
                        if (std::fmod(report_times.at(i), 30) == 0) {
                            NPV_times->append(i);
                            discount_factor = 1/(1*(pow(1+monthly_discount,j-1)));
                            discount_factor_list->append(discount_factor);
                            j += 1;
                        }
                    }
                }
        }
            for (int i = 0; i < components_->size(); ++i){
                if (components_->at(i)->usediscountfactor == true){
                    for(int j = 1; j < NPV_times->size(); ++j){
                        auto prod_difference = components_->at(i)->resolveValueDiscount(results_,NPV_times->at(j)) - components_->at(i)->resolveValueDiscount(results_,NPV_times->at(j-1));
                        value += prod_difference * components_->at(i)->coefficient * discount_factor_list->at(i);
                    }
                }else if(components_->at(i)->usediscountfactor == false){
                    value += components_->at(i)->resolveValue(results_);
                    QString prop_name = components_->at(i)->property_name;
                    double prop_coeff = components_->at(i)->coefficient;
                    cout << scientific << setprecision(8);
                    cout << "ObjFunctionProp[i=" << i << "]: -> (Coeff.) "
                         << prop_coeff << " * (" << prop_name.toStdString() << ") "
                         << results_->GetValue(results_->GetPropertyKeyFromString(prop_name))
                         << endl;
                }
            }


            return value;
        }

        double NPV::Component::resolveValue(Simulation::Results::Results *results)
        {
            return coefficient * results->GetValue(property);

        }
        double NPV::Component::resolveValueDiscount(Simulation::Results::Results *results, double time_step){
            int time_step_int = (int) time_step;
            return results->GetValue(property, time_step_int);
        }

        double NPV::Component::yearlyToMonthly(double discount_factor){
            return pow((1+discount_factor),0.083333)-1;

            }

        }

    }
