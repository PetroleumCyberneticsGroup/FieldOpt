/******************************************************************************
 *
 *
 *
 * Created: 16.12.2015 2015 by einar
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

#include "runtime_settings.h"

namespace Runner {

RuntimeSettings::RuntimeSettings(boost::program_options::variables_map vm)
{
    verbose_ = vm.count("verbose") != 0;

    if (verbose_)
        str_out = "FieldOpt runtime settings";
        std::cout << str_out << "\n" << std::string(str_out.length(),'=') << std::endl;
        std::cout << "Verbosity level:\t" << verbose_ << std::endl;

    if (vm.count("runner-type")) {
        QString runner_str = QString::fromStdString(vm["runner-type"].as<std::string>());
        if (QString::compare(runner_str, "serial") == 0)
            runner_type_ = RunnerType::SERIAL;
        else if (QString::compare(runner_str, "oneoff") == 0)
            runner_type_ = RunnerType::ONEOFF;
    } else runner_type_ = RunnerType::SERIAL;
    if (verbose_)
        std::cout << "Runner type:\t" << runnerTypeString().toStdString() << std::endl;

    if (vm.count("max-parallel-simulations")) {
        max_parallel_sims_ = vm["max-parallel-simulations"].as<int>();
    } else max_parallel_sims_ = 0;
    if (verbose_)
        std::cout << "Max parallel sims:\t" << (max_parallel_sims_ > 0 ? std::to_string(max_parallel_sims_) : "default") << std::endl;

    if (vm.count("simulation-timeout")) {
        simulation_timeout_ = vm["simulation-timeout"].as<int>();
    } else simulation_timeout_ = 0;

    overwrite_existing_ = vm.count("force") != 0;
    if (!overwrite_existing_ && !Utilities::FileHandling::DirectoryIsEmpty(output_dir_))
        throw std::runtime_error("Output directory is not empty. Use the --force flag to overwrite existing content.");
    if (verbose_)
        std::cout << "Overwr. existing out files: " << overwrite_existing_ << std::endl;

    if (vm.count("input-file")) {
        driver_file_ = QString::fromStdString(vm["input-file"].as<std::string>());
        if (verbose_)
            std::cout << "Current dir:\t" << Utilities::FileHandling::GetCurrentDirectoryPath().toStdString() << std::endl;
            std::cout << "Input file:\t" << driver_file_.toStdString() << std::endl;
        if (!Utilities::FileHandling::FileExists(driver_file_))
            throw std::runtime_error("The specified driver file does not exist.");
    } else throw std::runtime_error("An input file must be specified.");

    if (vm.count("output-dir")) {
        output_dir_ = QString::fromStdString(vm["output-dir"].as<std::string>());
        if (verbose_)
            std::cout << "Output dir:\t" << output_dir().toStdString() << std::endl;
        if (!Utilities::FileHandling::DirectoryExists(output_dir_))
            throw std::runtime_error("The specified output directory does not exist.");
    } else throw std::runtime_error("An output directory must be specified.");

    if (vm.count("sim-drv-path")) {
        QString sim_drv_path = QString::fromStdString(vm["sim-drv-path"].as<std::string>());
        if (verbose_)
            std::cout << "Sim driver file:\t" << (sim_drv_path.length() > 0 ? sim_drv_path.toStdString() : "from FieldOpt driver file") << std::endl;
        if (!Utilities::FileHandling::FileExists(sim_drv_path))
            throw std::runtime_error("Specified simulation driver file does not exist.");
        else simulator_driver_path_ = sim_drv_path;
    } else simulator_driver_path_ = "";

    if (vm.count("sim-exec-path")) {
        QString sim_exec_path = QString::fromStdString(vm["sim-exec-path"].as<std::string>());
        if (verbose_)
            std::cout << "Exec file path:\t" << (sim_exec_path.length() > 0 ? sim_exec_path.toStdString() : "from FieldOpt driver file") << std::endl;
        if (!Utilities::FileHandling::FileExists(sim_exec_path))
            throw std::runtime_error("Custom executable file path specified as argument does not exist.");
        else simulator_exec_script_path_ = sim_exec_path;
    } else simulator_exec_script_path_ = "";

    if (vm.count("fieldopt-build-dir")) {
        QString fieldopt_build_dir = QString::fromStdString(vm["fieldopt-build-dir"].as<std::string>());
        if (verbose_)
            std::cout << "Build dir:\t" << fieldopt_build_dir.toStdString() << std::endl;
        if (!Utilities::FileHandling::DirectoryExists(fieldopt_build_dir))
            throw std::runtime_error("FieldOpt build directory specified as argument does not exist.");
        else fieldopt_build_dir_ = fieldopt_build_dir;
    } else fieldopt_build_dir_ = "";

    if (vm.count("grid-path")) {
        QString grid_path = QString::fromStdString(vm["grid-path"].as<std::string>());
        if (verbose_)
            std::cout << "Grid file path: " << (grid_path.length() > 0 ? grid_path.toStdString() : "from FieldOpt driver file") << std::endl;
        if (!Utilities::FileHandling::FileExists(grid_path))
            throw std::runtime_error("Grid file path specified as argument does not exist.");
        else grid_file_path_ = grid_path;
    } else grid_file_path_ = "";

    if (vm.count("well-prod-points")) {
        if (vm["well-prod-points"].as<std::vector<double>>().size() != 6)
            throw std::runtime_error("Exactly six coordinates must be provided for the production well position.");
        std::vector<double> coords = vm["well-prod-points"].as<std::vector<double>>();
        prod_coords_.first = QVector<double>() << coords[0] << coords[1] << coords[2];
        prod_coords_.second = QVector<double>() << coords[3] << coords[4] << coords[5];
        if (verbose_)
            if (vm.count("well-prod-points"))
                std::cout << "Producer coordinates:   " << wellSplineCoordinateString(prod_coords_).toStdString() << std::endl;
    }
    if (vm.count("well-inj-points")) {
        if (vm["well-inj-points"].as<std::vector<double>>().size() != 6)
            throw std::runtime_error("Exactly six coordinates must be provided for the injection well position.");
        std::vector<double> coords = vm["well-inj-points"].as<std::vector<double>>();
        inje_coords_.first = QVector<double>() << coords[0] << coords[1] << coords[2];
        inje_coords_.second = QVector<double>() << coords[3] << coords[4] << coords[5];
        if (verbose_)
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
    else return "NOT SET";
}

}
