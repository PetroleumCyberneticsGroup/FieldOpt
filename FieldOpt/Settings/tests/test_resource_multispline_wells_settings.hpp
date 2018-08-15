//
// Created by einar on 5/14/18.
//

#ifndef FIELDOPT_TEST_RESOURCE_MULTISPLINE_SETTINGS_H
#define FIELDOPT_TEST_RESOURCE_MULTISPLINE_SETTINGS_H

#include <QJsonObject>
#include <QJsonArray>

QJsonObject model_adtl_pts {
    {"Model", QJsonObject {
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

    }}
};

#endif //FIELDOPT_TEST_RESOURCE_MULTISPLINE_SETTINGS_H
