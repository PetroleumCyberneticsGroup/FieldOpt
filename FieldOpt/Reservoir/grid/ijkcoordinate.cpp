/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#include "ijkcoordinate.h"

namespace Reservoir {
    namespace Grid {

        IJKCoordinate::IJKCoordinate(int i, int j, int k)
                : i_(i), j_(j), k_(k)
        {
        }

        bool IJKCoordinate::Equals(const IJKCoordinate *other) const
        {
            return this->i() == other->i() && this->j() == other->j() && this->k() == other->k();
        }

        IJKCoordinate *IJKCoordinate::Add(const IJKCoordinate *other) const
        {
            return new IJKCoordinate(
                    this->i() + other->i(),
                    this->j() + other->j(),
                    this->k() + other->k());
        }

    }
}
