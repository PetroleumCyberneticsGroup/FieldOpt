/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it
 and/or modify it under the terms of the GNU General
 Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at
 your option) any later version.

 FieldOpt is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#include "runtime_settings.h"

#include <boost/lexical_cast.hpp>
#include <QtCore/QUuid>

// ---------------------------------------------------------
using std::cout;
using std::endl;
using std::setw;
using std::setfill;
using std::string;
using std::runtime_error;

// ---------------------------------------------------------
namespace Runner {

// =========================================================
RuntimeSettings::RuntimeSettings(int argc, const char *argv[]) {

  auto vm = createVariablesMap(argc, argv);

  // -------------------------------------------------------
  // 1.FIELDOPT BUILD DIR
  if (vm.count("fieldopt-build-dir")) {
    QString fieldopt_build_dir =
        QString::fromStdString(vm["fieldopt-build-dir"].as<string>());

    // ----------------------------------------------------->
    if (!Utilities::FileHandling::DirectoryExists(fieldopt_build_dir)) {
      throw runtime_error(
          "FieldOpt build directory specified as argument "
              "does not exist: " + fieldopt_build_dir.toStdString());
    } else {
      fo_build_dir_ = fieldopt_build_dir;
      cout << fstr("[rts]fieldopt-build-dir")
           << fo_build_dir_.toStdString() << endl;
    }
  } else fo_build_dir_ = "";

  // -------------------------------------------------------
  // 2.FIELDOPT DRIVER FILE
  if (vm.count("input-file")) {
    fo_driver_file_ =
        QString::fromStdString(vm["input-file"].as<string>());

    // -----------------------------------------------------
    auto file_exist =
        Utilities::FileHandling::FileExists(fo_driver_file_);

    // -----------------------------------------------------
    if (!file_exist) {
      throw runtime_error(
          "The specified driver file does not exist: "
              + fo_driver_file_.toStdString());
    } else {
      cout << fstr("[rts]input-file")
           << fo_driver_file_.toStdString() << endl;
    }
  } else {
    throw runtime_error("An input file must be specified.");
  }

  // -------------------------------------------------------
  // 3.FIELDOPT OUTPUT DIR
  if (vm.count("output-dir")) {
    output_dir_ = Utilities::FileHandling::GetAbsoluteFilePath(
        QString::fromStdString(vm["output-dir"].as<string>()));

    // -----------------------------------------------------
    auto dir_exists =
        Utilities::FileHandling::DirectoryExists(output_dir_);

    // -----------------------------------------------------
    if (!dir_exists) {
      throw runtime_error(
          "The specified output directory does not exist: "
              + output_dir_.toStdString());
    } else {
      cout << fstr("[rts]output-dir")
                << output_dir_.toStdString() << endl;
    }
  } else {
    throw runtime_error("An output directory must be specified.");
  }

  // -------------------------------------------------------
  // 4.SIMULATOR INCLUDE DIR
  if (vm.count("sim-incl-dir")) {
    sim_incl_dir_path_ = Utilities::FileHandling::GetAbsoluteFilePath(
        QString::fromStdString(vm["sim-incl-dir"].as<string>()));

    // -----------------------------------------------------
    auto dir_exists =
        Utilities::FileHandling::DirectoryExists(sim_incl_dir_path_);

    // -----------------------------------------------------
    if (!dir_exists) {
      throw runtime_error(
          "The specified output directory does not exist: "
              + sim_incl_dir_path_.toStdString());
    } else {
      cout << fstr("[rts]sim-incl-dir")
           << sim_incl_dir_path_.toStdString() << endl;
    }
  } else {
    throw runtime_error("Simulator include directory must be specified.");
  }

  // -------------------------------------------------------
  // 5.SIMULATOR DRIVER PATH
  if (vm.count("sim-drv-path")) {

    QString sim_drv_path =
        QString::fromStdString(vm["sim-drv-path"].as<string>());

    if (!Utilities::FileHandling::FileExists(sim_drv_path)) {
      throw runtime_error(
          "Simulation driver file specified as argument "
              "does not exist: " + sim_drv_path.toStdString());
    } else {
      sim_driver_path_ =
          Utilities::FileHandling::GetAbsoluteFilePath(sim_drv_path);
      cout << fstr("[rts]sim-drv-path")
           << sim_driver_path_.toStdString() << endl;
    }
  } else sim_driver_path_ = "";

  // -------------------------------------------------------
  // 6.SIMULATOR SCHEDULE PATH
  if (vm.count("sch-file-path")) {
    QString sch_file_path =
        QString::fromStdString(vm["sch-file-path"].as<string>());

    // -----------------------------------------------------
    if (!Utilities::FileHandling::FileExists(sch_file_path))
      throw runtime_error(
          "Simulation driver file specified as argument does not exist: "
              + sch_file_path.toStdString());
    else {

      sched_file_path_ =
          Utilities::FileHandling::GetAbsoluteFilePath(sch_file_path);
      cout << fstr("[rts]sch-file-path")
           << sched_file_path_.toStdString() << endl;
    }
  } else sched_file_path_ = "";

  // -------------------------------------------------------
  // 7.SIMULATOR GRID PATH
  if (vm.count("grid-path")) {
    QString grid_path =
        QString::fromStdString(vm["grid-path"].as<string>());

    // -----------------------------------------------------
    if (!Utilities::FileHandling::FileExists(grid_path)) {
      throw runtime_error(
          "Grid file path specified as argument does "
              "not exist: " + fo_build_dir_.toStdString());
    } else {
      grid_file_path_ = grid_path;
      cout << fstr("[rts]grid-path")
           << grid_file_path_.toStdString() << endl;
    }
  } else grid_file_path_ = "";

  // -------------------------------------------------------
  // 8.SIMULATOR EXECUTABLE PATH
  if (vm.count("sim-exec-path")) {
    QString sim_exec_path =
        QString::fromStdString(vm["sim-exec-path"].as<string>());

    // -----------------------------------------------------
    if (!Utilities::FileHandling::FileExists(sim_exec_path))
      throw runtime_error(
          "Custom executable file path specified as argument "
              "does not exist: " + sim_exec_path.toStdString());
    else {

      sim_exec_script_path_ =
          Utilities::FileHandling::GetAbsoluteFilePath(sim_exec_path);
      cout << fstr("[rts]sim-file-path")
           << sim_exec_script_path_.toStdString() << endl;
    }
  } else sim_exec_script_path_ = "";

  // -------------------------------------------------------
  // 9.VERBOSITY VECTOR
  if (vm.count("verbosity-vector")) {
    verb_vector_str_ = vm["verbosity-vector"].as<string>();

    for (int i=0; i < verb_vector_str_.length(); i++ ) {
      verb_vector_.push_back(
          boost::lexical_cast<int>(verb_vector_str_[i]));
    }
  }
  else {
    verb_vector_ = std::vector<int>(11,1);
  }

  // -------------------------------------------------------
  // 10.FLAG: FORCE
  overwrite_existing_ = vm.count("force") != 0;

  if (!overwrite_existing_
      && !Utilities::FileHandling::DirectoryIsEmpty(output_dir_)) {
    throw runtime_error(
        "Output directory is not empty. Use the --force "
            "flag to overwrite existing content in: "
            + output_dir_.toStdString());
  }

  // -------------------------------------------------------
  // 11.FLAG: PARALLEL SIMULATIONS
  if (vm.count("max-parallel-simulations")) {
    max_parallel_sims_ = vm["max-parallel-simulations"].as<int>();
  } else max_parallel_sims_ = 0;

  // -------------------------------------------------------
  // 12.FLAG: THREADS-PER-SIMULATION
  if (vm.count("threads-per-simulation")) {
    threads_per_sim_ = vm["threads-per-simulation"].as<int>();
  } else threads_per_sim_ = 1;

  // -------------------------------------------------------
  // 13.FLAG: SIMULATOR TIMEOUT
  if (vm.count("simulation-timeout")) {
    simulation_timeout_ = vm["simulation-timeout"].as<int>();
  } else simulation_timeout_ = 0;

  // -------------------------------------------------------
  // 14.FLAG: RUNNER TYPE
  if (vm.count("runner-type")) {

    // -----------------------------------------------------
    QString runner_str =
        QString::fromStdString(vm["runner-type"].as<string>());

    // -----------------------------------------------------
    if (QString::compare(runner_str, "serial") == 0) {
      runner_type_ = RunnerType::SERIAL;

    } else if (QString::compare(runner_str, "oneoff") == 0) {
      runner_type_ = RunnerType::ONEOFF;

    } else if (QString::compare(runner_str, "mpisync") == 0) {
      runner_type_ = RunnerType::MPISYNC;

    }
  } else {
    runner_type_ = RunnerType::SERIAL;
  }

  // -------------------------------------------------------
  // 15.WELL-PROD-POINTS
  if (vm.count("well-prod-points")) {

    if (vm["well-prod-points"].as<vector<double>>().size() != 6) {
      throw runtime_error(
          "Exactly six coordinates must be provided "
              "for the production well position.");
    }

    std::vector<double> coords = vm["well-prod-points"].as<std::vector<double>>();
    prod_coords_.first = QVector<double>() << coords[0] << coords[1] << coords[2];
    prod_coords_.second = QVector<double>() << coords[3] << coords[4] << coords[5];

  }

  // -------------------------------------------------------
  // 16.WELL-INJ-POINTS
  if (vm.count("well-inj-points")) {

    if (vm["well-inj-points"].as<vector<double>>().size() != 6) {
      throw runtime_error(
          "Exactly six coordinates must be provided "
              "for the injection well position.");
    }

    std::vector<double> coords = vm["well-inj-points"].as<std::vector<double>>();
    inje_coords_.first = QVector<double>() << coords[0] << coords[1] << coords[2];
    inje_coords_.second = QVector<double>() << coords[3] << coords[4] << coords[5];
  }

  // -------------------------------------------------------
  // idx:0 -> run (Runner)
  if (find (verb_vector_.begin(), verb_vector_.end(), 1) != verb_vector_.end() ||
      find (verb_vector_.begin(), verb_vector_.end(), 2) != verb_vector_.end() ) {

    // -----------------------------------------------------
    str_out = "[run]FieldOpt runtime settings";
    cout << "\n" << BLDON << str_out << AEND << "\n" << string(str_out.length(),'=') << endl;
    cout << "Verbosity level:------- " << verbosity_level_ << endl;
    cout << "Verbosity vector:------ ";
    cout << "run=" << verb_vector_[0]; // (Runner)
    cout << ", ert=" << verb_vector_[1]; // (ERTWrapper)
    cout << ", hd5=" << verb_vector_[2]; // (Hdf5SummaryReader)
    cout << ", wic=" << verb_vector_[3]; // (FieldOpt-WellIndexCalculator)
    cout << ", con=" << verb_vector_[4]; // (ConstraintMath)
    cout << ", mod=" << verb_vector_[5]; // (Model)
    cout << ", opt=" << verb_vector_[6]; // (Optimization)
    cout << ", res=" << verb_vector_[7]; // (Reservoir)
    cout << ", sim=" << verb_vector_[8]; // (Simulation)
    cout << ", set=" << verb_vector_[9]; // (Settings)
    cout << ", uti=" << verb_vector_[10]; // (Utilities)
    cout << "." << endl;

    // -------------------------------------------------------
    cout << BLDON << fstr("\n[run]Current/specified paths:")
         << endl << fstr("") << AEND << endl;

    // 0.CURRENT DIR
    cout << fstr("Current dir")
         << Utilities::FileHandling::GetCurrentDirectoryPath().toStdString()
         << endl;

    // 1.FIELDOPT BUILD DIR
    cout << fstr("Build dir")
         << fo_build_dir_.toStdString()
         << endl;

    // 2.FIELDOPT DRIVER FILE
    cout << fstr("Input file")
         << fo_driver_file_.toStdString()
         << endl;

    // 3.FIELDOPT OUTPUT DIR
    cout << fstr("Output dir")
         << output_dir().toStdString() << endl;

    // 4.SIMULATOR INCLUDE DIR
    cout << fstr("Sim include dir")
         << (sim_incl_dir_path_.length() > 0 ?
             sim_incl_dir_path_.toStdString() : "from FieldOpt driver file")
         << endl;

    // 5.SIMULATOR DRIVER PATH
    cout << fstr("Sim driver file")
         << (sim_driver_path_.length() > 0 ?
             sim_driver_path_.toStdString() : "from FieldOpt driver file")
         << endl;

    // 7.SIMULATOR GRID PATH
    cout << fstr("Grid file path")
         << (grid_file_path_.length() > 0 ?
             grid_file_path_.toStdString() : "from FieldOpt driver file")
         << endl;

    // 8.SIMULATOR EXECUTABLE PATH
    cout << fstr("Exec file path")
         << (sim_exec_script_path_.length() > 0 ?
             sim_exec_script_path_.toStdString() : "from FieldOpt driver file")
         << endl;

    // -----------------------------------------------------
    // 10.FLAG: FORCE
    cout << fstr("Overwr. old out files")
         << overwrite_existing_ << endl;

    // 11.FLAG: PARALLEL SIMULATIONS
    cout << fstr("Max parallel sims")
         << (max_parallel_sims_ > 0 ?
             boost::lexical_cast<string>(max_parallel_sims_) : "default")
         << endl;

    // 12.FLAG: THREADS-PER-SIMULATION
    cout << fstr("Threads pr sim")
         << boost::lexical_cast<string>(threads_per_sim_)
         << endl;

    // 14.FLAG: RUNNER TYPE
    cout << fstr("Runner type")
         << runnerTypeString().toStdString()
         << endl;

    // -----------------------------------------------------
    // 15.WELL-PROD-POINTS
    if (vm.count("well-prod-points"))
      cout << fstr("Producer coordinates:")
           << wellSplineCoordinateString(prod_coords_).toStdString()
           << endl;

    // 16.WELL-INJ-POINTS
    if (vm.count("well-prod-points"))
      cout << fstr("Injector coordinates:")
           << wellSplineCoordinateString(inje_coords_).toStdString()
           << endl;
  }
}

// =========================================================
QString RuntimeSettings::wellSplineCoordinateString(
    const QPair<QVector<double>, QVector<double>> spline) const {

  return QString("(%1, %2, %3) - (%4, %5, %6)")
      .arg(spline.first[0]).arg(spline.first[1]).arg(spline.first[2])
      .arg(spline.second[0]).arg(spline.second[1]).arg(spline.second[2]);
}

// =========================================================
QString RuntimeSettings::runnerTypeString() const {

  if (runner_type_ == RunnerType::SERIAL) {
    return "serial";

  } else if (runner_type_ == RunnerType::ONEOFF) {
    return "oneoff";

  } else if (runner_type_ == RunnerType::MPISYNC) {
    return "mpisync";

  } else {
    return "NOT SET";
  }
}

// =========================================================
po::variables_map RuntimeSettings::
createVariablesMap(int argc, const char **argv) {

  // -------------------------------------------------------
  int max_par_sims;
  int thr_per_sim;
  int simulation_timeout;
  int verbosity_level;

  // -------------------------------------------------------
  po::options_description desc("FieldOpt options");
  desc.add_options()
      ("help,h", "print help message")

      // 1.FIELDOPT BUILD DIR
      ("fieldopt-build-dir,b", po::value<string>(),
       "path to FieldOpt build directory")

      // 2.FIELDOPT DRIVER FILE
      ("input-file", po::value<string>(),
       "path to FieldOpt driver file")

      // 3.FIELDOPT OUTPUT DIR
      ("output-dir", po::value<string>(),
       "path to folder in which to store the results.")

      // 4.SIMULATOR INCLUDE DIR
      ("sim-incl-dir,i", po::value<string>(),
       "path to simulator include dir (e.g. INPUT/)")

      // 5.SIMULATOR DRIVER PATH
      ("sim-drv-path,u", po::value<string>(),
       "path to simulator driver file (e.g. *.DATA)")

      // 6.SIMULATOR SCHEDULE PATH
      ("sch-file-path,s", po::value<string>(),
       "path to schedule file (e.g. *SCH.INC)")

      // 7.SIMULATOR GRID PATH
      ("grid-path,g", po::value<string>(),
       "path to model grid file (e.g. *.GRID)")

      // 8.SIMULATOR EXECUTABLE PATH
      ("sim-exec-path,e", po::value<string>(),
       "path to script that executes the reservoir simulation")

      // 9.VERBOSITY VECTOR
      ("verbosity-vector,c", po::value<string>(),
       "verbosity vector for section-wise console logging durting runtime")

      // 10.FLAG: FORCE
      ("force,f", po::value<int>()->implicit_value(0),
       "overwrite existing output files")

      // 11.FLAG: PARALLEL SIMULATIONS
      ("max-parallel-simulations,m", po::value<int>(&max_par_sims)->default_value(0),
       "start max <arg> parallel simulations")

      // 12.FLAG: THREADS-PER-SIMULATION
      ("threads-per-simulation,n", po::value<int>(&thr_per_sim)->default_value(1),
       "number of threads allocated to each simulation")

      // 13.FLAG: SIMULATOR TIMEOUT
      ("simulation-timeout,t", po::value<int>(&simulation_timeout)->default_value(0),
       "Simulations will be terminated after running for t*(lowest_recorded_time)")

      // 14.FLAG: RUNNER TYPE
      ("runner-type,r", po::value<string>(),
       "type of runner (serial/oneoff/mpisync)")

      // 15.WELL-PROD-POINTS
      ("well-prod-points,p", po::value<std::vector<double>>()->multitoken(),
       "Production well position coordinates")

      // 16.WELL-INJ-POINTS
      ("well-inj-points,i", po::value<std::vector<double>>()->multitoken(),
       "Injection well position coordinates")

      ;

  // -------------------------------------------------------
  // Positional arguments
  po::positional_options_description p;
  p.add("input-file", 1);
  p.add("output-dir", 2);

  // -------------------------------------------------------
  // Process arguments to variable map
  po::variables_map vm;
  po::store(
      po::command_line_parser(argc, argv)
          .options(desc).positional(p).run(), vm);
  po::notify(vm);

  // -------------------------------------------------------
  // If called with --help or -h flag:
  // Print help if --help present or input file/output
  // dir not present
  if (vm.count("help")
      || !vm.count("input-file")
      || !vm.count("output-dir")) {

    // -----------------------------------------------------
    cout << "Usage: ./FieldOpt input-file output-dir [options]" << endl;
    cout << desc << endl;
    exit(EXIT_SUCCESS);
  }

  return vm;
}

// =========================================================
Loggable::LogTarget RuntimeSettings::GetLogTarget() {
  return Loggable::LogTarget::LOG_SUMMARY;
}

// =========================================================
map<string, string> RuntimeSettings::GetState() {

  map<string, string> statemap;

  // -------------------------------------------------------
  statemap["Verbosity"] = boost::lexical_cast<string>(verbosity_level_);
  statemap["Verbosity vector"] = verb_vector_str_;
  statemap["Max. parallel sims"] = boost::lexical_cast<string>(max_parallel_sims_);
  statemap["Threads pr. sim"] = boost::lexical_cast<string>(threads_per_sim_);
  statemap["Simulator timeout"] = boost::lexical_cast<string>(simulation_timeout_);

  // -------------------------------------------------------
  statemap["Overwrite existing files"] = overwrite_existing_ ? "Yes" : "No";

  switch (runner_type_) {
    case SERIAL: statemap["Runner"] = "Serial"; break;

    case ONEOFF: statemap["Runner"] = "One-off"; break;

    case MPISYNC: statemap["Runner"] = "MPI Parallel"; break;
  }

  // -------------------------------------------------------
  statemap["path FieldOpt build directory"] = fo_build_dir_.toStdString();
  statemap["path FieldOpt driver"] = fo_driver_file_.toStdString();
  statemap["path Output Directory"] = output_dir_.toStdString();

  // -------------------------------------------------------
  statemap["path Simulator base driver"] = sim_driver_path_.toStdString();
  statemap["path Schedule file"] = sched_file_path_.toStdString();
  statemap["path Simulator include dir"] = sim_incl_dir_path_.toStdString();

  // -------------------------------------------------------
  statemap["path Grid file"] = grid_file_path_.toStdString();
  statemap["path Simulator execution script"] = sim_exec_script_path_.toStdString();

  // -------------------------------------------------------
  return statemap;
}

// =========================================================
QUuid RuntimeSettings::GetId() {
  return QUuid(); // Null UUID
}

// =========================================================
map<string, vector<double>> RuntimeSettings::GetValues() {
  map<string, vector<double>> valmap;
  return valmap;
}

}
