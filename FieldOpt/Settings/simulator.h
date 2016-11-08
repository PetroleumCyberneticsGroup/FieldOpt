#ifndef SETTINGS_SIMULATOR_H
#define SETTINGS_SIMULATOR_H

#include "settings.h"

#include <QStringList>

namespace Settings {

/*!
 * \brief The Simulator class contains simulator-specific settings. Simulator settings objects
 * may _only_ be created by the Settings class. They are created when reading a JSON-formatted
 * "driver file".
 */
    class Simulator
    {
        friend class Settings;

    public:
        enum SimulatorType { ECLIPSE, ADGPRS, Flow };
        enum SimulatorFluidModel { BlackOil, DeadOil };

        SimulatorType type() const { return type_; } //!< Get the simulator type (e.g. ECLIPSE).
        QStringList *commands() const { return commands_; } //!< Get the simulator commands (commands used to execute a simulation). Each list element is executed in sequence.
        QString script_name() const { return script_name_; } //!< Get the name of the script to be used to execute simulations.
        QString driver_file_path() const { return driver_file_path_; } //!< Get the path to the driver file.
        void set_driver_file_path(const QString path) { driver_file_path_ = path; } //!< Set the driver file path. Used when the path is passed by command line argument.
        void set_execution_script_path (const QString path) { custom_exec_script_path_ = path; } //!< Set a custom path for the simulator execution script.
        QString custom_simulator_execution_script_path() const { return custom_exec_script_path_; } //!< Get the path to the simulator execution script.
        QString output_directory() const { return output_directory_; } //!< Get the output directory path.
        SimulatorFluidModel fluid_model() const { return fluid_model_; } //!< Get the fluid model

    private:
        Simulator(QJsonObject json_simulator);
        SimulatorType type_;
        SimulatorFluidModel fluid_model_;
        QStringList *commands_;
        QString script_name_;
        QString driver_file_path_;
        QString output_directory_;
        QString custom_exec_script_path_;
    };

}


#endif // SETTINGS_SIMULATOR_H
