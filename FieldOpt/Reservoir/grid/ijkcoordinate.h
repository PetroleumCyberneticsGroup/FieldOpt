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

#ifndef IJKCOORDINATE_H
#define IJKCOORDINATE_H

namespace Reservoir {
    namespace Grid {

        /*!
         * \brief The IJKCoordinate class represents an integer-based (I,J,K) coordinate or index.
         * IJK coordinate objects are immutable.
         */
        class IJKCoordinate
        {
        public:
            IJKCoordinate(){}
            IJKCoordinate(int i, int j, int k);


            int i() const { return i_; }
            int j() const { return j_; }
            int k() const { return k_;}

            bool Equals(const IJKCoordinate *other) const;

            /*!
             * \brief Add Returns a new coordinate which is the sum of this coordinate and another coordinate.
             */
            IJKCoordinate *Add(const IJKCoordinate *other) const;

        private:
            int i_;
            int j_;
            int k_;
        };

    }
}

#endif // IJKCOORDINATE_H
