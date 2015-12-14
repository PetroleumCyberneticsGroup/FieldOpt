/******************************************************************************
 *
 *
 *
 * Created: 03.12.2015 2015 by einar
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

#ifndef BOXCONSTRAINT_H
#define BOXCONSTRAINT_H

#include "constraint.h"
#include <QPair>

namespace Optimization { namespace Constraints {

/*!
 * \brief The BoxConstraint class Represents a "box" constraint; i.e. a constraint where minimum
 * and maximum values are defined for variables.
 *
 * Box constraints may only be applied to real- and integer variables.
 */
class BoxConstraint : public Constraint
{
public:
    BoxConstraint(::Utilities::Settings::Optimizer::Constraint settings, ::Model::Properties::VariablePropertyContainer *variables); //!< This class' constructor should not be used directly. The constructors of subclasses should be used.

    // Constraint interface
public:
    bool CaseSatisfiesConstraint(Case *c);
    void SnapCaseToConstraints(Case *c);

private:
    QHash<QUuid, QPair<double, double> > integer_constraints_; //!< Constraints for integer variables. The pair contains the min- and max- values for each variable. The first element is min, the second is max.
    QHash<QUuid, QPair<double, double> > real_constraints_; //!< Constraints for real variables. The pair contains the min- and max- values for each variable. The first element is min, the second is max.

    void initializeBhpConstraints(double min, double max);
};

}}

#endif // BOXCONSTRAINT_H
