/******************************************************************************
 *
 * perforation.cpp
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

#include "perforation.h"

namespace Model {
namespace Wells {
namespace Wellbore {
namespace Completions {

Perforation::Perforation(Utilities::Settings::Model::Completion completion_settings,
                         Variables::VariableContainer *variable_container,
                         Variables::VariableHandler *variable_handler)
    : Completion(completion_settings)
{
    transmissibility_factor_ = new Variables::RealVariable(completion_settings.transmissibility_factor);
    if (variable_handler->GetPerforation(completion_settings.id)->transmissibility_factor())
        variable_container->AddVariable(transmissibility_factor_);
}






}
}
}
}
