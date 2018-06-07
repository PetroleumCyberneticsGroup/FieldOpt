//
// Created by einar on 6/07/18.
//

#ifndef FIELDOPT_TEST_RESOURCE_SCHEDULE_MULTISEGMENT_SETTINGS_H
#define FIELDOPT_TEST_RESOURCE_SCHEDULE_MULTISEGMENT_SETTINGS_H

namespace TestResources {
namespace TestResourceSettings {

QJsonObject multisegment_model_settings {
    {"Simulator", QJsonObject {
        {"Type", "ECLIPSE"},
        {"FluidModel", "BlackOil"},
        {"ExecutionScript", "csh_eclrun.sh"},
        {"DriverPath", "../examples/ECLIPSE/norne-simplified/NORNE_SIMPLIFIED.DATA"},
        {"SchedulePath", "INCLUDE/BC0407_HIST01122006.SCH"}
    }},
    {"Model", QJsonObject {
        {"ControlTimes", QJsonArray {
            0, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300
        }},
        {"Reservoir", QJsonObject {
            {"Type", "ECLIPSE"}
        }},
        {"Import", QJsonObject {
            {"Keywords", QJsonArray {
                "AllWells"
            }},
            {"ImportTrajectories", QJsonArray { "D-2H" }},
            {"InjectorDefaultRate", 2000},
            {"ProducerDefaultBHP", 150}
        }},
        {"Wells", QJsonArray {
            QJsonObject {
                {"Name", "D-2H"},
                {"Segmentation", QJsonObject {
                    {"Tubing", QJsonObject {
                        {"Diameter", 0.10},
                        {"Roughness", 1.52E-5}
                    }},
                    {"Annulus", QJsonObject {
                        {"Diameter", 0.04},
                        {"Roughness", 1.52E-5},
                        {"CrossSectionArea", 8.17E-3}
                    }},
                    {"Compartments", QJsonObject {
                        {"Count", 3},
                        {"VariablePackers", false},
                        {"VariableICDs", true},
                        {"ICDType", "Valve"},
                        {"ICDValveSize", 7.85E-5},
                    }}
                }}
            }
        }}

    }}
};

}
}

#endif //FIELDOPT_TEST_RESOURCE_SCHEDULE_MULTISEGMENT_SETTINGS_H
