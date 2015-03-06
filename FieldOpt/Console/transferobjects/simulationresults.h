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

#ifndef SIMULATIONRESULTS_H
#define SIMULATIONRESULTS_H

#include <boost/serialization/access.hpp>
#include <boost/lexical_cast.hpp>

/*!
 * \brief The SimulationResults class contains the results of running a simuluation on a perturbed model.
 *
 * The information should be sufficient to determine whether the perturbation was better/worse than other
 * perturbations.
 *
 * boost::serialization is used to serialize the object when transfering it with MPI.
 * This is implemented by making this class a friend of the boost::serialization::access class
 * and defining in the serialize() function which fields should be included.
 */
class SimulationResults
{
private:
    int model_id;      //!< A unique id for the model perturbation.
    float model_fopt;  //!< The simulated FOPT (Field Oil Production Total) for the perturbed model.

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & model_id;
        ar & model_fopt;
    }

public:
    SimulationResults(){}  //!< Default constructor. Used when defining the object before receiving into it using MPI.
    /*!
     * \brief Constructor setting the unique perturbed model id and FOPT. Used when defining a perturbation befor sending it using MPI.
     * \param id The model_id to be set.
     * \param fopt The FOPT to be set.
     */
    SimulationResults(int id, float fopt);

    int getModel_id() const;      //!< Get the unique perturbed model id.
    void setModel_id(int value);  //!< Set the unique perturbed model id.

    float getModel_fopt() const;      //!< Get the simulated FOPT.
    void setModel_fopt(float value);  //!< Set the simulated FOPT.

    std::string toString() const;  //!< Get a string description of the current state of this object. Primarily inteded for debugging purposes.
};

#endif // SIMULATIONRESULTS_H
