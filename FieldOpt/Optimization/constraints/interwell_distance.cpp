#include "interwell_distance.h"
#include "WellIndexCalculator/well_constraint_projections/well_constraint_projections.h"

namespace Optimization { namespace Constraints {

InterwellDistance::InterwellDistance(Utilities::Settings::Optimizer::Constraint settings,
                                     Model::Properties::VariablePropertyContainer *variables)
    : Constraint(settings, variables)
{
    distance_ = settings.min;

    QList<QPair<QUuid, QString>> vars_matching_constr_name = variables->GetContinousVariableNamesAndIdsMatchingSubstring(settings.name);

    foreach (QString name, settings.wells) {
        affected_wells_.append(initializeWell(vars_matching_constr_name, name));
    }
    if (affected_wells_.length() != 2) {
        throw std::runtime_error("Currently, the Interwell Distance constraint must be applied to exactly two wells. Found " + std::to_string(affected_wells_.length()));
    }
}

bool InterwellDistance::CaseSatisfiesConstraint(Case *c)
{
    QList<Eigen::Vector3d> points;
    foreach (Well well, affected_wells_) {
        double heel_x_val = c->real_variables()[well.heel.x];
        double heel_y_val = c->real_variables()[well.heel.y];
        double heel_z_val = c->real_variables()[well.heel.z];

        double toe_x_val = c->real_variables()[well.toe.x];
        double toe_y_val = c->real_variables()[well.toe.y];
        double toe_z_val = c->real_variables()[well.toe.z];

        Eigen::Vector3d heel_vals;
        Eigen::Vector3d toe_vals;
        heel_vals << heel_x_val, heel_y_val, heel_z_val;
        toe_vals << toe_x_val, toe_y_val, toe_z_val;
        points.append(heel_vals);
        points.append(toe_vals);
    }
    // Get the projection
    QList<Eigen::Vector3d> projection = WellIndexCalculator::WellConstraintProjections::interwell_constraint_projection_eigen(points, distance_);

    // Check if the projection is (approximately) equal to the input case
    for (int i = 0; i < projection.length(); ++i) {
        if (!points[i].isApprox(projection[i], 0.01))
            return false;
    }
    return true;
}

void InterwellDistance::SnapCaseToConstraints(Case *c)
{
    QList<Eigen::Vector3d> points;
    foreach (Well well, affected_wells_) {
        double heel_x_val = c->real_variables()[well.heel.x];
        double heel_y_val = c->real_variables()[well.heel.y];
        double heel_z_val = c->real_variables()[well.heel.z];

        double toe_x_val = c->real_variables()[well.toe.x];
        double toe_y_val = c->real_variables()[well.toe.y];
        double toe_z_val = c->real_variables()[well.toe.z];

        Eigen::Vector3d heel_vals;
        Eigen::Vector3d toe_vals;
        heel_vals << heel_x_val, heel_y_val, heel_z_val;
        toe_vals << toe_x_val, toe_y_val, toe_z_val;
        points.append(heel_vals);
        points.append(toe_vals);
    }
    // Get the projection
    QList<Eigen::Vector3d> projection = WellIndexCalculator::WellConstraintProjections::interwell_constraint_projection_eigen(points, distance_);

    for (int i = 0; i < affected_wells_.length(); ++i) {
        c->set_real_variable_value(affected_wells_[i].heel.x, projection[i*2](0));
        c->set_real_variable_value(affected_wells_[i].heel.y, projection[i*2](1));
        c->set_real_variable_value(affected_wells_[i].heel.z, projection[i*2](2));

        c->set_real_variable_value(affected_wells_[i].toe.x, projection[i*2+1](0));
        c->set_real_variable_value(affected_wells_[i].toe.y, projection[i*2+1](1));
        c->set_real_variable_value(affected_wells_[i].toe.z, projection[i*2+1](2));
    }
}


}}
