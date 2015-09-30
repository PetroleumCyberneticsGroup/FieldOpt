/******************************************************************************
 *
 * settings.h
 *
 * Created: 28.09.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QJsonObject>

#include "simulator.h"
#include "optimizer.h"
#include "model.h"

namespace Utilities {
namespace Settings {

class Simulator;
class Model;
class Optimizer;

class Settings
{
public:
    Settings(QString driver_path); //!< Used when reading a file

    QString driver_path() const { return driver_path_; }

    QString name() const { return name_; }
    QString output_directory() const { return output_directory_; }
    bool verbose() const { return verbose_; }

    Model *model() const { return model_; }
    Utilities::Settings::Optimizer *optimizer() const { return optimizer_; }
    Simulator *simulator() const { return simulator_; }

private:
    Settings(); //!< Used by friends when writing a file

    QString driver_path_;

    QString name_;
    QString output_directory_;
    bool verbose_;

    Model *model_;
    Utilities::Settings::Optimizer *optimizer_;
    Simulator *simulator_;

    QJsonObject *json_driver_;

    void readDriverFile();
    void readGlobalSection();
    void readSimulatorSection();
    void readOptimizerSection();
    void readModelSection();
};

}
}

#endif // SETTINGS_H
