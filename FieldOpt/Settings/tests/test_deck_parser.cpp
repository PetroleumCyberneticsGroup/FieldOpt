/******************************************************************************
   Created by einar on 4/11/18.
   Copyright (C) 2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include <QString>

#include "Settings/tests/test_resource_example_file_paths.hpp"
#include "Settings/deck_parser.h"

using namespace Settings;
using namespace TestResources::ExampleFilePaths;

namespace {

class DeckParserTest : public ::testing::Test {
 protected:
  DeckParserTest() { }
  virtual ~DeckParserTest() {}

  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(DeckParserTest, Initialization) {
    DeckParser dp = DeckParser("../examples/ECLIPSE/norne-simplified/NORNE_SIMPLIFIED.DATA");
//    DeckParser dp = DeckParser("/home/einar/Documents/GitHub/PCG/Models/ECLIPSE/olympus_ensemble/OLYMPUS_1/OLYMPUS_1.DATA");
//    DeckParser dp = DeckParser(TestResources::ExampleFilePaths::deck_horzwel_.toStdString());
//    auto wells = dp.GetWellData();
//
//    for (int i = 0; i < dp.GetTimeDays().size(); ++i) {
//        std::cout << i << "\t" << dp.GetTimeDays()[i] << "\t" << dp.GetTimeDates()[i] << std::endl;
//    }
//
//    for (auto well : wells) {
//        std::cout << well.toString();
//        for (int i=0; i < 3 && i < well.controls.size(); ++i) {
//            std::cout << well.controls[i].toString() << std::endl;
//        }
//    }
}

TEST_F(DeckParserTest, NumberOfWells) {
    // 1: C-4H i // 2: B-2H // 3: D-2H
    // 4: B-4H   // 5: D-4H // 6: C-1H i // 7: E-3H
    // 8: C-2H i // 9: B-1H //10: C-3H i //11: F-1H i
    DeckParser dp = DeckParser("../examples/ECLIPSE/norne-simplified/NORNE_SIMPLIFIED.DATA");
    auto wells = dp.GetWellData();
    EXPECT_EQ(wells.size(), 11);

    int n_inje = 0;
    int n_prod = 0;

    for (auto well : wells) {
        if (well.type == Model::WellType::Producer) {
            n_prod++;
        }
        else {
            n_inje++;
        }
    }
    EXPECT_EQ(n_inje, 5);
    EXPECT_EQ(n_prod, 6);
}

TEST_F(DeckParserTest, NumberOfConnections) {
    DeckParser dp = DeckParser("../examples/ECLIPSE/norne-simplified/NORNE_SIMPLIFIED.DATA");
    auto wells = dp.GetWellData();
    std::vector<int> n_conns = { 7,  // C-4H
                                 8,  // B-2H
                                 9,  // D-2H
                                20,  // B-4H
                                26,  // D-4H
                                21,  // C-1H
                                21,  // E-3H
                                 3,  // C-2H
                                14,  // B-1H
                                21,  // C-3H
                                17   // F-1H
    };
    for (int i = 0; i < wells.size(); ++i) {
        EXPECT_EQ(n_conns[i], wells[i].well_blocks.size());
    }

}

}
