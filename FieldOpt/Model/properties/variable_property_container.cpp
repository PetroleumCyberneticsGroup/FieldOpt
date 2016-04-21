/******************************************************************************
 *
 * VariableContainer.cpp
 *
 * Created: 23.09.2015 2015 by einar
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

#include "variable_property_container.h"
#include <QStringList>

namespace Model {
namespace Properties {

VariablePropertyContainer::VariablePropertyContainer()
{
    binary_variables_ = new QHash<QUuid, BinaryProperty *>();
    discrete_variables_ = new QHash<QUuid, DiscreteProperty *>();
    continous_variables_ = new QHash<QUuid, ContinousProperty *>();
}

void VariablePropertyContainer::AddVariable(BinaryProperty *var)
{
    binary_variables_->insert(var->id(), var);
}

void VariablePropertyContainer::AddVariable(DiscreteProperty *var)
{
    discrete_variables_->insert(var->id(), var);
}

void VariablePropertyContainer::AddVariable(ContinousProperty *var)
{
    continous_variables_->insert(var->id(), var);
}

BinaryProperty *VariablePropertyContainer::GetBinaryVariable(QUuid id) const
{
    if (!binary_variables_->contains(id)) throw VariableIdDoesNotExistException("Binary variable not found.");
    return binary_variables_->value(id);
}

DiscreteProperty *VariablePropertyContainer::GetDiscreteVariable(QUuid id) const
{
    if (!discrete_variables_->contains(id)) throw VariableIdDoesNotExistException("Discrete variable not found.");
    return discrete_variables_->value(id);
}

ContinousProperty *VariablePropertyContainer::GetContinousVariable(QUuid id) const
{
    if (!continous_variables_->contains(id)) throw VariableIdDoesNotExistException("Continous variable not found.");
    return continous_variables_->value(id);
}

void VariablePropertyContainer::SetBinaryVariableValue(QUuid id, bool val)
{
    if (!binary_variables_->contains(id)) throw VariableIdDoesNotExistException("Binary variable not found.");
    else binary_variables_->value(id)->setValue(val);
}

void VariablePropertyContainer::SetDiscreteVariableValue(QUuid id, int val)
{
    if (!discrete_variables_->contains(id)) throw VariableIdDoesNotExistException("Integer variable not found.");
    else discrete_variables_->value(id)->setValue(val);
}

void VariablePropertyContainer::SetContinousVariableValue(QUuid id, double val)
{
    if (!continous_variables_->contains(id)) throw VariableIdDoesNotExistException("Continous variable not found.");
    else continous_variables_->value(id)->setValue(val);
}

QHash<QUuid, bool> VariablePropertyContainer::GetBinaryVariableValues() const
{
    QHash<QUuid, bool> binary_values = QHash<QUuid, bool>();
    foreach (QUuid key, binary_variables_->keys())
        binary_values[key] = binary_variables_->value(key)->value();
    return binary_values;
}

QHash<QUuid, int> VariablePropertyContainer::GetDiscreteVariableValues() const
{
    QHash<QUuid, int> discrete_values = QHash<QUuid, int>();
    foreach (QUuid key, discrete_variables_->keys()) {
        discrete_values[key] = discrete_variables_->value(key)->value();
    }
    return discrete_values;
}

QHash<QUuid, double> VariablePropertyContainer::GetContinousVariableValues() const
{
    QHash<QUuid, double> continous_values = QHash<QUuid, double>();
    foreach (QUuid key, continous_variables_->keys()) {
        continous_values[key] = continous_variables_->value(key)->value();
    }
    return continous_values;
}

QList<QUuid> VariablePropertyContainer::GetBinaryVariableIdsWithName(QString var_name) const
{
    QString varn_wo_suffixes = var_name.split("#").first();
    QList<QUuid> ids_for_variables_with_name = QList<QUuid>();
    foreach (Properties::BinaryProperty * prop, binary_variables_->values()) {
        if (QString::compare(prop->name().split("#").first(), varn_wo_suffixes) == 0)
            ids_for_variables_with_name.append(prop->id());
    }
    return ids_for_variables_with_name;
}

QList<QUuid> VariablePropertyContainer::GetDiscreteVariableIdsWithName(QString var_name) const
{
    QString varn_wo_suffixes = var_name.split("#").first();
    QList<QUuid> ids_for_variables_with_name = QList<QUuid>();
    foreach (Properties::DiscreteProperty *prop, discrete_variables_->values()) {
        if (QString::compare(prop->name().split("#").first(), varn_wo_suffixes) == 0)
            ids_for_variables_with_name.append(prop->id());
    }
    return ids_for_variables_with_name;
}

QList<QUuid> VariablePropertyContainer::GetContinousVariableIdsWithName(QString var_name) const
{
    QString varn_wo_suffixes = var_name.split("#").first();
    QList<QUuid> ids_for_variables_with_name = QList<QUuid>();
    foreach (Properties::ContinousProperty *prop, continous_variables_->values()) {
        if (QString::compare(prop->name().split("#").first(), varn_wo_suffixes) == 0)
            ids_for_variables_with_name.append(prop->id());
    }
    return ids_for_variables_with_name;
}

QList<QPair<QUuid, QString> > VariablePropertyContainer::GetContinousVariableNamesAndIdsMatchingSubstring(QString substring)
{
    QList<QPair<QUuid, QString> > mathcing_vars = QList<QPair<QUuid, QString> >();
    foreach (Properties::ContinousProperty *prop, continous_variables_->values()) {
        if (prop->name().contains(substring)) {
            QPair<QUuid, QString> entry = QPair<QUuid, QString>();
            entry.first = prop->id();
            entry.second = prop->name();
            mathcing_vars.append(entry);
        }
    }
    return mathcing_vars;
}

void VariablePropertyContainer::DeleteBinaryVariable(QUuid id)
{
    if (!binary_variables_->contains(id)) throw VariableIdDoesNotExistException("Binary variable not found. Unable to delete.");
    binary_variables_->remove(id);
}

void VariablePropertyContainer::DeleteDiscreteVariable(QUuid id)
{
    if (!discrete_variables_->contains(id)) throw VariableIdDoesNotExistException("Integer variable not found. Unable to delete.");
    discrete_variables_->remove(id);
}

void VariablePropertyContainer::DeleteContinousVariable(QUuid id)
{
    if (!continous_variables_->contains(id)) throw VariableIdDoesNotExistException("Real variable not found. Unable to delete");
    continous_variables_->remove(id);
}

void VariablePropertyContainer::CheckVariableNameUniqueness()
{
    QList<QString> names = QList<QString>();
    foreach (auto var, discrete_variables_->values()) {
        if (var->name().size() > 0) {
            if (names.contains(var->name()))
                throw std::runtime_error("Encountered non-unique variable name: " + var->name().toStdString());
            else
                names.append(var->name());
        }
    }

    foreach (auto var, continous_variables_->values()) {
        if (var->name().size() > 0) {
            if (names.contains(var->name()))
                throw std::runtime_error("Encountered non-unique variable name: " + var->name().toStdString());
            else
                names.append(var->name());
        }
    }

    foreach (auto var, binary_variables_->values()) {
        if (var->name().size() > 0) {
            if (names.contains(var->name()))
                throw std::runtime_error("Encountered non-unique variable name: " + var->name().toStdString());
            else
                names.append(var->name());
        }
    }
}


}
}
