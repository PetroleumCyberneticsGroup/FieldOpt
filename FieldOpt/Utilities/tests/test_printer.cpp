/******************************************************************************
   Copyright (C) 2018 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "Utilities/printer.hpp"

using namespace std;

namespace {

class PrinterTest : public testing::Test {
 public:
  const std::string short_string = "This is a short string.";
  const std::string long_string = "This is a long string. It should be longer than "
      "one line in a text box, but not too long.";
  const std::string vlong_string = "This is a long string. It should be longer than "
      "one line in a text box, but not too long. However, it should be so "
      "long that it spans multiple lines without it looking unnecessary.";
};


TEST_F(PrinterTest, Truncate) {
    std::string truncated = long_string;
    std::string not_truncated = short_string;
    Printer::truncate_text(truncated);
    Printer::truncate_text(not_truncated);
    std::cout << "Truncated: " << truncated << std::endl;
    std::cout << "Not truncated: " << not_truncated << std::endl;
}

TEST_F(PrinterTest, Pad) {
    std::string padded = short_string;
    std::string not_padded = long_string;
    Printer::pad_text(padded);
    Printer::pad_text(not_padded);
    std::cout << "Padded: |" << padded << "|" << std::endl;
    std::cout << "Not Padded: |" << not_padded << "|" << std::endl;
}

TEST_F(PrinterTest, Split) {
    std::vector<std::string> not_split = Printer::split_line(short_string);
    std::vector<std::string> split = Printer::split_line(vlong_string);
    std::cout << "Not split: |" << not_split[0] << "|" << std::endl;
    std::cout << "Split: \n\n";
    for (std::string line : split) {
        std::cout << line << std::endl;
    }
    std::cout << std::endl;
}
TEST_F(PrinterTest, PrintShortInfo) {
    Printer::info(short_string);
    Printer::info(long_string);
}
TEST_F(PrinterTest, PrintLongInfo) {
    std::cout << "\n\n";
    Printer::ext_info(short_string);
    Printer::ext_info(long_string, "Simulator", "ADGPRSDriverFileWriter");
}
TEST_F(PrinterTest, PrintLongWarning) {
    std::cout << "\n\n";
    Printer::ext_warn(short_string);
    Printer::ext_warn(long_string, "Simulator", "ADGPRSDriverFileWriter");
}
TEST_F(PrinterTest, PrintError) {
    Printer::error(short_string);
    Printer::error(long_string);
}
}
