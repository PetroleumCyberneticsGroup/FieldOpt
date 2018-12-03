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
#include "Settings/paths.h"

namespace Settings {

class Simulator;
class Model;
class Optimizer;

/*!
 * \brief The Settings class contains both general settings for a FieldOpt run
 * and pointers to objects containing specific settings for the Model, Simulator
 * and Optimizer. Settings takes as input the path to a "driver file" in the
 * JSON format.
 *
 * \todo Remove bool verbose_, and associated functions, since this functionality
 * is now handled by int verbosity_level_ in runtime_settings
 */
class Settings
{
 public:
  Settings(){}
  Settings(Paths &paths);

  QString name() const { return name_; } //!< The name to be used for the run. Output file and folder names are derived from this.

  // To be removed:
  bool verbose() const { return verbose_; } //!< Verbose mode (with or without debug printing).
  void set_verbosity(const bool verbosity) { verbose_ = verbosity; }

  //!< Get the value for the bookkeeper tolerance. Used by the Bookkeeper in the Runner library.
  double bookkeeper_tolerance() const { return bookkeeper_tolerance_; }

  Model *model() const { return model_; } //!< Object containing model specific settings.
  Optimizer *optimizer() const { return optimizer_; } //!< Object containing optimizer specific settings.
  Simulator *simulator() const { return simulator_; } //!< Object containing simulator specific settings.

  QString GetLogCsvString() const; //!< Get a string containing the CSV header and contents for the log.

  Paths &paths() { return paths_; }

 private:
  Paths paths_;
  QJsonObject *json_driver_;
  QString name_;
  double bookkeeper_tolerance_;
  bool verbose_ = false;
  Model *model_;
  Optimizer *optimizer_;
  Simulator *simulator_;

  void readDriverFile();
  void readGlobalSection();
  void readSimulatorSection();
  void readModelSection();
  void readOptimizerSection();
};

}

#endif // SETTINGS_H
