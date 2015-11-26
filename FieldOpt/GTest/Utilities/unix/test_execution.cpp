/******************************************************************************
 *
 *
 *
 * Created: 23.11.2015 2015 by einar
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

#include "Utilities/unix/execution.h"
#include <QStringList>

namespace {

class UnixPipeTest : public ::testing::Test {
protected:
    UnixPipeTest() {
        directory_ = "/home/einar/Documents/GitHub/PCG/fieldopt_output";
        command_ = "";
        //commands_ = QStringList();
        //commands_.append("ls");
        //commands_.append("touch test.txt");
        //commands_.append("ls");
        //commands_.append("rm test.txt");
        //commands_.append("ls");
    }
    virtual ~UnixPipeTest() {}

    virtual void SetUp() {}
    virtual void TearDown() {}

    QString directory_;
    QString command_;
    QStringList commands_;
};

TEST_F(UnixPipeTest, SingleCommand) {
    ::Utilities::Unix::Exec(directory_, command_, true);
}

TEST_F(UnixPipeTest, MultipleCommands) {
    ::Utilities::Unix::Exec(directory_, commands_, true);
}

}
