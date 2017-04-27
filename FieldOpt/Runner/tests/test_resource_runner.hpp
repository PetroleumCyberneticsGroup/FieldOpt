/******************************************************************************
   Created by einar on 4/4/17.
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
#ifndef FIELDOPT_TEST_RESOURCE_RUNNER_HPP_H
#define FIELDOPT_TEST_RESOURCE_RUNNER_HPP_H

#include "../runtime_settings.h"
#include "../logger.h"
#include "../../Settings/tests/test_resource_example_file_paths.hpp"
namespace TestResources {

class RunnerResources {
 private:
  const int argc = 16;

  const char *argv[16] = {"FieldOpt",
                          "../examples/ADGPRS/5spot/fo_driver_5vert_wells.json",
                          "../fieldopt-output",
                          "-g", "../examples/Flow/5spot/5SPOT.EGRID",
                          "-s", "../examples/Flow/5spot/5SPOT.DATA",
                          "-b", ".",
                          "-r", "mpisync",
                          "-f",
                          "-v", "0",
                          "-t", "1000"
  };

 protected:
  Runner::RuntimeSettings *rts_ =
      new Runner::RuntimeSettings(argc, argv);
  Logger *logger_ =
      new Logger(rts_, TestResources::ExampleFilePaths::directory_output_, false);
};

}

#endif //FIELDOPT_TEST_RESOURCE_RUNNER_HPP_H
