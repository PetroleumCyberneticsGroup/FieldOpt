/******************************************************************************
 *
 *
 *
 * Created: 14.12.2015 2015 by einar
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

#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <iostream>

#include "runners/main_runner.h"

int main(int argc, char *argv[])
{
    try {
        int max_par_sims;
        po::options_description desc("FieldOpt options");
        desc.add_options()
                ("help,h", "print help message")
                ("verbose,v", po::value<int>()->implicit_value(0),
                 "show verbose console output while optimizing")
                ("force,f", po::value<int>()->implicit_value(0),
                 "overwrite existing output files")
                ("max-parallel-simulations,m", po::value<int>(&max_par_sims)->default_value(0),
                 "start max <arg> parallel simulations")
                ("runner-type,r", po::value<std::string>(),
                 "type of runner (serial/oneoff)")
                ("grid-path,g", po::value<std::string>(),
                 "path to model grid file (e.g. *.GRID)")
                ("sim-drv-path,s", po::value<std::string>(),
                 "path to simulator driver file (e.g. *.DATA)")
                ("simulation-timeout,t", po::value<int>()->implicit_value(0),
                 "Simulations will be terminated after running for t*(lowest_recorded_time)")
                ("well-prod-points,p", po::value<std::vector<double>>()->multitoken(),
                 "Production well position coordinates")
                ("well-inj-points,i", po::value<std::vector<double>>()->multitoken(),
                 "Injection well position coordinates")
                ("input-file", po::value<std::string>(),
                 "path to FieldOpt driver file")
                ("output-dir", po::value<std::string>(),
                 "path to folder in which to store the results.")
                ;

        // Positional arguments
        po::positional_options_description p;
        p.add("input-file", 1);
        p.add("output-dir", 2);

        // Process arguments to variable map
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);

        // If called with --help or -h flag:
        if (vm.count("help") || !vm.count("input-file") || !vm.count("output-dir")) { // Print help if --help present or input file/output dir not present
            std::cout << "Usage: ./FieldOpt input-file output-dir [options]" << std::endl;
            std::cout << desc << std::endl;
            return 0;
        }

        // Parse runtime settings
        auto *runtime_settings = new Runner::RuntimeSettings(vm);

        // Initialize runner
        auto runner = Runner::MainRunner(runtime_settings);
        runner.Execute();
    }
    catch (std::exception &e) {
        std::cout << "error: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cout << "Exception of unknown type." << std::endl;

    }
    return 0;
}

