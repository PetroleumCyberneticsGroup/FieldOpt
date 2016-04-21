#include "well_spline_constraint.h"

namespace Optimization { namespace Constraints {

WellSplineConstraint::Well WellSplineConstraint::initializeWell(QList<QPair<QUuid, QString> > vars, QString well_name)
{
    Well well;

    QList<QPair<QUuid, QString>> heel_vars = QList<QPair<QUuid, QString>>();
    QList<QPair<QUuid, QString>> toe_vars = QList<QPair<QUuid, QString>>();
    for (int i = 0; i < vars.length(); ++i) {
        if (QString::compare(vars[i].second.split("#")[1], well_name) == 0) {
            if (QString::compare(vars[i].second.split("#")[2], "heel") == 0)
                heel_vars.append(vars[i]);
            else if (QString::compare(vars[i].second.split("#")[2], "toe") == 0)
                toe_vars.append(vars[i]);
        }
    }
    if (heel_vars.length() + toe_vars.length() != 6)
        throw std::runtime_error("More/less than 6 variables were found for a well. Something is wrong in the with a WellSpline constraint.");
    well.heel = initializeCoord(heel_vars);
    well.toe = initializeCoord(toe_vars);
    return well;
}

WellSplineConstraint::Coord WellSplineConstraint::initializeCoord(QList<QPair<QUuid, QString> > point_vars)
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
