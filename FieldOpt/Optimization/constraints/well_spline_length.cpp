#include "well_spline_length.h"
#include "WellIndexCalculator/well_constraint_projections/well_constraint_projections.h"
#include <Eigen/Core>
#include <iostream>

namespace Optimization { namespace Constraints {

WellSplineLength::WellSplineLength(Utilities::Settings::Optimizer::Constraint settings,
                                   Model::Properties::VariablePropertyContainer *variables)
    : Constraint(settings, variables)
{
    min_length_ = settings.min;
    max_length_ = settings.max;
    affected_well_.name = settings.name;

    QList<QPair<QUuid, QString>> all_matching_vars = variables->GetContinousVariableNamesAndIdsMatchingSubstring(settings.name);
    initializeWell(all_matching_vars);
    std::cout << "Matching vars: " << all_matching_vars.size() << std::endl;
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
    std::cout << "SNAPPING SHIT TO CONSTRAINTS YO!" << std::endl;
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

    std::cout << "Old heel x: " << heel_x_val << std::endl;
    std::cout << "Old heel y: " << heel_y_val << std::endl;
    std::cout << "Old heel z: " << heel_z_val << std::endl;
    std::cout << "Old toe x: " << toe_x_val << std::endl;
    std::cout << "Old toe y: " << toe_y_val << std::endl;
    std::cout << "Old toe z: " << toe_z_val << std::endl;

    std::cout << "New heel x: " << projection.first()(0) << std::endl;
    std::cout << "New heel y: " << projection.first()(1) << std::endl;
    std::cout << "New heel z: " << projection.first()(2) << std::endl;
    std::cout << "New toe x: " << projection.last()(0) << std::endl;
    std::cout << "New toe y: " << projection.last()(1) << std::endl;
    std::cout << "New toe z: " << projection.last()(2) << std::endl;
}

void WellSplineLength::initializeWell(QList<QPair<QUuid, QString> > vars)
{
    if (vars.length() > 6)
        throw std::runtime_error("More than 6 variables were found for a well. Something is wrong in the WellSplineLength constraint.");

    QList<QPair<QUuid, QString>> heel_vars = QList<QPair<QUuid, QString>>();
    QList<QPair<QUuid, QString>> toe_vars = QList<QPair<QUuid, QString>>();
    for (int i = 0; i < vars.length(); ++i) {
        std::cout << "Checking " << vars[i].second.toStdString() << std::endl;
        if (QString::compare(vars[i].second.split("#")[2], "heel") == 0)
            heel_vars.append(vars[i]);
        else if (QString::compare(vars[i].second.split("#")[2], "toe") == 0)
            toe_vars.append(vars[i]);
    }
    affected_well_.heel = initializeCoord(heel_vars);
    affected_well_.toe = initializeCoord(toe_vars);

    std::cout << "heel x: " << affected_well_.heel.x.toString().toStdString() << std::endl;
    std::cout << "heel y: " << affected_well_.heel.y.toString().toStdString() << std::endl;
    std::cout << "heel z: " << affected_well_.heel.z.toString().toStdString() << std::endl;
}

WellSplineLength::Coord WellSplineLength::initializeCoord(QList<QPair<QUuid, QString> > point_vars)
{
    if (point_vars.length() != 3)
        throw std::runtime_error(std::to_string(point_vars.length()) + " coordinate variables were found for the heel/toe. Need 3.");

    Coord coord;

    for (int i = 0; i < point_vars.length(); ++i) {
        if (QString::compare(point_vars[i].second.split("#").last(), "x") == 0)
            coord.x = point_vars[i].first;
        else if (QString::compare(point_vars[i].second.split("#").last(), "y") == 0)
            coord.y = point_vars[i].first;
        else if (QString::compare(point_vars[i].second.split("#").last(), "z") == 0)
            coord.z = point_vars[i].first;
        else throw std::runtime_error("One of variables passed to initializeCoord does not match the #heel/toe#x/y/z pattern.");
    }
    return coord;
}

}}
