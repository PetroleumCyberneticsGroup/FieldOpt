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

#include "Utilities/printer.hpp"
#include "optimizer.h"
#include "settings_exceptions.h"
#include "Settings/helpers.hpp"

namespace Settings {

Optimizer::Optimizer(QJsonObject json_optimizer)
{
    // Get the root objects.
    QJsonObject json_parameters = json_optimizer["Parameters"].toObject();
    QJsonObject json_objective = json_optimizer["Objective"].toObject();
    QJsonArray json_constraints = json_optimizer["Constraints"].toArray();
    QString type = json_optimizer["Type"].toString();

    type_ = parseType(type);

    if (type_ != ExhaustiveSearch2DVert) {
        mode_ = parseMode(json_optimizer);
        if (type_ == Hybrid) {
            hybrid_components_ = parseHybridComponents(json_optimizer);
        }
        parameters_ = parseParameters(json_parameters);
    }
    objective_ = parseObjective(json_objective);


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
        for (auto wname : json_constraint["Wells"].toArray()) {
            optimizer_constraint.wells.append(wname.toString());
        }
    }
    else throw std::runtime_error("A constraint must always specify either the Well or the Wells property.");

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
        // Packer- and ICV Constraints
    else if (QString::compare(constraint_type, "ICVConstraint") == 0) {
        optimizer_constraint.type = ConstraintType::ICVConstraint;
        optimizer_constraint.max = json_constraint["Max"].toDouble();
        if (optimizer_constraint.max >= 7.8540E-03) {
            Printer::ext_warn("Maximum valve size is too big. Setting it to 7.8539-3.", "Settings", "Optimizer");
            optimizer_constraint.max = 7.8539E-3;
        }
        optimizer_constraint.min = json_constraint["Min"].toDouble();
    }
    else if (QString::compare(constraint_type, "PackerConstraint") == 0) {
        optimizer_constraint.type = ConstraintType::PackerConstraint;
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
        else throw UnableToParseOptimizerConstraintsSectionException("Well spline constraint type not recognized.");
    }
    else if (QString::compare(constraint_type, "WellSplineLength") == 0 || QString::compare(constraint_type, "PolarWellLength") == 0 ) {
        if (constraint_type == "WellSplineLength"){
            optimizer_constraint.type = ConstraintType::WellSplineLength;
        } else {
            optimizer_constraint.type = ConstraintType::PolarWellLength;
        }
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
    else if (QString::compare(constraint_type, "PolarAzimuth") == 0 || QString::compare(constraint_type, "PolarElevation") == 0){
        if (constraint_type == "PolarAzimuth") {
            optimizer_constraint.type = ConstraintType::PolarAzimuth;
        } else {
            optimizer_constraint.type = ConstraintType::PolarElevation;
        }
        if (json_constraint.contains("Max")){
            optimizer_constraint.max = json_constraint["Max"].toDouble();
        }
        if (json_constraint.contains("Min")){
            optimizer_constraint.min = json_constraint["Min"].toDouble();
        }
    }

    else if (QString::compare(constraint_type, "ReservoirBoundary") == 0 || QString::compare(constraint_type, "PolarSplineBoundary") == 0 ||
    QString::compare(constraint_type, "ReservoirBoundaryToe") == 0) {
        if (QString::compare(constraint_type, "ReservoirBoundary") == 0){
            optimizer_constraint.type = ConstraintType::ReservoirBoundary;
        }
        else if (QString::compare(constraint_type, "PolarSplineBoundary") == 0){
            optimizer_constraint.type = ConstraintType::PolarSplineBoundary;
        }
        else if (QString::compare(constraint_type, "ReservoirBoundaryToe") == 0){
            optimizer_constraint.type = ConstraintType::ReservoirBoundaryToe;
        }
        optimizer_constraint.box_imin = json_constraint["BoxImin"].toInt();
        optimizer_constraint.box_imax = json_constraint["BoxImax"].toInt();
        optimizer_constraint.box_jmin = json_constraint["BoxJmin"].toInt();
        optimizer_constraint.box_jmax = json_constraint["BoxJmax"].toInt();
        optimizer_constraint.box_kmin = json_constraint["BoxKmin"].toInt();
        optimizer_constraint.box_kmax = json_constraint["BoxKmax"].toInt();
    }
    else if (QString::compare(constraint_type, "PolarXYZBoundary") == 0 || QString::compare(constraint_type, "ReservoirXYZBoundary") == 0){
        if(QString::compare(constraint_type, "PolarXYZBoundary") == 0){
            optimizer_constraint.type = ConstraintType::PolarXYZBoundary;
        }else{
            optimizer_constraint.type = ConstraintType::ReservoirXYZBoundary;
        }
        optimizer_constraint.box_xyz_xmin = json_constraint["xMin"].toDouble();
        optimizer_constraint.box_xyz_ymin = json_constraint["yMin"].toDouble();
        optimizer_constraint.box_xyz_zmin = json_constraint["zMin"].toDouble();
        optimizer_constraint.box_xyz_xmax = json_constraint["xMax"].toDouble();
        optimizer_constraint.box_xyz_ymax = json_constraint["yMax"].toDouble();
        optimizer_constraint.box_xyz_zmax = json_constraint["zMax"].toDouble();
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
Optimizer::OptimizerMode Optimizer::parseMode(QJsonObject &json_optimizer) {
    OptimizerMode opt_mode;
    if (json_optimizer.contains("Mode")) {
        QString mode = json_optimizer["Mode"].toString();
        if (QString::compare(mode, "Minimize", Qt::CaseInsensitive) == 0)
            opt_mode = OptimizerMode::Minimize;
        else if (QString::compare(mode, "Maximize", Qt::CaseInsensitive) == 0)
            opt_mode = OptimizerMode::Maximize;
        else
            throw UnableToParseOptimizerSectionException("Did not recognize optimizer Mode setting.");
    }
    else throw UnableToParseOptimizerSectionException("Optimizer Mode keyword must be specified.");
    return opt_mode;
}
Optimizer::Parameters Optimizer::parseParameters(QJsonObject &json_parameters) {
    Parameters params;

    try {
        // GSS parameters
        if (json_parameters.contains("MaxEvaluations"))
            params.max_evaluations = json_parameters["MaxEvaluations"].toInt();
        if (json_parameters.contains("AutoStepLengths"))
            params.auto_step_lengths = json_parameters["AutoStepLengths"].toBool();
        if (json_parameters.contains("AutoStepInitScale"))
            params.auto_step_init_scale = json_parameters["AutoStepInitScale"].toDouble();
        if (json_parameters.contains("AutoStepConvScale"))
            params.auto_step_conv_scale = json_parameters["AutoStepConvScale"].toDouble();
        if (json_parameters.contains("InitialStepLength"))
            params.initial_step_length = json_parameters["InitialStepLength"].toDouble();
        if (json_parameters.contains("MinimumStepLength"))
            params.minimum_step_length = json_parameters["MinimumStepLength"].toDouble();
        if (json_parameters.contains("ContractionFactor"))
            params.contraction_factor = json_parameters["ContractionFactor"].toDouble();
        else params.contraction_factor = 0.5;
        if (json_parameters.contains("ExpansionFactor"))
            params.expansion_factor = json_parameters["ExpansionFactor"].toDouble();
        else params.expansion_factor = 1.0;
        if (json_parameters.contains("MaxQueueSize"))
            params.max_queue_size = json_parameters["MaxQueueSize"].toDouble();
        else params.max_queue_size = 2;
        if (json_parameters.contains("Pattern"))
            params.pattern = json_parameters["Pattern"].toString();
        else params.pattern = "Compass";

        // GA parameters
        if (json_parameters.contains("MaxGenerations"))
            params.max_generations = json_parameters["MaxGenerations"].toInt();
        else params.max_generations = 50;
        if (json_parameters.contains("PopulationSize"))
            params.population_size = json_parameters["PopulationSize"].toInt();
        else params.population_size = -1; // Will be properly set in optimizer.
        if (json_parameters.contains("CrossoverProbability"))
            params.p_crossover = json_parameters["CrossoverProbability"].toDouble();
        else params.p_crossover = 0.1;
        if (json_parameters.contains("DiscardParameter"))
            params.discard_parameter = json_parameters["DiscardParameter"].toDouble();
        else params.discard_parameter = -1; // Will be properly set in optimizer
        if (json_parameters.contains("DecayRate"))
            params.decay_rate = json_parameters["DecayRate"].toDouble();
        else params.decay_rate = 4.0;
        if (json_parameters.contains("MutationStrength"))
            params.mutation_strength = json_parameters["MutationStrength"].toDouble();
        else params.mutation_strength = 0.25;
        if (json_parameters.contains("StagnationLimit"))
            params.stagnation_limit = json_parameters["StagnationLimit"].toDouble();
        else params.stagnation_limit = 1e-10;
        if (json_parameters.contains("LowerBound"))
            params.lower_bound = json_parameters["LowerBound"].toDouble();
        else params.lower_bound = -10;
        if (json_parameters.contains("UpperBound"))
            params.upper_bound = json_parameters["UpperBound"].toDouble();
        else params.upper_bound = 10;

        // PSO parameters
        if(json_parameters.contains("PSO-LearningFactor1")){
            params.pso_learning_factor_1 = json_parameters["PSO-LearningFactor1"].toDouble();
        }else params.pso_learning_factor_1 = 2;
        if(json_parameters.contains("PSO-LearningFactor2")){
            params.pso_learning_factor_2 = json_parameters["PSO-LearningFactor2"].toDouble();
        }else params.pso_learning_factor_2 = 2;
        if(json_parameters.contains("PSO-SwarmSize")){
            params.pso_swarm_size = json_parameters["PSO-SwarmSize"].toDouble();
        }else params.pso_swarm_size = 50;
        if(json_parameters.contains("PSO-VelocityScale")){
            params.pso_velocity_scale = json_parameters["PSO-VelocityScale"].toDouble();
        }else params.pso_velocity_scale = 1.0;

        // EGO Parameters
        if (json_parameters.contains("EGO-InitGuesses")) {
            params.ego_init_guesses = json_parameters["EGO-InitGuesses"].toInt();
        }
        if (json_parameters.contains("EGO-InitSamplingMethod")) {
            QString method = json_parameters["EGO-InitSamplingMethod"].toString();
            if (QString::compare(method, "Random") == 0 || QString::compare(method, "Uniform") == 0)
                params.ego_init_sampling_method = json_parameters["EGO-InitSamplingMethod"].toString().toStdString();
            else {
                Printer::error("EGO-InitSamplingMethod " + method.toStdString() + " not recognized.");
                throw std::runtime_error("Failed reading EGO settings.");
            }
        }
        if (json_parameters.contains("EGO-Kernel")) {
            QStringList available_kernels = { "CovLinearard", "CovLinearone", "CovMatern3iso",
                                              "CovMatern5iso", "CovNoise", "CovRQiso", "CovSEard",
                                              "CovSEiso", "CovPeriodicMatern3iso", "CovPeriodic"};
            if (available_kernels.contains(json_parameters["EGO-Kernel"].toString())) {
                params.ego_kernel = json_parameters["EGO-Kernel"].toString().toStdString();
            }
            else {
                Printer::error("EGO-Kernel " + json_parameters["EGO-Kernel"].toString().toStdString() + " not recognized.");
                Printer::info("Available kernels: " + available_kernels.join(", ").toStdString());
                throw std::runtime_error("Failed reading EGO settings.");
            }
        }
        if (json_parameters.contains("EGO-AF")) {
            QStringList available_afs = { "ExpectedImprovement", "ProbabilityOfImprovement" };
            if (available_afs.contains(json_parameters["EGO-AF"].toString())) {
                params.ego_af = json_parameters["EGO-AF"].toString().toStdString();
            }
            else {
                Printer::error("EGO-AF " + json_parameters["EGO-AF"].toString().toStdString() + " not recognized.");
                Printer::info("Available acquisition functions: " + available_afs.join(", ").toStdString());
                throw std::runtime_error("Failed reading EGO settings.");
            }
        }

        // CMA-ES Parameters
        if (json_parameters.contains("ImproveBaseCase")) {
            params.improve_base_case = json_parameters["ImproveBaseCase"].toBool();
        }

        // VFSA Parameters
        if (json_parameters.contains("VFSA-EvalsPrIteration")) {
            params.vfsa_evals_pr_iteration = json_parameters["VFSA-EvalsPrIteration"].toInt();
        }
        if (json_parameters.contains("VFSA-MaxIterations")) {
            params.vfsa_max_iterations = json_parameters["VFSA-MaxIterations"].toInt();
        }
        if (json_parameters.contains("VFSA-Parallel")) {
            params.vfsa_parallel = json_parameters["VFSA-Parallel"].toBool();
        }
        if (json_parameters.contains("VFSA-InitTemp")) {
            params.vfsa_init_temp = json_parameters["VFSA-InitTemp"].toDouble();
        }
        if (json_parameters.contains("VFSA-TempScale")) {
            params.vfsa_temp_scale = json_parameters["VFSA-TempScale"].toDouble();
        }

        // SPSA Parameters
        if (json_parameters.contains("SPSA-MaxIterations")) {
            params.spsa_max_iterations = json_parameters["SPSA-MaxIterations"].toInt();
        }
        if (json_parameters.contains("SPSA-gamma")) {
            params.spsa_gamma = json_parameters["SPSA-gamma"].toDouble();
        }
        if (json_parameters.contains("SPSA-alpha")) {
            params.spsa_alpha = json_parameters["SPSA-alpha"].toDouble();
        }
        if (json_parameters.contains("SPSA-c")) {
            params.spsa_c = json_parameters["SPSA-c"].toDouble();
        }
        if (json_parameters.contains("SPSA-A")) {
            params.spsa_A = json_parameters["SPSA-A"].toDouble();
        } else params.spsa_A = 0.1 * params.spsa_max_iterations;
        if (json_parameters.contains("SPSA_a")) {
            params.spsa_a = json_parameters["SPSA_a"].toDouble();
        }
        if (json_parameters.contains("SPSA-InitStepMagnitude")) {
            params.spsa_init_step_magnitude = json_parameters["SPSA-InitStepMagnitude"].toDouble();
        }


        // Hybrid parameters
        if (json_parameters.contains("HybridSwitchMode")) {
            if (json_parameters["HybridSwitchMode"].toString() == "OnConvergence") {
                params.hybrid_switch_mode = "OnConvergence";
            }
            else {
                throw std::runtime_error("HybridSwitchMode setting not recognized.");
            }
        }
        if (json_parameters.contains("HybridTerminationCondition")) {
            if (json_parameters["HybridTerminationCondition"].toString() == "NoImprovement") {
                params.hybrid_termination_condition = "NoImprovement";
            }
            else {
                throw std::runtime_error("HybridTerminationCondition setting not recognized.");
            }
        }
        if (json_parameters.contains("HybridMaxIterations")) {
            if (json_parameters["HybridMaxIterations"].toInt() >= 1) {
                params.hybrid_max_iterations = json_parameters["HybridMaxIterations"].toInt();
            }
            else {
                throw std::runtime_error("Invalid value for setting HybridMaxIterations");
            }
        }


        // RNG seed
        if (json_parameters.contains("RNGSeed")) {
            Printer::error("Passing RNGSeed is no longer supported.");
            Printer::ext_warn("Passing RNGSeed in settings is no longer supporte. In fact, it never really worked: this"
                              "is why it has now been removed. Remove the RNGSeed line from your driver file and try again.",
                              "Settings", "Optimization");
            throw std::runtime_error("Error in driver file");
        }
        else {
            params.rng_seed = 0;
        }
    }
    catch (std::exception const &ex) {
        throw UnableToParseOptimizerParametersSectionException("Unable to parse optimizer parameters: " + std::string(ex.what()));
    }

    return params;
}
Optimizer::Objective Optimizer::parseObjective(QJsonObject &json_objective) {
    Objective obj;

    try {
        QString objective_type = json_objective["Type"].toString();
        if (QString::compare(objective_type, "WeightedSum") == 0) {
            obj.type = ObjectiveType::WeightedSum;
            obj.weighted_sum = QList<Objective::WeightedSumComponent>();
            QJsonArray json_components = json_objective["WeightedSumComponents"].toArray();
            for (int i = 0; i < json_components.size(); ++i) {
                Objective::WeightedSumComponent component;
                component.coefficient = json_components.at(i).toObject()["Coefficient"].toDouble();
                component.property = json_components.at(i).toObject()["Property"].toString();
                if (json_components.at(i).toObject()["IsWellProp"].toBool()) {
                    component.is_well_prop = true;
                    component.well = json_components.at(i).toObject()["Well"].toString();
                } else component.is_well_prop = false;
                component.time_step = json_components.at(i).toObject()["TimeStep"].toInt();
                obj.weighted_sum.append(component);
            }
        }
        else if (QString::compare(objective_type, "NPV") == 0 || QString::compare(objective_type, "carbondioxidenpv") == 0) {
            // -------------------------------------------------
            if (QString::compare(objective_type, "NPV") == 0){
                obj.type = ObjectiveType::NPV;
            } else if (QString::compare(objective_type, "carbondioxidenpv") == 0){
                obj.type = ObjectiveType::carbondioxidenpv;
            }

            obj.NPV_sum = QList<Objective::NPVComponent>();
            // ---------------------------------------------------
            QJsonArray json_components = json_objective["NPVComponents"].toArray();
            // ---------------------------------------------------
            for (int i = 0; i < json_components.size(); ++i) {
                // -------------------------------------------------
                Objective::NPVComponent component;
                set_req_prop_double(component.coefficient, json_components[i].toObject(), "Coefficient");
                set_req_prop_string(component.property, json_components[i].toObject(), "Property");
                set_opt_prop_double(component.discount, json_components[i].toObject(), "DiscountFactor");
                set_req_prop_string(component.interval, json_components[i].toObject(), "Interval");
                set_opt_prop_bool(component.usediscountfactor, json_components[i].toObject(), "UseDiscountFactor");
                obj.NPV_sum.append(component);
            }

            if (QString::compare(objective_type, "carbondioxidenpv") == 0){
                obj.NPVCarbonComponents = QList<Objective::NPVCarbonComponent>();
                QJsonArray json_carbon_components = json_objective["NPVCarbonComponents"].toArray();
                //Objective::NPVCarbonComponent carbonComponent;
                for (int i = 0; i < json_carbon_components.size(); ++i) {
                    Objective::NPVCarbonComponent carbonComponent;
                    carbonComponent.property = json_carbon_components.at(i).toObject()["Property"].toString();
                    carbonComponent.is_well_prop = json_carbon_components.at(i).toObject()["IsWellProp"].toBool();
                    if (carbonComponent.is_well_prop == true) {
                        carbonComponent.well = json_carbon_components[i].toObject()["Well"].toString();
                        carbonComponent.well_tvd = json_carbon_components[i].toObject()["WellTVD"].toDouble();
                    }
                    //set_req_prop_double(carbonComponent.coefficient, json_carbon_components[i].toObject(), "Coefficient");
                    //set_req_prop_double(carbonComponent.coefficient, json_carbon_components[i].toObject(), "Capacity");
                    obj.NPVCarbonComponents.append(carbonComponent);
                }

                if (json_objective.contains("AdditionalComponents")) {
                    QJsonObject json_additional_components = json_objective["AdditionalComponents"].toObject();
                    if (json_additional_components.contains("NumberOfPumps")){
                        obj.npump = json_additional_components["NumberOfPumps"].toInt();
                    }
                    if (json_additional_components.contains("PowerOfEachTurbine")){
                        obj.pow_gen_per_turbine = json_additional_components["PowerOfEachTurbine"].toDouble();
                    }
                    if (json_additional_components.contains("CostOfEachTurbine")){
                        obj.cost_per_turbine = json_additional_components["CostOfEachTurbine"].toDouble();
                    }
                    if (json_additional_components.contains("CO2TaxRate")){
                        obj.CO2_tax_rate = json_additional_components["CO2TaxRate"].toDouble();
                    }
                }
            }
        } else
            throw UnableToParseOptimizerObjectiveSectionException(
                "Objective type " + objective_type.toStdString() + " not recognized");
        if (json_objective.contains("UsePenaltyFunction")) {
            obj.use_penalty_function = json_objective["UsePenaltyFunction"].toBool();
        }
        else {
            obj.use_penalty_function = false;
        }
        if(json_objective.contains("SeparateHorizontalAndVertical")){
            obj.separatehorizontalandvertical=json_objective["SeparateHorizontalAndVertical"].toBool();
        } else {
            obj.separatehorizontalandvertical= false;
        }
        if (json_objective.contains("UseWellCost")) {
            obj.use_well_cost = json_objective["UseWellCost"].toBool();
            if (obj.separatehorizontalandvertical) {
                if (json_objective.contains("WellCostXY")) {
                    obj.wellCostXY = json_objective["WellCostXY"].toDouble();
                    if (json_objective.contains("WellCostZ")) {
                        obj.wellCostZ = json_objective["WellCostZ"].toDouble();
                    } else {
                        throw UnableToParseOptimizerObjectiveSectionException(
                            "Unable to parse optimizer objective a WellCostZ was not defined, while SeparateHorizontalAndVertical was invoked");
                    }
                } else {
                    throw UnableToParseOptimizerObjectiveSectionException(
                        "Unable to parse optimizer objective a WellCostXY was not defined, while SeparateHorizontalAndVertical was invoked");
                }
            } else {
                if (json_objective.contains("WellCost")) {
                    obj.wellCost = json_objective["WellCost"].toDouble();
                    obj.wellCostXY = 0;
                    obj.wellCostZ = 0;
                } else {
                    throw UnableToParseOptimizerObjectiveSectionException(
                        "Unable to parse optimizer objective a WellCost was not defined, while UseWellCost was invoked");
                }
            }

        } else {
            obj.use_well_cost = false;
            obj.wellCost = 0;
            obj.wellCostXY = 0;
            obj.wellCostZ = 0;
        }

    }
    catch (std::exception const &ex) {
        throw UnableToParseOptimizerObjectiveSectionException(
            "Unable to parse optimizer objective: " + std::string(ex.what()));
    }

    return obj;
}
Optimizer::OptimizerType Optimizer::parseType(QString &type) {
    OptimizerType opt_type;
    if (QString::compare(type, "Compass") == 0)
        opt_type = OptimizerType::Compass;
    else if (QString::compare(type, "APPS") == 0)
        opt_type = OptimizerType::APPS;
    else if (QString::compare(type, "GeneticAlgorithm") == 0)
        opt_type = OptimizerType::GeneticAlgorithm;
    else if (QString::compare(type, "EGO") == 0)
        opt_type = OptimizerType::EGO;
    else if (QString::compare(type, "ExhaustiveSearch2DVert") == 0)
        opt_type = OptimizerType::ExhaustiveSearch2DVert;
    else if (QString::compare(type, "PSO") == 0)
        opt_type = OptimizerType::PSO;
    else if (QString::compare(type, "CMA_ES") == 0)
        opt_type = OptimizerType::CMA_ES;
    else if (QString::compare(type, "VFSA") == 0)
        opt_type = OptimizerType::VFSA;
    else if (QString::compare(type, "SPSA") == 0)
        opt_type = OptimizerType::SPSA;
    else if (QString::compare(type, "Hybrid") == 0)
        opt_type = OptimizerType::Hybrid;
    else throw OptimizerTypeNotRecognizedException("The optimizer type " + type.toStdString() + " was not recognized.");
    return opt_type;
}
QList<Optimizer::HybridComponent> Optimizer::parseHybridComponents(QJsonObject &json_optimizer) {
    QList<HybridComponent> comps;
    for (auto json_comp : json_optimizer["HybridComponents"].toArray()) {
        HybridComponent comp;
        QString type = json_comp.toObject()["Type"].toString();
        QJsonObject json_params = json_comp.toObject()["Parameters"].toObject();
        comp.type = parseType(type);
        comp.parameters = parseParameters(json_params);
        comps.push_back(comp);
    }
    return comps;
}
Optimizer::Optimizer(Optimizer::HybridComponent hc) {
    type_ = hc.type;
    parameters_ = hc.parameters;
}

}
