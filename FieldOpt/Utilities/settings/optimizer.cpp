/******************************************************************************
 *
 * optimizer.cpp
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

#include "optimizer.h"
#include "settings_exceptions.h"

#include <QJsonArray>

namespace Utilities {
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
    else throw OptimizerTypeNotRecognizedException("The optimizer type " + type.toStdString() + " was not recognized.");

    // Optimizer parameters
    try {
        parameters_.max_evaluations = json_parameters["MaxEvaluations"].toInt();
        parameters_.initial_step_length = json_parameters["InitialStepLength"].toDouble();
        parameters_.minimum_step_length = json_parameters["MinimumStepLength"].toDouble();
    }
    catch (std::exception const &ex) {
        throw UnableToParseOptimizerParametersSectionException("Unable to parse optimizer parameters: " + std::string(ex.what()));
    }

    // Optimizer objective
    try {
        QString objective_type = json_objective["Type"].toString();
        if (QString::compare(objective_type, "Expression") == 0) {
            objective_.type = Optimizer::ObjectiveType::Expression;
            objective_.expression = json_objective["Expression"].toString();
        }
        else throw UnableToParseOptimizerObjectiveSectionException("Objective type " + objective_type.toStdString() + " not recognized");
    }
    catch (std::exception const &ex) {
        throw UnableToParseOptimizerObjectiveSectionException("Unable to parse optimizer objective: " + std::string(ex.what()));
    }

    // Optimizer constraints
    try {
        constraints_ = new QList<Constraint>();
        for (int i = 0; i < json_constraints.size(); ++i) { // Iterate over all constraints
            QJsonObject json_constraint = json_constraints[i].toObject();
            constraints_->append(parseSingleConstraint(json_constraint));
        }
    }
    catch (std::exception const &ex) {
        throw UnableToParseOptimizerConstraintsSectionException("Unable to parse optimizer constraints: " + std::string(ex.what()));
    }
}

Optimizer::Constraint Optimizer::parseSingleConstraint(QJsonObject json_constraint)
{
    Constraint optimizer_constraint;

    if (!json_constraint.contains("Name") || json_constraint["Name"].toString().size() < 1)
        throw UnableToParseOptimizerConstraintsSectionException("All constraints must specify a name corresponding to a unique variable name.");
    optimizer_constraint.name = json_constraint["Name"].toString();

    // Constraint types BHP and Rate
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

    // Constraint type Well Spline Points
    else if (QString::compare(constraint_type, "WellSplinePoints") == 0) {
        optimizer_constraint.type = ConstraintType::SplinePoints;

        // Spline points constraint input type
        QString optimizer_constraints_spline_points_type = json_constraint["WellSplinePointsInputType"].toString();
        if (QString::compare(optimizer_constraints_spline_points_type, "Function") == 0) {
            optimizer_constraint.spline_points_type = ConstraintWellSplinePointsType::Function;
            optimizer_constraint.well_spline_points_function = json_constraint["Function"].toString();
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
        else throw UnableToParseOptimizerConstraintsSectionException("Well spline constraint type not recognized.");
    }
    else throw UnableToParseOptimizerConstraintsSectionException("Constraint type " + constraint_type.toStdString() + " not recognized.");

    optimizer_constraint.well = json_constraint["Well"].toString();
    return optimizer_constraint;
}

}
}
