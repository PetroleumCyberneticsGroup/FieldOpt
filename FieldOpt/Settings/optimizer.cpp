/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#include "optimizer.h"
#include "settings_exceptions.h"

namespace Settings {

Optimizer::Optimizer(QJsonObject json_optimizer)
{
  // Get the root objects.
  QJsonObject json_parameters = json_optimizer["Parameters"].toObject();
  QJsonObject json_objective = json_optimizer["Objective"].toObject();
  QJsonArray json_constraints = json_optimizer["Constraints"].toArray();
  QString type = json_optimizer["Type"].toString();

  // Optimizer type
  if (QString::compare(type, "Compass") == 0)
    type_ = OptimizerType::Compass;
  else if (QString::compare(type, "APPS") == 0)
    type_ = OptimizerType::APPS;
  else if (QString::compare(type, "GeneticAlgorithm") == 0)
    type_ = OptimizerType::GeneticAlgorithm;
  else if (QString::compare(type, "ExhaustiveSearch2DVert") == 0)
    type_ = OptimizerType::ExhaustiveSearch2DVert;
  else if (QString::compare(type, "SNOPTSolver") == 0)
    type_ = OptimizerType::SNOPTSolver;
  else {
    throw OptimizerTypeNotRecognizedException(
        "The optimizer type " + type.toStdString() + " was not recognized.");
  }

  if (type_ == SNOPTSolver) {
    // IO files
    if (json_parameters.contains("OptionFile"))
      parameters_.thrdps_optn_file = json_parameters["OptionFile"].toString();
    if (json_parameters.contains("SummaryFile"))
      parameters_.thrdps_smry_file = json_parameters["SummaryFile"].toString();
    if (json_parameters.contains("PrintFile"))
      parameters_.thrdps_prnt_file = json_parameters["PrintFile"].toString();
  }

  if (type_ != ExhaustiveSearch2DVert) {

    // Optimizer mode
    if (json_optimizer.contains("Mode")) {
      QString mode = json_optimizer["Mode"].toString();
      if (QString::compare(mode, "Minimize", Qt::CaseInsensitive) == 0)
        mode_ = OptimizerMode::Minimize;
      else if (QString::compare(mode, "Maximize", Qt::CaseInsensitive) == 0)
        mode_ = OptimizerMode::Maximize;
      else {
        throw UnableToParseOptimizerSectionException(
            "Did not recognize optimizer Mode setting.");
      }
    } else {
      throw UnableToParseOptimizerSectionException(
          "Optimizer Mode keyword must be specified.");
    }

    // Optimizer parameters
    try {
      // GSS parameters
      if (json_parameters.contains("MaxEvaluations"))
        parameters_.max_evaluations = json_parameters["MaxEvaluations"].toInt();

      if (json_parameters.contains("InitialStepLength"))
        parameters_.initial_step_length = json_parameters["InitialStepLength"].toDouble();

      if (json_parameters.contains("MinimumStepLength"))
        parameters_.minimum_step_length = json_parameters["MinimumStepLength"].toDouble();

      if (json_parameters.contains("InitialStepLengthXYZ")) {
        parameters_.initial_step_length_xyz = QList<double>();
        for (int i = 0; i < json_parameters["InitialStepLengthXYZ"].toArray().size(); ++i) {
          parameters_.initial_step_length_xyz.append(
              json_parameters["InitialStepLengthXYZ"].toArray().at(i).toDouble());
        }
      }

      if (json_parameters.contains("MinimumStepLengthXYZ")) {
        parameters_.minimum_step_length_xyz = QList<double>();
        for (int i = 0; i < json_parameters["MinimumStepLengthXYZ"].toArray().size(); ++i) {
          parameters_.minimum_step_length_xyz.append(
              json_parameters["MinimumStepLengthXYZ"].toArray().at(i).toDouble());
        }
      }

      if (json_parameters.contains("ContractionFactor"))
        parameters_.contraction_factor = json_parameters["ContractionFactor"].toDouble();
      else parameters_.contraction_factor = 0.5;

      if (json_parameters.contains("ExpansionFactor"))
        parameters_.expansion_factor = json_parameters["ExpansionFactor"].toDouble();
      else parameters_.expansion_factor = 1.0;

      if (json_parameters.contains("MaxQueueSize"))
        parameters_.max_queue_size = json_parameters["MaxQueueSize"].toDouble();
      else parameters_.max_queue_size = 2;

      if (json_parameters.contains("Pattern"))
        parameters_.pattern = json_parameters["Pattern"].toString();
      else parameters_.pattern = "Compass";

      // GA parameters
      if (json_parameters.contains("MaxGenerations"))
        parameters_.max_generations = json_parameters["MaxGenerations"].toInt();
      else parameters_.max_generations = 50;
      if (json_parameters.contains("PopulationSize"))
        parameters_.population_size = json_parameters["PopulationSize"].toInt();
      else parameters_.population_size = -1; // Will be properly set in optimizer.
      if (json_parameters.contains("CrossoverProbability"))
        parameters_.p_crossover = json_parameters["CrossoverProbability"].toDouble();
      else parameters_.p_crossover = 0.1;
      if (json_parameters.contains("DiscardParameter"))
        parameters_.discard_parameter = json_parameters["DiscardParameter"].toDouble();
      else parameters_.discard_parameter = -1; // Will be properly set in optimizer
      if (json_parameters.contains("DecayRate"))
        parameters_.decay_rate = json_parameters["DecayRate"].toDouble();
      else parameters_.decay_rate = 4.0;
      if (json_parameters.contains("MutationStrength"))
        parameters_.mutation_strength = json_parameters["MutationStrength"].toDouble();
      else parameters_.mutation_strength = 0.25;
      if (json_parameters.contains("StagnationLimit"))
        parameters_.stagnation_limit = json_parameters["StagnationLimit"].toDouble();
      else parameters_.stagnation_limit = 1e-10;
      if (json_parameters.contains("LowerBound"))
        parameters_.lower_bound = json_parameters["LowerBound"].toDouble();
      else parameters_.lower_bound = -10;
      if (json_parameters.contains("UpperBound"))
        parameters_.upper_bound = json_parameters["UpperBound"].toDouble();
      else parameters_.upper_bound = 10;
    }
    catch (std::exception const &ex) {
      throw UnableToParseOptimizerParametersSectionException(
          "Unable to parse optimizer parameters: " + std::string(ex.what()));
    }
  }

  // Optimizer objective
  try {
    QString objective_type = json_objective["Type"].toString();
    if (QString::compare(objective_type, "WeightedSum") == 0) {
      objective_.type = ObjectiveType::WeightedSum;
      objective_.weighted_sum = QList<Objective::WeightedSumComponent>();
      QJsonArray json_components = json_objective["WeightedSumComponents"].toArray();
      for (int i = 0; i < json_components.size(); ++i) {
        Objective::WeightedSumComponent component;
        component.coefficient = json_components.at(i).toObject()["Coefficient"].toDouble();
        component.property = json_components.at(i).toObject()["Property"].toString();
        if (json_components.at(i).toObject()["IsWellProp"].toBool()) {
          component.is_well_prop = true;
          component.well = json_components.at(i).toObject()["Well"].toString();
        }
        else component.is_well_prop = false;
        component.time_step = json_components.at(i).toObject()["TimeStep"].toInt();
        objective_.weighted_sum.append(component);
      }
    }
    else {
      throw UnableToParseOptimizerObjectiveSectionException(
          "Objective type " + objective_type.toStdString() + " not recognized");
    }
    if (json_objective.contains("UsePenaltyFunction")) {
      objective_.use_penalty_function = json_objective["UsePenaltyFunction"].toBool();
    }
    else {
      objective_.use_penalty_function = false;
    }
  }
  catch (std::exception const &ex) {
    throw UnableToParseOptimizerObjectiveSectionException(
        "Unable to parse optimizer objective: " + std::string(ex.what()));
  }

  // Optimizer constraints
  try {
    constraints_ = QList<Constraint>();
    for (int i = 0; i < json_constraints.size(); ++i) { // Iterate over all constraints
      QJsonObject json_constraint = json_constraints[i].toObject();
      constraints_.append(parseSingleConstraint(json_constraint));
    }
  }
  catch (std::exception const &ex) {
    throw UnableToParseOptimizerConstraintsSectionException(
        "Unable to parse optimizer constraints: " + std::string(ex.what()));
  }
}

Optimizer::Constraint Optimizer::parseSingleConstraint(QJsonObject json_constraint)
{
  Constraint optimizer_constraint;

  if (json_constraint.contains("Well")) {
    optimizer_constraint.well = json_constraint["Well"].toString();
    optimizer_constraint.wells.append(optimizer_constraint.well);
  }
  else if (json_constraint.contains("Wells") && json_constraint["Wells"].isArray()) {
    if (json_constraint["Wells"].toArray().size() == 1) {
      optimizer_constraint.well = json_constraint["Wells"].toArray()[0].toString();
    }
    for (auto wname : json_constraint["Wells"].toArray()) {
      optimizer_constraint.wells.append(wname.toString());
    }
  }
  else {
    throw std::runtime_error(
        "A constraint must always specify either the Well or the Wells property.");
  }

  // Penalty function weight for the constraint
  if (json_constraint.contains("PenaltyWeight")) {
    optimizer_constraint.penalty_weight = json_constraint["PenaltyWeight"].toDouble();
  }
  else {
    optimizer_constraint.penalty_weight = 0.0;
  }

  // Constraint types BHP, Rate and Boundary2D
  QString constraint_type = json_constraint["Type"].toString();
  if (QString::compare(constraint_type, "BHP") == 0) {
    optimizer_constraint.type = ConstraintType::BHP;
    if (json_constraint.contains("Max"))
      optimizer_constraint.max = json_constraint["Max"].toDouble();
    if (json_constraint.contains("Min"))
      optimizer_constraint.min = json_constraint["Min"].toDouble();
  }
  else if (QString::compare(constraint_type, "Rate") == 0) {
    optimizer_constraint.type = ConstraintType::Rate;
    if (json_constraint.contains("Max"))
      optimizer_constraint.max = json_constraint["Max"].toDouble();
    if (json_constraint.contains("Min"))
      optimizer_constraint.min = json_constraint["Min"].toDouble();
  }
  else if (QString::compare(constraint_type, "Boundary2D") == 0) {
    optimizer_constraint.type = ConstraintType::PseudoContBoundary2D;
    optimizer_constraint.box_imin = json_constraint["Imin"].toDouble();
    optimizer_constraint.box_imax = json_constraint["Imax"].toDouble();
    optimizer_constraint.box_jmin = json_constraint["Jmin"].toDouble();
    optimizer_constraint.box_jmax = json_constraint["Jmax"].toDouble();
  }


    // Constraint type Well Spline Points
  else if (QString::compare(constraint_type, "WellSplinePoints") == 0) {
    optimizer_constraint.type = ConstraintType::SplinePoints;

    // Spline points constraint input type
    QString optimizer_constraints_spline_points_type = json_constraint["WellSplinePointsInputType"].toString();
    if (QString::compare(optimizer_constraints_spline_points_type, "Function") == 0) {
      optimizer_constraint.spline_points_type = ConstraintWellSplinePointsType::Function;
      json_constraint["Function"].toString();
    }
    else if (QString::compare(optimizer_constraints_spline_points_type, "MaxMin") == 0) {
      optimizer_constraint.spline_points_type = ConstraintWellSplinePointsType::MaxMin;
      optimizer_constraint.spline_points_limits = QList<Constraint::RealMaxMinLimit>();
      QJsonArray well_spline_point_limits = json_constraint["WellSplinePointLimits"].toArray();
      for (int i = 0; i < well_spline_point_limits.size(); ++i) {
        QJsonObject well_spline_point_limit = well_spline_point_limits[i].toObject();
        QJsonArray min_array = well_spline_point_limit["Min"].toArray();
        QJsonArray max_array = well_spline_point_limit["Max"].toArray();
        Constraint::RealCoordinate min;
        min.x = min_array[0].toDouble();
        min.y = min_array[1].toDouble();
        min.z = min_array[2].toDouble();
        Constraint::RealCoordinate max;
        max.x = max_array[0].toDouble();
        max.y = max_array[1].toDouble();
        max.z = max_array[2].toDouble();
        Constraint::RealMaxMinLimit limit;
        limit.min = min;
        limit.max = max;
        optimizer_constraint.spline_points_limits.append(limit);
      }
    }
    else {
      throw UnableToParseOptimizerConstraintsSectionException(
          "Well spline constraint type not recognized.");
    }
  }
  else if (QString::compare(constraint_type, "WellSplineLength") == 0) {
    optimizer_constraint.type = ConstraintType::WellSplineLength;
    if (json_constraint.contains("Min")) {
      optimizer_constraint.min = json_constraint["Min"].toDouble();
      optimizer_constraint.min_length = json_constraint["Min"].toDouble();
    }
    else if (json_constraint.contains("MinLength")) {
      optimizer_constraint.min = json_constraint["MinLength"].toDouble();
      optimizer_constraint.min_length = json_constraint["MinLength"].toDouble();
    }
    else throw std::runtime_error("The MinLength field must be specified for well spline length constraints.");

    if (json_constraint.contains("Max")) {
      optimizer_constraint.max = json_constraint["Max"].toDouble();
      optimizer_constraint.max_length = json_constraint["Max"].toDouble();
    }
    else if (json_constraint.contains("MaxLength")) {
      optimizer_constraint.max = json_constraint["MaxLength"].toDouble();
      optimizer_constraint.max_length = json_constraint["MaxLength"].toDouble();
    }
    else throw std::runtime_error("The MaxLength field must be specified for well length constraints.");
  }
  else if (QString::compare(constraint_type, "WellSplineInterwellDistance") == 0) {
    optimizer_constraint.type = ConstraintType::WellSplineInterwellDistance;
    if (json_constraint.contains("Min")) {
      optimizer_constraint.min = json_constraint["Min"].toDouble();
      optimizer_constraint.min_distance = json_constraint["Min"].toDouble();
    }
    else if (json_constraint.contains("MinDistance")) {
      optimizer_constraint.min = json_constraint["MinDistance"].toDouble();
      optimizer_constraint.min_distance = json_constraint["MinDistance"].toDouble();
    }
    if (optimizer_constraint.wells.length() != 2)
      throw UnableToParseOptimizerConstraintsSectionException("WellSplineInterwellDistance constraint"
                                                                  " needs a Wells array with exactly two well names specified.");
  }

  else if (QString::compare(constraint_type, "ReservoirBoundary") == 0) {
    optimizer_constraint.type = ConstraintType::ReservoirBoundary;
    optimizer_constraint.box_imin = json_constraint["BoxImin"].toInt();
    optimizer_constraint.box_imax = json_constraint["BoxImax"].toInt();
    optimizer_constraint.box_jmin = json_constraint["BoxJmin"].toInt();
    optimizer_constraint.box_jmax = json_constraint["BoxJmax"].toInt();
    optimizer_constraint.box_kmin = json_constraint["BoxKmin"].toInt();
    optimizer_constraint.box_kmax = json_constraint["BoxKmax"].toInt();
  }
  else if (QString::compare(constraint_type, "CombinedWellSplineLengthInterwellDistance") == 0) {
    optimizer_constraint.type = ConstraintType::CombinedWellSplineLengthInterwellDistance;
    optimizer_constraint.min_length = json_constraint["MinLength"].toDouble();
    optimizer_constraint.max_length = json_constraint["MaxLength"].toDouble();
    optimizer_constraint.min_distance = json_constraint["MinDistance"].toDouble();
    optimizer_constraint.max_iterations = json_constraint["MaxIterations"].toInt();
    if (optimizer_constraint.wells.length() != 2)
      throw UnableToParseOptimizerConstraintsSectionException("WellSplineInterwellDistance constraint"
                                                                  " needs a Wells array with exactly two well names specified.");
  }
  else if (QString::compare(constraint_type, "CombinedWellSplineLengthInterwellDistanceReservoirBoundary") == 0) {
    optimizer_constraint.type = ConstraintType::CombinedWellSplineLengthInterwellDistanceReservoirBoundary;
    optimizer_constraint.min_length = json_constraint["MinLength"].toDouble();
    optimizer_constraint.max_length = json_constraint["MaxLength"].toDouble();
    optimizer_constraint.min_distance = json_constraint["MinDistance"].toDouble();
    optimizer_constraint.max_iterations = json_constraint["MaxIterations"].toInt();
    optimizer_constraint.box_imin = json_constraint["BoxImin"].toInt();
    optimizer_constraint.box_imax = json_constraint["BoxImax"].toInt();
    optimizer_constraint.box_jmin = json_constraint["BoxJmin"].toInt();
    optimizer_constraint.box_jmax = json_constraint["BoxJmax"].toInt();
    optimizer_constraint.box_kmin = json_constraint["BoxKmin"].toInt();
    optimizer_constraint.box_kmax = json_constraint["BoxKmax"].toInt();
    if (optimizer_constraint.wells.length() != 2)
      throw UnableToParseOptimizerConstraintsSectionException(
          "WellSplineInterwellDistanceReservoirBoundary constraint needs a Wells array with exactly two well names specified.");
  }
  else throw UnableToParseOptimizerConstraintsSectionException("Constraint type " + constraint_type.toStdString() + " not recognized.");
  return optimizer_constraint;
}

}
