/******************************************************************************
   Copyright (C) 2016-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#include "well_spline_constraint.h"
#include <boost/lexical_cast.hpp>

namespace Optimization {
namespace Constraints {

using namespace Model::Properties;

WellSplineConstraint::Well WellSplineConstraint::initializeWell(QList<Model::Properties::ContinousProperty *> vars) {
    Well well;
    if (vars.length() >= 6 && (vars.length() % 3) == 0) {
        std::cout << "Using heel-toe parameterization for well spline constraint." << std::endl;
        for (auto var : vars) {
            if (var->propertyInfo().spline_end == Property::SplineEnd::Heel) {
                if (var->propertyInfo().coord == Property::Coordinate::x)
                    well.heel.x = var->id();
                else if (var->propertyInfo().coord == Property::Coordinate::y)
                    well.heel.y = var->id();
                else if (var->propertyInfo().coord == Property::Coordinate::z)
                    well.heel.z = var->id();
                else throw std::runtime_error("Unable to parse variable " + var->name().toStdString());
            } else if (var->propertyInfo().spline_end == Property::SplineEnd::Toe) {
                if (var->propertyInfo().coord == Property::Coordinate::x)
                    well.toe.x = var->id();
                else if (var->propertyInfo().coord == Property::Coordinate::y)
                    well.toe.y = var->id();
                else if (var->propertyInfo().coord == Property::Coordinate::z)
                    well.toe.z = var->id();
                else throw std::runtime_error("Unable to parse variable " + var->name().toStdString());
            } else throw std::runtime_error("Unable to parse variable " + var->name().toStdString());
        }
        if (vars.length() > 6) { // Adding additional points
            std::cout << "Using multi-point parameterization for well spline constraint." << std::endl;
            std::map<int, Coord> addtl_points;
            for (auto var : vars) {
                // use hash/map when creating the ponts (it will autosort them alphabeticaly, which is what we want)
                if (var->propertyInfo().spline_end == Property::SplineEnd::Middle) {
                    if (var->propertyInfo().coord == Property::Coordinate::x)
                        addtl_points[var->propertyInfo().index].x = var->id();
                    else if (var->propertyInfo().coord == Property::Coordinate::y)
                        addtl_points[var->propertyInfo().index].y = var->id();
                    else if (var->propertyInfo().coord == Property::Coordinate::z)
                        addtl_points[var->propertyInfo().index].z = var->id();
                }
            }
            for (int i = 0; i < addtl_points.size(); ++i) {
                well.additional_points.push_back(addtl_points[i+1]);
            }

        }
    }
    else {
        throw std::runtime_error("Incorrect number of variables (" + boost::lexical_cast<std::string>(vars.length())
                                     + ")passed to the initialize well method.");
    }
    return well;
}

QPair<Eigen::Vector3d, Eigen::Vector3d> WellSplineConstraint::GetEndpointValueVectors(Case *c, Well well) {
    double hx = c->real_variables()[well.heel.x];
    double hy = c->real_variables()[well.heel.y];
    double hz = c->real_variables()[well.heel.z];
    double tx = c->real_variables()[well.toe.x];
    double ty = c->real_variables()[well.toe.y];
    double tz = c->real_variables()[well.toe.z];
    Eigen::Vector3d heel(hx, hy, hz);
    Eigen::Vector3d toe(tx, ty, tz);
    return qMakePair(heel, toe);
}
std::vector<Eigen::Vector3d> WellSplineConstraint::GetPointValueVectors(Case *c, WellSplineConstraint::Well well) {
    std::vector<Eigen::Vector3d> points;
    auto endpoints = GetEndpointValueVectors(c, well);
    points.push_back(endpoints.first);

    for (auto p : well.additional_points) {
        double x = c->real_variables()[p.x];
        double y = c->real_variables()[p.y];
        double z = c->real_variables()[p.z];
        Eigen::Vector3d ep = Eigen::Vector3d(x, y, z);
        points.push_back(ep);
    }

    points.push_back(endpoints.second);
    return points;
}
double WellSplineConstraint::GetWellLength(Case *c, WellSplineConstraint::Well well) {
    double length = 0.0;
    auto points = GetPointValueVectors(c, well);
    for (int i = 0; i < points.size() - 1; ++i) {
        length += (points[i+1] - points[i]).norm();
    }
    return length;
}

}
}
