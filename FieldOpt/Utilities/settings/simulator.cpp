#include "simulator.h"
#include "settings_exceptions.h"

namespace Utilities {
    namespace Settings {

        Simulator::Simulator(QJsonObject json_simulator)
        {
            // Driver path
            if (json_simulator.contains("DriverPath"))
                driver_file_path_ = json_simulator["DriverPath"].toString();
            else driver_file_path_ = "";

            // Simulator type
            QString type = json_simulator["Type"].toString();
            if (QString::compare(type, "ECLIPSE") == 0)
                type_ = SimulatorType::ECLIPSE;
            else if (QString::compare(type, "ADGPRS") == 0)
                type_ = SimulatorType::ADGPRS;
            else throw SimulatorTypeNotRecognizedException("The simulator type " + type.toStdString() + " was not recognized");

            // Simulator commands
            QJsonArray commands = json_simulator["Commands"].toArray();
            script_name_ = "";
            if (json_simulator.contains("ExecutionScript") && json_simulator["ExecutionScript"].toString().size() > 0) {
                script_name_ = json_simulator["ExecutionScript"].toString();
            }
            if (json_simulator.contains("Commands") && commands.size() > 0) {
                commands_ = new QStringList();
                for (int i = 0; i < commands.size(); ++i) {
                    commands_->append(commands[i].toString());
                }
            }
            if (script_name_.length() == 0 && commands.size() == 0)
                throw NoSimulatorCommandsGivenException("At least one simulator command or a simulator script must be given.");

            if (json_simulator.contains("FluidModel")) {
                QString fluid_model = json_simulator["FluidModel"].toString();
                if (QString::compare(fluid_model, "DeadOil") == 0)
                    fluid_model_ = SimulatorFluidModel::DeadOil;
                else if (QString::compare(fluid_model, "BlackOil") == 0)
                    fluid_model_ = SimulatorFluidModel::BlackOil;
            }
            else fluid_model_ = SimulatorFluidModel::BlackOil;
        }

    }
}
