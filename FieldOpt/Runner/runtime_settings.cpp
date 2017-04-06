/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "runtime_settings.h"
#include <boost/lexical_cast.hpp>

namespace Runner {

RuntimeSettings::RuntimeSettings(int argc, const char *argv[])
{
    auto vm = createVariablesMap(argc, argv);

    if (vm.count("input-file")) {
        driver_file_ = QString::fromStdString(vm["input-file"].as<std::string>());
        if (!Utilities::FileHandling::FileExists(driver_file_))
            throw std::runtime_error("The specified driver file does not exist: " + driver_file_.toStdString());
    } else throw std::runtime_error("An input file must be specified.");

    if (vm.count("output-dir")) {
        output_dir_ = QString::fromStdString(vm["output-dir"].as<std::string>());
        if (!Utilities::FileHandling::DirectoryExists(output_dir_))
            throw std::runtime_error("The specified output directory does not exist: " + output_dir_.toStdString());
    } else throw std::runtime_error("An output directory must be specified.");

    if (vm.count("verbose")) verbosity_level_ = vm["verbose"].as<int>();
    else verbosity_level_ = 0;

    overwrite_existing_ = vm.count("force") != 0;
    if (!overwrite_existing_ && !Utilities::FileHandling::DirectoryIsEmpty(output_dir_))
        throw std::runtime_error("Output directory is not empty. Use the --force flag to overwrite existing content in: " + output_dir_.toStdString());

    if (vm.count("max-parallel-simulations")) {
        max_parallel_sims_ = vm["max-parallel-simulations"].as<int>();
    } else max_parallel_sims_ = 0;

    if (vm.count("threads-per-simulation")) {
        threads_per_sim_ = vm["threads-per-simulation"].as<int>();
    } else threads_per_sim_ = 1;

    if (vm.count("simulation-timeout")) {
        simulation_timeout_ = vm["simulation-timeout"].as<int>();
    } else simulation_timeout_ = 0;

    if (vm.count("runner-type")) {
        QString runner_str = QString::fromStdString(vm["runner-type"].as<std::string>());
        if (QString::compare(runner_str, "serial") == 0)
            runner_type_ = RunnerType::SERIAL;
        else if (QString::compare(runner_str, "oneoff") == 0)
            runner_type_ = RunnerType::ONEOFF;
        else if (QString::compare(runner_str, "mpisync") == 0)
            runner_type_ = RunnerType::MPISYNC;
    } else runner_type_ = RunnerType::SERIAL;

    if (vm.count("sim-drv-path")) {
        QString sim_drv_path = QString::fromStdString(vm["sim-drv-path"].as<std::string>());
        if (!Utilities::FileHandling::FileExists(sim_drv_path))
            throw std::runtime_error("Simulation driver file specified as argument does not exist: " + sim_drv_path.toStdString());
        else simulator_driver_path_ = sim_drv_path;
    } else simulator_driver_path_ = "";

    if (vm.count("sim-exec-path")) {
        QString sim_exec_path = QString::fromStdString(vm["sim-exec-path"].as<std::string>());
        if (!Utilities::FileHandling::FileExists(sim_exec_path))
            throw std::runtime_error("Custom executable file path specified as argument does not exist: " + sim_exec_path.toStdString());
        else simulator_exec_script_path_ = sim_exec_path;
    } else simulator_exec_script_path_ = "";

    if (vm.count("fieldopt-build-dir")) {
        QString fieldopt_build_dir = QString::fromStdString(vm["fieldopt-build-dir"].as<std::string>());
        if (!Utilities::FileHandling::DirectoryExists(fieldopt_build_dir))
            throw std::runtime_error("FieldOpt build directory specified as argument does not exist: " + fieldopt_build_dir.toStdString());
        else fieldopt_build_dir_ = fieldopt_build_dir;
    } else fieldopt_build_dir_ = "";

    if (vm.count("grid-path")) {
        QString grid_path = QString::fromStdString(vm["grid-path"].as<std::string>());
        if (!Utilities::FileHandling::FileExists(grid_path))
            throw std::runtime_error("Grid file path specified as argument does not exist: " + fieldopt_build_dir_.toStdString());
        else grid_file_path_ = grid_path;
    } else grid_file_path_ = "";

    if (vm.count("well-prod-points")) {
        if (vm["well-prod-points"].as<std::vector<double>>().size() != 6)
            throw std::runtime_error("Exactly six coordinates must be provided for the production well position.");
        std::vector<double> coords = vm["well-prod-points"].as<std::vector<double>>();
        prod_coords_.first = QVector<double>() << coords[0] << coords[1] << coords[2];
        prod_coords_.second = QVector<double>() << coords[3] << coords[4] << coords[5];
    }
    if (vm.count("well-inj-points")) {
        if (vm["well-inj-points"].as<std::vector<double>>().size() != 6)
            throw std::runtime_error("Exactly six coordinates must be provided for the injection well position.");
        std::vector<double> coords = vm["well-inj-points"].as<std::vector<double>>();
        inje_coords_.first = QVector<double>() << coords[0] << coords[1] << coords[2];
        inje_coords_.second = QVector<double>() << coords[3] << coords[4] << coords[5];
    }


    if (verbosity_level_) {
        str_out = "FieldOpt runtime settings";
        std::cout << "\n" << str_out << "\n" << std::string(str_out.length(),'=') << std::endl;
        std::cout << "Verbosity level:  " << verbosity_level_ << std::endl;
        std::cout << "Runner type:      " << runnerTypeString().toStdString() << std::endl;
        std::cout << "Overwr. old out files: " << overwrite_existing_ << std::endl;
        std::cout << "Max parallel sims:   " << (max_parallel_sims_ > 0 ? boost::lexical_cast<std::string>(max_parallel_sims_) : "default") << std::endl;
        std::cout << "Threads pr sim:      " << boost::lexical_cast<std::string>(threads_per_sim_) << std::endl;
        str_out = "Current/specified paths:";
        std::cout << "\n" << str_out << "\n" << std::string(str_out.length(),'-') << std::endl;
        std::cout << "Current dir:-------" << Utilities::FileHandling::GetCurrentDirectoryPath().toStdString() << std::endl;
        std::cout << "Input file:--------" << driver_file_.toStdString() << std::endl;
        std::cout << "Output dir:--------" << output_dir().toStdString() << std::endl;
        std::cout << "Sim driver file:---" << (simulator_driver_path_.length() > 0 ? simulator_driver_path_.toStdString() : "from FieldOpt driver file") << std::endl;
        std::cout << "Grid file path:----" << (grid_file_path_.length() > 0 ? grid_file_path_.toStdString() : "from FieldOpt driver file") << std::endl;
        std::cout << "Exec file path:----" << (simulator_exec_script_path_.length() > 0 ? simulator_exec_script_path_.toStdString() : "from FieldOpt driver file") << std::endl;
        std::cout << "Build dir:---------" << fieldopt_build_dir_.toStdString() << std::endl;
        if (vm.count("well-prod-points"))
            std::cout << "Producer coordinates:   " << wellSplineCoordinateString(prod_coords_).toStdString() << std::endl;
        if (vm.count("well-prod-points"))
            std::cout << "Injector coordinates:   " << wellSplineCoordinateString(inje_coords_).toStdString() << std::endl;
    }
}

QString RuntimeSettings::wellSplineCoordinateString(const QPair<QVector<double>, QVector<double>> spline) const {
    return QString("(%1, %2, %3) - (%4, %5, %6)")
        .arg(spline.first[0]).arg(spline.first[1]).arg(spline.first[2])
        .arg(spline.second[0]).arg(spline.second[1]).arg(spline.second[2]);
}


QString RuntimeSettings::runnerTypeString() const {
    if (runner_type_ == RunnerType::SERIAL)
        return "serial";
    else if (runner_type_ == RunnerType::ONEOFF)
        return "oneoff";
    else if (runner_type_ == RunnerType::MPISYNC)
        return "mpisync";
    else return "NOT SET";
}

po::variables_map RuntimeSettings::createVariablesMap(int argc, const char **argv) {
    int max_par_sims;
    int thr_per_sim;
    int simulation_timeout;
    int verbosity_level;
    po::options_description desc("FieldOpt options");
    desc.add_options()
        ("help,h", "print help message")
        ("verbose,v", po::value<int>(&verbosity_level)->default_value(0),
         "verbosity level for runtime console logging")
        ("force,f", po::value<int>()->implicit_value(0),
         "overwrite existing output files")
        ("max-parallel-simulations,m", po::value<int>(&max_par_sims)->default_value(0),
         "start max <arg> parallel simulations")
        ("threads-per-simulation,n", po::value<int>(&thr_per_sim)->default_value(1),
         "number of threads allocated to each simulation")
        ("runner-type,r", po::value<std::string>(),
         "type of runner (serial/oneoff/mpisync)")
        ("grid-path,g", po::value<std::string>(),
         "path to model grid file (e.g. *.GRID)")
        ("sim-exec-path,e", po::value<std::string>(),
         "path to script that executes the reservoir simulation")
        ("fieldopt-build-dir,b", po::value<std::string>(),
         "path to FieldOpt build directory")
        ("sim-drv-path,s", po::value<std::string>(),
         "path to simulator driver file (e.g. *.DATA)")
        ("simulation-timeout,t", po::value<int>(&simulation_timeout)->default_value(0),
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
        exit(EXIT_SUCCESS);
    }

    return vm;
}
Loggable::LogTarget RuntimeSettings::GetLogTarget() {
    return Loggable::LogTarget::SUMMARY_PRERUN;
}
map<string, string> RuntimeSettings::GetState() {
    map<string, string> statemap;
    statemap["verbosity"] = boost::lexical_cast<string>(verbosity_level_);
    statemap["sim_maxpar"] = boost::lexical_cast<string>(max_parallel_sims_);
    statemap["sim_threads"] = boost::lexical_cast<string>(threads_per_sim_);
    statemap["sim_timeout"] = boost::lexical_cast<string>(simulation_timeout_);

    statemap["overwrite"] = overwrite_existing_ ? "Yes" : "No";

    switch (runner_type_) {
        case SERIAL: statemap["runner"] = "Serial"; break;
        case ONEOFF: statemap["runner"] = "One-off"; break;
        case MPISYNC: statemap["runner"] = "MPI Parallel"; break;
    }

    statemap["path_driver"] = driver_file_.toStdString();
    statemap["path_output"] = output_dir_.toStdString();
    statemap["path_simdriver"] = simulator_driver_path_.toStdString();
    statemap["path_grid"] = grid_file_path_.toStdString();
    statemap["path_simexec"] = simulator_exec_script_path_.toStdString();
    statemap["path_build"] = fieldopt_build_dir_.toStdString();
    return statemap;
}
QUuid RuntimeSettings::GetId() {
    return nullptr;
}
map<string, vector<double>> RuntimeSettings::GetValues() {
    map<string, vector<double>> valmap;
    return valmap;
}

}
