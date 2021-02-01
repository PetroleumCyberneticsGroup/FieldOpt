/******************************************************************************
   Copyright (C) 2015-2018 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "property.h"
#include <QStringList>

namespace Model {
namespace Properties {

void Property::SetVariable() {
    if (name_.length() == 1)
        throw std::runtime_error("Cannot set an unnamed property to variable.");

    is_variable_ = true;
    set_property_info();
}

Property::PropertyInfo Property::propertyInfo() const {
    if (info_.is_set_)
        return info_;
    else throw std::runtime_error("Cannot get information about a property that is not variable.");
}

void Property::set_property_info() {
    info_.prop_type = get_prop_type_name(name_);
    info_.parent_well_name = get_well_name(name_);

    if (info_.prop_type == PropertyType::SplinePoint) {
        info_.spline_end = get_spline_end(name_);
        info_.index = get_spline_index(name_);
    }

    if (info_.prop_type == WellBlock
        || info_.prop_type == Transmissibility
        || info_.prop_type == Packer
        || info_.prop_type == BHP
        || info_.prop_type == Rate)
        info_.index = get_prop_index(name_);

    if (info_.prop_type == PolarSpline) {
        info_.polar_prop = get_polar_prop(name_);
        if (info_.polar_prop == Midpoint){
            info_.coord = get_prop_coord(name_);
        }
    }

    if (info_.prop_type == AWPSpline) {
        info_.awp_prop = get_AWP_prop(name_);
    }
    if (info_.prop_type == SplinePoint || info_.prop_type == WellBlock || info_.prop_type == PseudoContVert)
        info_.coord = get_prop_coord(name_);

    info_.is_set_ = true;
}


Property::SplineEnd Model::Properties::Property::get_spline_end(const QString prop_name) const {
    if (prop_name.split("#").length() != 4)
        throw std::runtime_error("Invalid SplinePoint format " + prop_name.toStdString()
                                     + ", unable to extract heel/toe info.");
    QString endstr = prop_name.split("#")[2];
    if (QString::compare("heel", endstr) == 0)
        return SplineEnd::Heel;
    else if (QString::compare("toe", endstr) == 0)
        return SplineEnd::Toe;
    else if (QString::compare("P", endstr.at(0)) == 0)
        return SplineEnd::Middle;
    else throw std::runtime_error("Invalid SplinePoint name format " + prop_name.toStdString()
                                      + ", unable to extract heel/toe info.");
}

Property::PropertyType Property::get_prop_type_name(const QString prop_name) const {
    QString propstr = prop_name.split("#")[0];
    if (QString::compare("BHP", propstr) == 0)
        return BHP;
    else if (QString::compare("Rate", propstr) == 0)
        return Rate;
    else if (QString::compare("SplinePoint", propstr) == 0)
        return SplinePoint;
    else if (QString::compare("WellBlock", propstr) == 0)
        return WellBlock;
    else if (QString::compare("Transmissibility", propstr) == 0)
        return Transmissibility;
    else if (QString::compare("PseudoContVert", propstr) == 0)
        return PseudoContVert;
    else if (QString::compare("Packer", propstr) == 0)
        return Packer;
    else if (QString::compare("ICD", propstr) == 0)
        return ICD;
    else if (QString::compare("PolarSpline", propstr) == 0)
        return PolarSpline;
    else if (QString::compare("AWPSpline", propstr) == 0)
        return AWPSpline;
    else throw std::runtime_error("Unable to recognize property type " + propstr.toStdString());
}

QString Property::get_well_name(const QString prop_name) const {
    return prop_name.split("#")[1];
}

int Property::get_prop_index(const QString prop_name) const {
    int index;
    bool cast_ok;
    index = prop_name.split("#")[2].toInt(&cast_ok);
    if (cast_ok)
        return index;
    else throw std::runtime_error("Unable to extract index from property name " + prop_name.toStdString());
}
Property::PolarProp Property::get_polar_prop(const QString prop_name) const {
    QString propstr = prop_name.split("#")[2];
    if (QString::compare(propstr, "Azimuth") == 0)
        return Azimuth;
    else if (QString::compare(propstr, "Elevation") == 0)
        return Elevation;
    else if (QString::compare(propstr, "Length") == 0)
        return Length;
    else if (QString::compare(propstr, "Midpoint") == 0)
        return Midpoint;
    else throw std::runtime_error("Unable to extract PolarSpline data from property name.");
}

Property::AWPProp Property::get_AWP_prop(const QString prop_name) const {
    QString propstr = prop_name.split("#")[2];
    if (QString::compare(propstr, "heel_x") == 0)
        return xh;
    else if (QString::compare(propstr, "heel_y") == 0)
        return yh;
    else if (QString::compare(propstr, "toe_x") == 0)
        return xt;
    else if (QString::compare(propstr, "toe_y") == 0)
        return yt;
    else throw std::runtime_error("Unable to extract AWPSpline data from property name.");
}
Property::Coordinate Property::get_prop_coord(const QString prop_name) const {
    QString cstr = prop_name.split("#").last();
    if (QString::compare("x", cstr) == 0) return x;
    else if (QString::compare("y", cstr) == 0) return y;
    else if (QString::compare("z", cstr) == 0) return z;
    else if (QString::compare("i", cstr) == 0) return i;
    else if (QString::compare("j", cstr) == 0) return j;
    else if (QString::compare("k", cstr) == 0) return k;
    else throw std::runtime_error("Unable to extract coordinate from property name " + prop_name.toStdString());
}

void Property::UpdateId(QUuid new_id) {
    id_ = new_id;
}
int Property::get_spline_index(const QString prop_name) const {
    QString endstr = prop_name.split("#")[2];
    if (QString::compare("heel", endstr) == 0)
        return 0;
    else if (QString::compare("toe", endstr) == 0)
        return -1;
    else if (QString::compare("P", endstr.at(0)) == 0) {
        endstr.remove(0, 1); // Remove the P
        bool cast_ok;
        int index = endstr.toInt(&cast_ok);
        if (cast_ok) {
            return index;
        }
        else {
            throw std::runtime_error("Unable to extract index from property name " + prop_name.toStdString());
        }
    }
}

}
}

