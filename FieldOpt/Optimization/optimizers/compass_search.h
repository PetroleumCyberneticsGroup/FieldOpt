/******************************************************************************
 *
 *
 *
 * Created: 04.12.2015 2015 by einar
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

#ifndef COMPASSSEARCH_H
#define COMPASSSEARCH_H

#include "Optimization/optimizer.h"

namespace Optimization { namespace Optimizers {

/*!
 * \brief The CompassSearch class is an implementation of the Compass Search optimization algorithm
 * described by Torczon, Lewis and Kolda.
 *
 * This algorithm only supports integer and real variables.
 *
 * Reference:
 *
 * Kolda, Tamara G., Robert Michael Lewis, and Virginia Torczon.
 *  "Optimization by direct search: New perspectives on some classical and modern methods."
 *  SIAM review 45.3 (2003): 385-482.
 */
class CompassSearch : public Optimizer
{
public:
    CompassSearch(::Utilities::Settings::Optimizer *settings, Case *base_case, ::Model::Properties::VariablePropertyContainer *variables);
    double step_length() const { return step_length_; }

private:
    double step_length_; //!< The size of the perturbation for each variable.
    double minimum_step_length_; //!< Smallest allowed step length for the optimizer. _This is a termination condition_.

    void step(); //!< Move to a new tentative best case found in the list of recently evaluated cases.
    void contract(); //!< Contract the pattern, i.e. reduce the step length.
    void perturb(); //!< Apply the pattern to generate new cases.

    // Optimizer interface
public:
    /*!
     * \brief IsFinished Check if the optimization is finished.
     *
     * This algorithm has two termination conditions: max number of objective function evaluations and
     * minimum step length.
     * \return True if the algorithm has finished, otherwise false.
     */
    bool IsFinished();

    QString GetStatusStringHeader() const;
    QString GetStatusString() const;

private:
    void iterate(); //!< Step or contract, perturb, and clear list of recently evaluated cases.
};

}}

#endif // COMPASSSEARCH_H
