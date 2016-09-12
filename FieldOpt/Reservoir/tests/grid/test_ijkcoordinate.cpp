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

#include <gtest/gtest.h>
#include "Reservoir/grid/ijkcoordinate.h"

using namespace Reservoir::Grid;

namespace {

    class IJKCoordinateTest : public ::testing::Test {
    protected:
        IJKCoordinateTest () { }
        virtual ~IJKCoordinateTest () { }
    };

    TEST_F(IJKCoordinateTest , Equality) {
        IJKCoordinate *ijk_1 = new IJKCoordinate(1,2,3);
        IJKCoordinate *ijk_2 = new IJKCoordinate(1,2,3);
        IJKCoordinate *ijk_3 = new IJKCoordinate(2,2,3);
        EXPECT_TRUE(ijk_1->Equals(ijk_2));
        EXPECT_FALSE(ijk_1->Equals(ijk_3));
    }

    TEST_F(IJKCoordinateTest, Addition) {
        IJKCoordinate *ijk_1 = new IJKCoordinate(1,1,1);
        IJKCoordinate *ijk_2 = new IJKCoordinate(3,4,5);
        IJKCoordinate *ijk_3 = new IJKCoordinate(4,5,6);
        IJKCoordinate *ijk_sum = ijk_1->Add(ijk_2);
        EXPECT_TRUE(ijk_sum->Equals(ijk_3));
    }

}

