/******************************************************************************
   Copyright (C) 2015-2016 Einar J.M. Baumann <einar.baumann@gmail.com>
   Modified by M.Bellout (2017) <mathias.bellout@ntnu.no, chakibbb@gmail.com>
   Modified by Alin G. Chitu (2016-2017) <alin.chitu@tno.nl, chitu_alin@yahoo.com>

   This file and the WellIndexCalculator as a whole is part of the
   FieldOpt project. However, unlike the rest of FieldOpt, the
   WellIndexCalculator is provided under the GNU Lesser General Public
   License.

   WellIndexCalculator is free software: you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation, either version 3 of
   the License, or (at your option) any later version.

   WellIndexCalculator is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with WellIndexCalculator.  If not, see
   <http://www.gnu.org/licenses/>.
******************************************************************************/

/*!
 * @brief This file contains helper methods for the main.cpp file in
 * this folder.
 * It contains methods to parse the program arguments, as well as well
 * as methods to print the output in various formats.
 */

#ifndef WIC_MAIN_H
#define WIC_MAIN_H

// -----------------------------------------------------------------
// STD
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <map>

// -----------------------------------------------------------------
// Boost
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

// -----------------------------------------------------------------
// FieldOpt:WIC
#include "intersected_cell.h"

// -----------------------------------------------------------------
// OV
#if _WIN32
#include <sys/stat.h>
    inline bool exists(const std::string& name)
    {
        struct stat buffer;
        return (stat (name.c_str(), &buffer) == 0);
    }
#else
#include <boost/filesystem/operations.hpp>
#endif

// -----------------------------------------------------------------

namespace po = boost::program_options;
using namespace Reservoir::WellIndexCalculation;
using namespace std;

const double minimum_well_index = 1e-9;

// -----------------------------------------------------------------
void printCsv(map<string, vector<IntersectedCell>> &well_indices) {
  cout << "i,\tj,\tk1,\tk2,\twi" << endl;

  vector<string> well_names;
  for(map<string, vector<IntersectedCell>>::iterator
          it = well_indices.begin(); it != well_indices.end(); ++it) {
    well_names.push_back(it->first);
  }

  for (auto well_name : well_names) {
    // Write the well name
    cout << well_name << endl;

    // Write the matrix part
    for (auto block : well_indices[well_name]) {
      if (block.is_active_matrix() && block.cell_well_index_matrix() > minimum_well_index) {
        auto line = boost::str(boost::format("%d,\t%d,\t%d,\t%d,\t%s")
                                   %(block.ijk_index().i() + 1)         // %1
                                   %(block.ijk_index().j() + 1)         // %2
                                   %(block.ijk_index().k() + 1)         // %3
                                   %(block.ijk_index().k() + 1)         // %4
                                   %block.cell_well_index_matrix());    // %5
        cout << line << endl;
      }
    }

    // Write the fracture part
    for (auto block : well_indices[well_name]) {
      if (block.is_active_fracture() && block.cell_well_index_fracture() > minimum_well_index) {
        auto line = boost::str(boost::format("%d,\t%d,\t%d,\t%d,\t%s")
                                   %(block.ijk_index().i() + 1)         // %1
                                   %(block.ijk_index().j() + 1)         // %2
                                   %(block.ijk_index().k() + 1)         // %3
                                   %(block.ijk_index().k() + 1)         // %4
                                   %block.cell_well_index_fracture());  // %5
        cout << line << endl;
      }
    }
  }
}

// -----------------------------------------------------------------
void printCompdat(map<string, vector<IntersectedCell>> &well_indices) {
  string head = "COMPDAT\n";
  string foot = "\n/";
  vector<string> body;
  //                      NAME   I   J  K1  K2  OP/SH ST  WI   DIAM
  string compdat_frmt = "   %s  %d  %d  %d  %d  OPEN  1*  %8s  %8s /";

  vector<string> well_names;
  for(map<string, vector<IntersectedCell>>::iterator
          it = well_indices.begin(); it != well_indices.end(); ++it) {
    well_names.push_back(it->first);
  }

  for (auto well_name : well_names) {
    for (auto block : well_indices[well_name]) {
      // Write the matrix part
      if (block.is_active_matrix() && block.cell_well_index_matrix() > minimum_well_index) {
        auto entry = boost::str(boost::format(compdat_frmt)
                                    % well_name             			// %1
                                    %(block.ijk_index().i() + 1) 		// %2
                                    %(block.ijk_index().j() + 1) 		// %3
                                    %(block.ijk_index().k() + 1) 		// %4
                                    %(block.ijk_index().k() + 1) 		// %5
                                    %(block.cell_well_index_matrix())	// %6
                                    %(2*block.get_segment_radius(0)));  // %7
        body.push_back(entry);
      }

      // Write the fracure part
      if (block.is_active_fracture() && block.cell_well_index_fracture() > minimum_well_index) {
        auto entry = boost::str(boost::format(compdat_frmt)
                                    % well_name             			// %1
                                    %(block.ijk_index().i() + 1) 		// %2
                                    %(block.ijk_index().j() + 1) 		// %3
                                    %(block.k_fracture_index() + 1) 	// %4
                                    %(block.k_fracture_index() + 1) 	// %5
                                    %(block.cell_well_index_fracture())	// %6
                                    %(2*block.get_segment_radius(0)));  // %7
        body.push_back(entry);
      }
    }
  }

  // Write the full block to file after adding the head and the foot
  string full = head + boost::algorithm::join(body, "\n") + foot;
  cout << full << endl;
}

