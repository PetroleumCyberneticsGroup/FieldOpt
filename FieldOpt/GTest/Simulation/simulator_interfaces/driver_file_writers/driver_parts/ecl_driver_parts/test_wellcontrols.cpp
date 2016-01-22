/******************************************************************************
 *
 *
 *
 * Created: 13.11.2015 2015 by einar
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

#include "GTest/Model/test_fixture_model_base.h"
#include "Simulation/simulator_interfaces/driver_file_writers/driver_parts/ecl_driver_parts/wellcontrols.h"
#include <iostream>

using namespace ::Simulation::SimulatorInterfaces::DriverFileWriters::DriverParts::ECLDriverParts;

namespace {

class DriverPartWellControlsTest : public ModelBaseTest {
protected:
    DriverPartWellControlsTest(){
        well_controls_ = new WellControls(model_->wells());
    }
    virtual ~DriverPartWellControlsTest(){}

    WellControls *well_controls_;
};

TEST_F(DriverPartWellControlsTest, Constructor) {
    //std::cout << well_controls_->GetPartString().toStdString() << std::endl;
}

}
