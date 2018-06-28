/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it
 and/or modify it under the terms of the GNU General
 Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at
 your option) any later version.

 FieldOpt is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#include "settings.h"
#include "settings_exceptions.h"
#include "Utilities/filehandling.hpp"

// ---------------------------------------------------------
// QT
#include <QJsonDocument>

// ---------------------------------------------------------
// STD
#include <iostream>
#include <iomanip>

// ---------------------------------------------------------
using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::fixed;
using std::setprecision;

// ---------------------------------------------------------
namespace Settings {

// =========================================================
Settings::Settings(QString driver_path,
                   QString output_directory,
                   vector<int> verb_vector) {

  // -------------------------------------------------------
  if (!::Utilities::FileHandling::FileExists(driver_path))
    throw FileNotFoundException(driver_path.toStdString());

  // -------------------------------------------------------
  verb_vector_ = verb_vector;
  output_directory_ = output_directory;
  driver_path_ = driver_path;
//  verbstr(verb_vector_); // dbg

  // -------------------------------------------------------
  readDriverFile();

}

// =========================================================
void Settings::readDriverFile() {

  // -------------------------------------------------------
  QFile *file = new QFile(driver_path_);
  if (!file->open(QIODevice::ReadOnly))
    throw DriverFileReadException(
        "Unable to open the driver file");

  // -------------------------------------------------------
  QByteArray data = file->readAll();

  // -------------------------------------------------------
  QJsonDocument json = QJsonDocument::fromJson(data);
  if (json.isNull())
    throw DriverFileJsonParsingException(
        "Unable to parse the input file to JSON.");

  // -------------------------------------------------------
  if (!json.isObject())
    throw DriverFileFormatException(
        "Driver file format incorrect. Must be a JSON object.");

  // -------------------------------------------------------
  json_driver_ = new QJsonObject(json.object());

//  verbstr(verb_vector_); // dbg
  readGlobalSection();
  readSimulatorSection();
  readModelSection();

  readOptimizationSection();
  readOptimizerSection();

  file->close();
}

// =========================================================
void Settings::readGlobalSection() {

  try {

    // -----------------------------------------------------
    QJsonObject global =
        json_driver_->value("Global").toObject();

    name_ = global["Name"].toString();

    bookkeeper_tolerance_ =
        global["BookkeeperTolerance"].toDouble();

    // -----------------------------------------------------
    if (bookkeeper_tolerance_ < 0.0) {
      throw UnableToParseGlobalSectionException(
          "The bookkeeper tolerance must be a positive number.");
    }

    // -----------------------------------------------------
  } catch (std::exception const &ex) {
    throw UnableToParseGlobalSectionException(
        "Unable to parse driver file global section: "
            + string(ex.what()));
  }

  // -----------------------------------------------------
//  verbstr(verb_vector_); // dbg
  if (verb_vector_[9] > 1) { // idx:9 -> set
    string str_out = "[set]Global settings---";

    cout << "\n" << BLDON << str_out << AEND << "\n"
         << std::string(str_out.length(), '=') << endl;

    cout << "Name:------------------ "
         << name_.toStdString() << endl;

    cout << "BookkeeperTolerance:--- "
         << bookkeeper_tolerance() << endl;
  }
//  verbstr(verb_vector_); // dbg
}

// =========================================================
void Settings::readSimulatorSection() {

  // -------------------------------------------------------
  QJsonObject json_simulator;

  try {

    // -----------------------------------------------------
    json_simulator =
        json_driver_->value("Simulator").toObject();
    simulator_ = new Simulator(json_simulator);

    // -----------------------------------------------------
  } catch (std::exception const &ex) {
    throw UnableToParseSimulatorSectionException(
        "Unable to parse driver file "
            "simulator section: " + string(ex.what()));
  }

  // -------------------------------------------------------
  simulator_->verb_vector_ = verb_vector_;
  simulator_->output_directory_ = output_directory_;

  if (simulator_->verb_vector_[9] > 1) { // idx:9 -> set

    // -----------------------------------------------------
    string str_out = "[set]Simulator settings";
    cout << "\n" << BLDON << str_out << AEND << "\n"
         << std::string(str_out.length(), '=') << endl;

    // verbstr(&simulator_->verb_vector_); // dbg

    cout << "SimulatorType:--------- "
         << json_simulator["Type"].
             toString().toUtf8().constData() << endl;

    cout << "FluidModel:------------ "
         << json_simulator["FluidModel"].
             toString().toUtf8().constData() << endl;

    cout << "MaxMinutes:------------ "
         << simulator_->max_minutes_ << endl;
  }
}

// ========================================================
void Settings::readModelSection() {

  // -------------------------------------------------------------
  try {
    QJsonObject model = json_driver_->value("Model").toObject();

    model_ = new Model(model,
                       simulator_->driver_file_path_,
                       verb_vector_);

  }
  catch (std::exception const &ex) {
    throw UnableToParseModelSectionException(
        "Unable to parse model section: " + string(ex.what()));
  }

  // -------------------------------------------------------
  model_->verb_vector_ = verb_vector_;
  model_->output_directory_ = output_directory_;

  // -------------------------------------------------------------
  if (model_->verb_vector_[9] > 0) { // idx:9 -> set (Settings)
    string str_out = "[set]model settings----";
    cout << "\n" << BLDON << str_out << AEND << "\n"
         << string(str_out.length(), '=') << endl;

    // ---------------------------------------------------------
    // Reservoir type
    cout << fixed << setprecision(1);
    cout << "Reservoir type:-------- "
         << model_->ResTypeStr(
             model_->reservoir_.type).toStdString() << endl;

    // ---------------------------------------------------------
    // Drilling type
    cout << "Drilling mode:--------- "
         << model_->DrillingStr(
             model_->drillingMode_).toStdString() << endl;

    // ---------------------------------------------------------
    // Drilling sequence
    // cout << "Drilling sequence:----- " << endl;
    // model_->GetDrillingStr(model_->drillseq_);

  }
}

// =========================================================
void Settings::readOptimizerSection() {

  // -------------------------------------------------------
  try {
    QJsonObject optimizer =
        json_driver_->value("Optimizer").toObject();
    optimizer_ = new Optimizer(optimizer);
  }
  catch (std::exception const &ex) {
    throw UnableToParseOptimizerSectionException(
        "Unable to parse driver file optimizer section: "
            + string(ex.what()));
  }

  // -------------------------------------------------------
  verbstr(verb_vector_); // dbg
  optimizer_->verb_vector_ = verb_vector_;
  optimizer_->output_directory_ = output_directory_;
  cout << "constraints" << endl;
  cout << fstr(output_directory_.toStdString()) << endl;

  for (auto constraint : optimizer_->constraints()) {

    constraint.verb_vector_ = verb_vector_;
    constraint.output_directory_ = output_directory_;
    verbstr(constraint.verb_vector_); // dbg
    cout << fstr(constraint.output_directory_.toStdString()) << endl;

  }

  // -------------------------------------------------------
  if (optimizer_->verb_vector_[9] > 1) { // idx:9 -> set

    // -----------------------------------------------------
    string str_out = "[set]Optimizer settings";
    cout << "\n" << BLDON << str_out << AEND << "\n"
         << string(str_out.length(), '=') << endl;

    // -----------------------------------------------------
//    verbstr(optimizer_->verb_vector_); // dbg

    // -----------------------------------------------------
    if (optimizer_->type() == Optimizer::OptimizerType::Compass ||
        optimizer_->type() == Optimizer::OptimizerType::APPS) {

      // ---------------------------------------------------
      cout << fixed << setprecision(1);
      cout << "MaxEvaluations:-------- "
           << optimizer_->parameters_.max_evaluations << endl;
      cout << "InitialStepLength:----- "
           << optimizer_->parameters_.initial_step_length << endl;

      // ---------------------------------------------------
      cout << "InitialStepLengthXYZ:-- ";
      for(int i=0; i<optimizer_->
          parameters_.initial_step_length_xyz.count(); ++i ) {
        cout << optimizer_->parameters_.initial_step_length_xyz[i] << " ";
      }
      cout << endl;

      // ---------------------------------------------------
      cout << "MinimumStepLength:----- "
           << optimizer_->parameters_.minimum_step_length << endl;
      cout << "MinimumStepLengthXYZ:-- ";
      for(int i=0; i<optimizer_->
          parameters_.minimum_step_length_xyz.count(); ++i ) {
        cout << optimizer_->parameters_.minimum_step_length_xyz[i] << " ";
      }
      cout << endl;

      // ---------------------------------------------------
      cout << fixed << setprecision(8);
      cout << "ContractionFactor:----- "
           << optimizer_->parameters_.contraction_factor << endl;
      cout << "ExpansionFactor:------- "
           << optimizer_->parameters_.expansion_factor << endl;

      // ---------------------------------------------------
      cout << fixed << setprecision(1);
      cout << "MaxQueueSize:---------- "
           << optimizer_->parameters_.max_queue_size << endl;
      cout << "Pattern:--------------- "
           << optimizer_->parameters_.pattern.toStdString() << endl;

    } else if (optimizer_->type() == Optimizer::OptimizerType::SNOPTSolver) {

      // ---------------------------------------------------------
      cout << fixed << setprecision(1);
      cout << "Option file:----------- "
           << optimizer_->parameters_.thrdps_optn_file.toStdString() << endl;
      cout << "Summary file:---------- "
           << optimizer_->parameters_.thrdps_smry_file.toStdString() << endl;
      cout << "Print file:------------ "
           << optimizer_->parameters_.thrdps_prnt_file.toStdString() << endl;

    } else if (optimizer_->type() ==
        Optimizer::OptimizerType::GeneticAlgorithm) {

      cout << fixed << setprecision(1);
      cout << "DecayRate:------------- "
           << optimizer_->parameters_.decay_rate << endl;
      // ---------------------------------------------------------
      cout << "MutationStrength:------ "
           << optimizer_->parameters_.mutation_strength << endl;
      // ---------------------------------------------------------
      cout << "MaxGenerations:-------- "
           << optimizer_->parameters_.max_generations << endl;
      // ---------------------------------------------------------
      cout << "PopulationSize:-------- "
           << optimizer_->parameters_.population_size << endl;
      // ---------------------------------------------------------
      cout << "PopulationCrossover:--- "
           << optimizer_->parameters_.p_crossover << endl;

    } else if (optimizer_->type() ==
        Optimizer::OptimizerType::DFO){

      // ---------------------------------------------------------
      cout << "InitialTrustRegionRadius"
           << optimizer_->parameters_.initial_trust_region_radius << endl;
    }

  }
}


// =========================================================
void Settings::readOptimizationSection() {

  // -------------------------------------------------------
  try {
    QJsonObject optimization =
        json_driver_->value("Optimization").toObject();
    optimization_ = new Optimization(optimization);
  }
  catch (std::exception const &ex) {
    throw UnableToParseOptimizerSectionException(
        "Unable to parse driver file Optimization section: "
            + string(ex.what()));
  }

  // -------------------------------------------------------
  // Fields:
  // -Optimizers
  // -Optimization structure:
  // -Variable assignment, i.e., which optimizers that
  // should deal with which variable types (no overlap
  // possible); types refers to either intrinsic or
  // extrinsic variable types, e.g., distinction by
  // continuous, discrete and/or categorical; and
  // well placement, well controls, completions, etc.

  // -------------------------------------------------------
  optimization_->verb_vector_ = verb_vector_;
  optimization_->output_directory_ = output_directory_;

  if (optimization_->verb_vector_[9] > 1) { // idx:9 -> set

  }

}

// ===============================================================
void Settings::set_build_path(const QString &build_path) {

  // -------------------------------------------------------------
  if (!Utilities::FileHandling::DirectoryExists(build_path))
    throw std::runtime_error(
        "Attempted to set the build path to a non-existent directory.");
  build_path_ = build_path;
}

// =========================================================
QString Settings::GetLogCsvString() const {

  QStringList header  = QStringList();
  QStringList content = QStringList();

  // -------------------------------------------------------
  header  << "name"
          << "maxevals"
          << "initstep"
          << "minstep";

  // -------------------------------------------------------
  content << name_
          << QString::number(optimizer_->parameters().max_evaluations)
          << QString::number(optimizer_->parameters().initial_step_length)
          << QString::number(optimizer_->parameters().minimum_step_length);

  return QString("%1\n%2").
      arg(header.join(",")).arg(content.join(","));
}

}


