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

#ifndef MODELPERTURBATION_H
#define MODELPERTURBATION_H

#include <QString>
#include <iostream>

/*!
 * \brief The ModelPerturbation class contains the information necessary to perturb a model.
 *
 * The class is used to transfer information from the root-process to the child processes.
 * Each child-process reads and creates the Model object by itself, and uses the ModelPerturbation
 * object it receives to alter the model.
 */
class ModelPerturbation
{
private:
    int model_id;                  //!< A unique id for the model perturbation.
    int perturbation_variable_id;  //!< An id for the variable to be perturbed.
    double perturbation_value;     //!< The amount with which to perturb the model.

public:
    ModelPerturbation(){}  //!< Default constructor. Used when defining the object before receiving into it using MPI.
    /*!
     * \brief Constructor setting id, var and val fields. Used when defining a perturbation befor sending it using MPI.
     * \param id The unique perturbed model id to be set.
     * \param var The id of the variable to be perturbed.
     * \param val The amount with which to perturb the value.
     */
    ModelPerturbation(int id, int var, double val);

    int getModel_id() const;      //!< Get the unique perturbed model id.
    void setModel_id(int value);  //!< Set the unique perturbed model id.

    int getPerturbation_variable() const;             //!< Get the id of the variable to be perturbed.
    void setPerturbation_variable(const int &value);  //!< Set the id of the variable to be perturbed.

    double getPerturbation_value() const;      //!< Get the value with which to perturb the model.
    void setPerturbation_value(double value);  //!< Set the value with which to perturb the model.

    QString toString() const;  //!< Get a string description of the current state of this object. Primarily inteded for debugging purposes.
};

#endif // MODELPERTURBATION_H
