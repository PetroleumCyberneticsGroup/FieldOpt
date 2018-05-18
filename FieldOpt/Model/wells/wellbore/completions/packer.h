/******************************************************************************
   Copyright (C) 2015-2018 Einar J.M. Baumann <einar.baumann@gmail.com>

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
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef FIELDOPT_PACKER_H
#define FIELDOPT_PACKER_H

#include "segmented_completion.h"
namespace Model {
namespace Wells {
namespace Wellbore {
namespace Completions {

class Packer : public SegmentedCompletion {
 public:
  Packer(const Settings::Model::Well::Completion &completion_settings,
         Properties::VariablePropertyContainer *variable_container);


};

}
}
}
}

#endif //FIELDOPT_PACKER_H
