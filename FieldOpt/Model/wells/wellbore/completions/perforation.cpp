/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
   along with FieldOpt. If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "perforation.h"

namespace Model {
namespace Wells {
namespace Wellbore {
namespace Completions {

Perforation::Perforation(Settings::Model::Well::Completion completion_settings,
                         Properties::VariablePropertyContainer *variable_container)
    : Completion(completion_settings)
{
    transmissibility_factor_ =
        new Properties::ContinousProperty(completion_settings.transmissibility_factor);
    if (completion_settings.is_variable) {
        transmissibility_factor_->setName(completion_settings.name);
        variable_container->AddVariable(transmissibility_factor_);
    }
}

Perforation::Perforation()
    : Completion(CompletionType::Perforation) {
    transmissibility_factor_ = new Properties::ContinousProperty(0.0);
}
}
}
}
}
