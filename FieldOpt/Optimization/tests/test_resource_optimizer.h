/******************************************************************************
   Created by einar on 2/6/16.
   Copyright (C) 2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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


#ifndef FIELDOPT_TEST_RESOURCE_OPTIMIZER_H
#define FIELDOPT_TEST_RESOURCE_OPTIMIZER_H

#include "Model/tests/test_resource_model.h"
#include "Optimization/case.h"
#include "test_resource_cases.h"

namespace TestResources {
class TestResourceOptimizer : public TestResourceModel, public TestResourceCases {
 protected:
  TestResourceOptimizer() {
      base_case_ = new Optimization::Case(model_->variables()->GetBinaryVariableValues(),
                                          model_->variables()->GetDiscreteVariableValues(),
                                          model_->variables()->GetContinousVariableValues());
      base_case_->set_objective_function_value(1000.0);

      settings_compass_search_min_unconstr_ = new Settings::Optimizer(get_json_settings_compass_search_minimize_);
      settings_compass_search_max_unconstr_ = new Settings::Optimizer(get_json_settings_compass_search_maximize_);
      settings_apps_min_unconstr_ = new Settings::Optimizer(get_json_settings_apps_minimize_);
      settings_apps_max_unconstr_ = new Settings::Optimizer(get_json_settings_apps_maximize_);
      settings_ga_min_ = new Settings::Optimizer(get_json_settings_ga_minimize_);
      settings_ga_max_ = new Settings::Optimizer(get_json_settings_ga_maximize_);
      settings_ego_max_ = new Settings::Optimizer(get_json_settings_ego_maximize_);
  }

  Optimization::Case *base_case_;
  Settings::Optimizer *settings_compass_search_min_unconstr_;
  Settings::Optimizer *settings_compass_search_max_unconstr_;
  Settings::Optimizer *settings_apps_min_unconstr_;
  Settings::Optimizer *settings_apps_max_unconstr_;
  Settings::Optimizer *settings_ga_min_;
  Settings::Optimizer *settings_ga_max_;
  Settings::Optimizer *settings_ego_max_;

 private:
  QJsonObject obj_fun_ {
      {"Type", "WeightedSum"},
      {"WeightedSumComponents", QJsonArray{
          QJsonObject{
              {"Coefficient", 1.0}, {"Property", "CumulativeOilProduction"}, {"TimeStep", -1}, {"IsWellProp", false}
          },
          QJsonObject{
              {"Coefficient", 0.0}, {"Property", "CumulativeWaterProduction"}, {"TimeStep", -1}, {"IsWellProp", false}
          }
      }}
  };
  QJsonObject get_json_settings_compass_search_minimize_ {
      {"Type", "Compass"},
      {"Mode", "Minimize"},
      {"Parameters", QJsonObject{
          {"MaxEvaluations", 100},
          {"InitialStepLength", 0.25},
          {"MinimumStepLength", 0.01}
      }},
      {"Objective", obj_fun_}
  };

  QJsonObject get_json_settings_compass_search_maximize_ {
      {"Type", "Compass"},
      {"Mode", "Maximize"},
      {"Parameters", QJsonObject{
          {"MaxEvaluations", 100},
          {"InitialStepLength", 8},
          {"MinimumStepLength", 1}
      }},
      {"Objective", obj_fun_}
  };

  QJsonObject get_json_settings_apps_minimize_ {
      {"Type", "APPS"},
      {"Mode", "Minimize"},
      {"Parameters", QJsonObject{
          {"MaxEvaluations", 500},
          {"InitialStepLength", 0.64},
          {"MinimumStepLength", 0.005}
      }},
      {"Objective", obj_fun_}
  };

  QJsonObject get_json_settings_apps_maximize_ {
      {"Type", "APPS"},
      {"Mode", "Maximize"},
      {"Parameters", QJsonObject{
          {"MaxEvaluations", 500},
          {"InitialStepLength", 8},
          {"MinimumStepLength", 1}
      }},
      {"Objective", obj_fun_}
  };

  QJsonObject get_json_settings_ga_maximize_ {
      {"Type", "APPS"},
      {"Mode", "Maximize"},
      {"Parameters", QJsonObject{
          {"MaxGenerations", 100},
          {"PopulationSize", 20},
          {"CrossoverProbability", 0.8},
          {"MutationProbability", 0.05},
          {"CrossoverScaleInt", 0.15},
          {"CrossoverScaleReal", 0.25},
          {"MutationPowerInt", 5.0},
          {"MutationPowerReal", 2.0},
          {"LowerBoundInt", 0.0},
          {"UpperBoundInt", 10.0},
          {"LowerBoundReal", 0.0},
          {"UpperBoundReal", 100.0}

      }},
      {"Objective", obj_fun_}
  };

  QJsonObject get_json_settings_ga_minimize_ {
      {"Type", "APPS"},
      {"Mode", "Minimize"},
      {"Parameters", QJsonObject{
          {"MaxGenerations",        700},
          {"PopulationSize",        60},
          {"CrossoverProbability",  0.1},
          {"DecayRate",             4.0},
          {"MutationStrength",      0.25},
          {"StagnationLimit",       1e-10},
          {"LowerBound",            -10.0},
          {"UpperBound",            10.0}

      }},
      {"Objective", obj_fun_}
  };

  QJsonObject get_json_settings_ego_maximize_ {
      {"Type", "EGO"},
      {"Mode", "Maximize"},
      {"Parameters", QJsonObject{
          {"LowerBound", -5},
          {"UpperBound",  5},
          {"MaxEvaluations", 1000}
      }},
      {"Objective", obj_fun_},
//      {"Constraints", QJsonArray{
//          QJsonObject{
//              {"Type", "ReservoirBoundary"},
//              {"Wells", QJsonArray{"INJECTOR", "PRODUCER"}},
//              {"PenaltyWeight", 0.01},
//              {"BoxImin", 8},
//              {"BoxImax", 26},
//              {"BoxJmin", 11},
//              {"BoxJmax", 50},
//              {"BoxKmin", 6},
//              {"BoxKmax", 7}
//          }
//      }}
  };

};
}
#endif //FIELDOPT_TEST_RESOURCE_OPTIMIZER_H
