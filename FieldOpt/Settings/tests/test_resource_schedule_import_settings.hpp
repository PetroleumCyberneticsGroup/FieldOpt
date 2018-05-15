//
// Created by einar on 4/19/18.
//

#ifndef FIELDOPT_TEST_RESOURCE_SCHEDULE_IMPORT_SETTINGS_H
#define FIELDOPT_TEST_RESOURCE_SCHEDULE_IMPORT_SETTINGS_H

namespace TestResources {
namespace TestResourceSettings {

QJsonObject imported_model_settings {
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
            {"SplineConversion", QJsonObject {
                {"Wells", QJsonArray {"C-1H"}},
                {"SplinePoints", 4}
            }},
            {"InjectorDefaultRate", 2000},
            {"ProducerDefaultBHP", 150}
        }},
        {"Wells", QJsonArray {
            QJsonObject {
                {"Name", "C-1H"},
                {"Controls", QJsonObject {
                    {"IsVariable", true},
                    {"VariableTimeSteps", QJsonArray {
                        0, 300, 600
                    }}
                }}
            }
        }}

    }}
};

}
}

#endif //FIELDOPT_TEST_RESOURCE_SCHEDULE_IMPORT_SETTINGS_H
