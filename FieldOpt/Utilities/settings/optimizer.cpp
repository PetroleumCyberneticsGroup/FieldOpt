#include "optimizer.h"
#include "settings_exceptions.h"

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

            // Optimizer mode
            if (json_optimizer.contains("Mode")) {
                QString mode = json_optimizer["Mode"].toString();
                if (QString::compare(mode, "Minimize", Qt::CaseInsensitive) == 0)
                    mode_ = OptimizerMode::Minimize;
                else if (QString::compare(mode, "Maximize", Qt::CaseInsensitive) == 0)
                    mode_ = OptimizerMode::Maximize;
                else throw UnableToParseOptimizerSectionException("Did not recognize optimizer Mode setting.");
            } else throw UnableToParseOptimizerSectionException("Optimizer Mode keyword must be specified.");

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
                else throw UnableToParseOptimizerObjectiveSectionException("Objective type " + objective_type.toStdString() + " not recognized");
            }
            catch (std::exception const &ex) {
                throw UnableToParseOptimizerObjectiveSectionException("Unable to parse optimizer objective: " + std::string(ex.what()));
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
                throw UnableToParseOptimizerConstraintsSectionException("Unable to parse optimizer constraints: " + std::string(ex.what()));
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
                foreach (auto wname, json_constraint["Wells"].toArray()) {
                    optimizer_constraint.wells.append(wname.toString());
                }
            }
            else throw std::runtime_error("A constraint must always specify either the Well or the Wells property.");

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
                else throw UnableToParseOptimizerConstraintsSectionException("Well spline constraint type not recognized.");
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
                    throw UnableToParseOptimizerConstraintsSectionException("WellSplineInterwellDistance constraint needs a Wells array with exactly two well names specified.");
            }
            else if (QString::compare(constraint_type, "CombinedWellSplineLengthInterwellDistance") == 0) {
                optimizer_constraint.type = ConstraintType::CombinedWellSplineLengthInterwellDistance;
                optimizer_constraint.min_length = json_constraint["MinLength"].toDouble();
                optimizer_constraint.max_length = json_constraint["MaxLength"].toDouble();
                optimizer_constraint.min_distance = json_constraint["MinDistance"].toDouble();
                optimizer_constraint.max_iterations = json_constraint["MaxIterations"].toInt();
                if (optimizer_constraint.wells.length() != 2)
                    throw UnableToParseOptimizerConstraintsSectionException("WellSplineInterwellDistance constraint needs a Wells array with exactly two well names specified.");
            }
            else throw UnableToParseOptimizerConstraintsSectionException("Constraint type " + constraint_type.toStdString() + " not recognized.");
            return optimizer_constraint;
        }

    }
}