// -----------------------------------------------------------------
void printDebug(map<string, vector<IntersectedCell>> &well_indices) {

  ofstream debugfile;
  debugfile.open ("debug_info.dat");

  vector<string> body;
  vector<string> well_names;
  for(map<string, vector<IntersectedCell>>::iterator
          it = well_indices.begin(); it != well_indices.end(); ++it) {
    well_names.push_back(it->first);
  }

  for (auto well_name : well_names) {
    debugfile << well_name << endl;

    auto block = well_indices[well_name].at(0);
    map<string, vector<double>> calc_data = block.get_calculation_data();
    vector<string> data_names;

    for(map<string, vector<double>>::iterator
            it = calc_data.begin(); it != calc_data.end(); ++it) {
      data_names.push_back(it->first);
    }

    debugfile << "i\tj\tk\t";
    for (string item : data_names) {
      debugfile << item << "\t";
    }

    debugfile << "-- repeated for all segments" << endl;

    for (auto block : well_indices[well_name]) {
      debugfile << block.ijk_index().i() + 1 << "\t"
                << block.ijk_index().j() + 1 << "\t"
                << block.ijk_index().k() + 1 << "\t";
      calc_data = block.get_calculation_data();

      for (int iSegment = 0; iSegment < calc_data[data_names[0]].size(); ++iSegment) {
        for (string item : data_names) {
          debugfile << calc_data[item].at(iSegment) << "\t";
        }
      }
      debugfile << endl;
    }
  }
  debugfile.close();
}

// -----------------------------------------------------------------
po::variables_map createVariablesMap(int argc, const char **argv) {

  // This function parses the runtime arguments and creates
  // a boost::program_options::variable_map from them.
  // It also displays help if the --help flag is passed.

  po::options_description desc("WellIndexCalc options");
  desc.add_options()
      ("help", "print help message")

      ("debug", po::value<int>()->implicit_value(0), "write debug information")

      ("grid,g", po::value<string>()->required(), "path to model grid file (e.g. *.GRID)")

      ("well-filedef,f", po::value<string>(), "path to the well(s) definition file name")

      ("heel,h", po::value<vector<double>>()->multitoken(), "Heel coordinates (x y z)")

      ("toe,t", po::value<vector<double>>()->multitoken(), "Toe coordinates (x y z)")

      ("radius,r", po::value<double>(), "wellbore radius")

      ("skin-factor,s", po::value<double>(), "skin factor")

      ("compdat,c", po::value<int>()->implicit_value(0), "print in compdat format instead of CSV")

      ("well-name,w", po::value<string>(), "well name to be used when writing compdat");

  // Process arguments to variable map
  po::variables_map vm;

  bool success_arg = true;
  try {
    // Parse the input arguments and store the values
    po::store(po::parse_command_line(
        argc, argv, desc), vm);

    // ??
    po::notify(vm);
  }
  catch (std::exception& e) {
    success_arg = false;
    cout << e.what() << endl;
  }

  // If called with --help or -h flag:
  if (vm.count("help") || !success_arg)
  { // Print help if --help present or input file/output dir not present
    cout << "Usage: ./wicalc_ri --grid gridpath --heel x1 y1 z1 "
        "--toe x2 y2 z2 --radius r --skin-factor s [options]" << endl;
    cout << "options can be --compdat --well-name Name" << endl;
    cout << "Or" << endl;
    cout << "Usage: ./wicalc_ri --grid gridpath --well-filedef filepath" << endl;
    cout << desc << endl;
    exit(EXIT_SUCCESS);
  }

  // Make sure that the user either specifies an input file
  // or the data for a single well segment is specified correctly
  if (!vm.count("grid")) {
    cerr << "Error: You must provide a grid path." << endl;
    exit(EXIT_FAILURE);
  }
  if (!vm.count("well-filedef") && (!vm.count("heel") || !vm.count("toe"))) {
    cerr << "Error: You must provide a well definition file or heel and toe." << endl;
    exit(EXIT_FAILURE);
  }
  if (!vm.count("well-filedef") && !vm.count("radius")) {
    cerr << "Error: You must provide a well definition file or a well radius." << endl;
    exit(EXIT_FAILURE);
  }
  if (!vm.count("well-filedef") && !vm.count("skin-factor")) {
    cerr << "Error: You must provide a well definition file or a skin factor." << endl;
    exit(EXIT_FAILURE);
  }
  if (!vm.count("well-filedef")
      && vm["heel"].as<vector<double>>().size() != 3
      && vm["toe"].as<vector<double>>().size() != 3) {
    cerr << "Error: You must provide a well definition file or provide three values for heel and toe parameters." << endl;
    exit(EXIT_FAILURE);
  }
  if (!vm.count("well-filedef") && vm["radius"].as<double>() <= 0.0) {
    cerr << "Error: Well radius must be larger than zero." << endl;
    exit(EXIT_FAILURE);
  }
  if (!vm.count("well-filedef") && vm["skin-factor"].as<double>() < 0.0) {
    cerr << "Error: Skin factor must be larger or equal to zero." << endl;
    exit(EXIT_FAILURE);
  }

  // Notify if any unhandled errors are encountered while parsing
  po::notify(vm);
  return vm;
}

#endif // WIC_MAIN_H
