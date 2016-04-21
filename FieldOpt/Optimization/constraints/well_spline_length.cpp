#include "well_spline_length.h"
#include "WellIndexCalculator/well_constraint_projections/well_constraint_projections.h"
#include <Eigen/Core>

namespace Optimization { namespace Constraints {

WellSplineLength::WellSplineLength(Utilities::Settings::Optimizer::Constraint settings,
                                   Model::Properties::VariablePropertyContainer *variables)
    : Constraint(settings, variables), WellSplineConstraint()
{
    min_length_ = settings.min;
    max_length_ = settings.max;

    QList<QPair<QUuid, QString>> all_matching_vars = variables->GetContinousVariableNamesAndIdsMatchingSubstring(settings.name);
    affected_well_ = initializeWell(all_matching_vars, settings.well);
}

bool WellSplineLength::CaseSatisfiesConstraint(Case *c)
{
    double heel_x_val = c->real_variables()[affected_well_.heel.x];
    double heel_y_val = c->real_variables()[affected_well_.heel.y];
    double heel_z_val = c->real_variables()[affected_well_.heel.z];

    double toe_x_val = c->real_variables()[affected_well_.toe.x];
    double toe_y_val = c->real_variables()[affected_well_.toe.y];
    double toe_z_val = c->real_variables()[affected_well_.toe.z];

    Eigen::Vector3d heel_vals;
    Eigen::Vector3d toe_vals;
    heel_vals << heel_x_val, heel_y_val, heel_z_val;
    toe_vals << toe_x_val, toe_y_val, toe_z_val;

    QList<Eigen::Vector3d> projection = WellIndexCalculator::WellConstraintProjections::well_length_projection_eigen(
                heel_vals, toe_vals,
                max_length_, min_length_,
                0.001);

    if (heel_vals.isApprox(projection.first(), 0.01) && toe_vals.isApprox(projection.last(), 0.01))
        return true;
    else
        return false;
}

void WellSplineLength::SnapCaseToConstraints(Case *c)
{
    double heel_x_val = c->real_variables()[affected_well_.heel.x];
    double heel_y_val = c->real_variables()[affected_well_.heel.y];
    double heel_z_val = c->real_variables()[affected_well_.heel.z];

    double toe_x_val = c->real_variables()[affected_well_.toe.x];
    double toe_y_val = c->real_variables()[affected_well_.toe.y];
    double toe_z_val = c->real_variables()[affected_well_.toe.z];

    Eigen::Vector3d heel_vals;
    Eigen::Vector3d toe_vals;
    heel_vals << heel_x_val, heel_y_val, heel_z_val;
    toe_vals << toe_x_val, toe_y_val, toe_z_val;

    QList<Eigen::Vector3d> projection = WellIndexCalculator::WellConstraintProjections::well_length_projection_eigen(
                heel_vals, toe_vals,
                max_length_, min_length_,
                0.001);

    c->set_real_variable_value(affected_well_.heel.x, projection.first()(0));
    c->set_real_variable_value(affected_well_.heel.y, projection.first()(1));
    c->set_real_variable_value(affected_well_.heel.z, projection.first()(2));

    c->set_real_variable_value(affected_well_.toe.x, projection.last()(0));
    c->set_real_variable_value(affected_well_.toe.y, projection.last()(1));
    c->set_real_variable_value(affected_well_.toe.z, projection.last()(2));
}

}}
