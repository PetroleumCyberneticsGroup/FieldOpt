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


#ifndef FIELDOPT_TEST_RESOURCE_MODEL_SETTING_SNIPPETS_H
#define FIELDOPT_TEST_RESOURCE_MODEL_SETTING_SNIPPETS_H

#include <QJsonObject>
#include <QJsonArray>

namespace TestResources {
namespace TestResourceModelSettingSnippets {

inline QJsonObject model_wells_pseudoVertical2dProducer_01() {
    return QJsonObject {
        {"Name", "PROD1"},
        {"Group", "G1"},
        {"Type", "Producer"},
        {"DefinitionType", "PseudoContVertical2D"},
        {"PreferedPhase", "Oil"},
        {"WellboreRadius", 0.1905},
        {
            "Position", QJsonObject {
            {"i", 5}, {"j", 5},
            {"IsVariable", true}
        }},
        {
            "Controls", QJsonArray {
            QJsonObject {
                {"TimeStep", 0},
                {"State", "Open"},
                {"Mode", "BHP"},
                {"BHP", 90.0},
                {"IsVariable", true}
            }
        }}
    };
}

inline QJsonObject model_wells_pseudoVertical2dProducer_02() {
    return QJsonObject {
        {"Name", "PROD1"},
        {"Group", "G1"},
        {"Type", "Producer"},
        {"DefinitionType", "PseudoContVertical2D"},
        {"PreferedPhase", "Oil"},
        {"WellboreRadius", 0.1905},
        {"Position", QJsonObject {
            {"i", 35}, {"j", 35},
            {"IsVariable", true}
        }},
        {"Controls", QJsonArray {
            QJsonObject {
                {"TimeStep", 0},
                {"State", "Open"},
                {"Mode", "BHP"},
                {"BHP", 90.0},
                {"IsVariable", true}
            }
        }}
    };
}

inline QJsonObject model_pseudocont_2_prods_var_pos_and_bhp() {
    return QJsonObject {
        {"ControlTimes", QJsonArray {0, 50, 100, 200}},
        {"Reservoir", QJsonObject {{"Type", "ECLIPSE"}}},
        {"Wells", QJsonArray {
            model_wells_pseudoVertical2dProducer_01(),
            model_wells_pseudoVertical2dProducer_02()
        }}
    };
}

inline QJsonObject model_adtl_pts() {
    return QJsonObject {
        {"ControlTimes", QJsonArray {
            0, 50
        }},
        {"Reservoir", QJsonObject {
            {"Type", "ECLIPSE"}
        }},
        {"Wells", QJsonArray {
            QJsonObject {
                {"Name", "PRODUCER"},
                {"Group", "G1"},
                {"Use2DModel", true},
                {"Type", "Producer"},
                {"DefinitionType", "WellSpline"},
                {"PreferredPhase", "Oil"},
                {"WellboreRadius", 0.1905},
                {"Controls", QJsonArray { QJsonObject{
                    {"TimeStep", 0},
                    {"State", "Open"},
                    {"Mode", "BHP"},
                    {"BHP", 90.0} }
                }},
                {"SplinePoints", QJsonObject {
                    {"Heel", QJsonObject {
                        {"x", 300}, {"y", 900}, {"z", 1712}, {"IsVariable", true}
                    }},
                    {"Toe", QJsonObject {
                        {"x", 900}, {"y", 900}, {"z", 1712}, {"IsVariable", true}
                    }},
                    {"AdditionalPoints", 2}
                }}
            }
        }}

    };
}

}
}
#endif //FIELDOPT_TEST_RESOURCE_MODEL_SETTING_SNIPPETS_H
