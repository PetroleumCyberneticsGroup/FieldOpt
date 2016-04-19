/******************************************************************************
 *
 * perforation.h
 *
 * Created: 24.09.2015 2015 by einar
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

#ifndef PERFORATION_H
#define PERFORATION_H

#include "completion.h"
#include "Model/properties/continous_property.h"
#include "Model/properties/variable_property_handler.h"
#include "Model/properties/variable_property_container.h"

namespace Model {
namespace Wells {
namespace Wellbore {
namespace Completions {

/*!
 * \brief The Perforation class represents a well perforation. It contains the potentially variable transmissibility factor.
 */
class Perforation : public Completion
{
public:
    Perforation(::Utilities::Settings::Model::Completion completion_settings,
                Properties::VariablePropertyContainer *variable_container,
                Properties::VariablePropertyHandler *variable_handler);

    Perforation();

    double transmissibility_factor() const { return transmissibility_factor_->value(); }
    void setTransmissibility_factor(double value) { transmissibility_factor_->setValue(value); }

private:
    Properties::ContinousProperty *transmissibility_factor_;
};

}
}
}
}

#endif // PERFORATION_H
