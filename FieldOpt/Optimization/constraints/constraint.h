/******************************************************************************
 *
 *
 *
 * Created: 01.12.2015 2015 by einar
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

#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "Optimization/case.h"
#include "Utilities/settings/optimizer.h"
#include "Model/properties/variable_property_container.h"

namespace Optimization { namespace Constraints {

/*!
 * \brief The Constraint class is the abstract parent class to all other constraint classes. One Constraint
 * object should be created for each defined constraint.
 */
class Constraint
{
public:
    /*!
     * \brief CaseSatisfiesConstraint checks whether a case satisfies the constraints for all
     * applicable variables.
     * \param c The case to be checked.
     * \return True if the constraint is satisfied; otherwise false.
     */
    virtual bool CaseSatisfiesConstraint(Case *c) = 0;

    /*!
     * \brief SnapCaseToConstraints Snaps all variable values in the case to the closest value
     * that satisfies the constraint.
     * \param c The case that should have it's variable values snapped.
     */
    virtual void SnapCaseToConstraints(Case *c) = 0;

    QString name() const { return name_; }

protected:
    Constraint(::Utilities::Settings::Optimizer::Constraint settings, ::Model::Properties::VariablePropertyContainer *variables); //!< This class' constructor should not be used directly. The constructors of subclasses should be used.
    QList<QUuid> affected_binary_variables_;
    QList<QUuid> affected_integer_variables_;
    QList<QUuid> affected_real_variables_;
    QString name_;
};

}}

#endif // CONSTRAINT_H
