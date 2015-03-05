/*
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einarjba@stud.ntnu.no>
 *
 * The code is largely based on ResOpt, created by  Aleksander O. Juell <aleksander.juell@ntnu.no>
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
 */

#include "modelperturbation.h"

ModelPerturbation::ModelPerturbation(int id, int var, double val)
{
    model_id = id;
    perturbation_variable_id = var;
    perturbation_value = val;
}

int ModelPerturbation::getPerturbation_variable() const
{
    return perturbation_variable_id;
}

void ModelPerturbation::setPerturbation_variable(const int &value)
{
    perturbation_variable_id = value;
}

double ModelPerturbation::getPerturbation_value() const
{
    return perturbation_value;
}

void ModelPerturbation::setPerturbation_value(double value)
{
    perturbation_value = value;
}

int ModelPerturbation::getModel_id() const
{
    return model_id;
}

void ModelPerturbation::setModel_id(int value)
{
    model_id = value;
}

std::string ModelPerturbation::toString()
{
    std::string mid = "Model id: " + boost::lexical_cast<std::string>(model_id);
    std::string pvid = "Perturbation variable id: " + boost::lexical_cast<std::string>(perturbation_variable_id);
    std::string pval = "Perturbation value: " + boost::lexical_cast<std::string>(perturbation_value);
    return mid + "\n" + pvid + "\n" + pval;
}
