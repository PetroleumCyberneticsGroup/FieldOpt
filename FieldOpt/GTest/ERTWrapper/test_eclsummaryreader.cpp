/******************************************************************************
 *
 * test_eclsummaryreader.cpp
 *
 * Created: 07.10.2015 2015 by einar
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

#include <gtest/gtest.h>
#include "ERTWrapper/eclsummaryreader.h"

using namespace ERTWrapper::ECLSummary;

namespace {

class ECLSummaryReaderTest : public ::testing::Test {
protected:
    ECLSummaryReaderTest() {

    }

    virtual ~ECLSummaryReaderTest() {

    }

    virtual void SetUp() {
        ecl_summary_reader_ = new ECLSummaryReader(file_name_);
    }

    virtual void TearDown() {
        delete ecl_summary_reader_;
    }

    ECLSummaryReader *ecl_summary_reader_;
    QString file_name_ = "../../examples/ECLIPSE/HORZWELL/HORZWELL";
};

TEST_F(ECLSummaryReaderTest, ReportSteps) {
    //EXPECT_EQ(0, ecl_summary_reader_->GetFirstReportStep());
}

}
