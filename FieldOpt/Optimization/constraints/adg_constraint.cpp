/***********************************************************
 Copyright (C) 2017
 Mathias C. Bellout <mathias.bellout@ntnu.no>

 Created by bellout on 5/6/18.

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
#include "adg_constraint.h"

//#include "optimizers/SNOPTSolverC.h"
//#include <FieldOpt-WellIndexCalculator/resinxx/rixx_prj_viz/RivIntersectionGeometryGenerator.h>

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

using ::Utilities::FileHandling::FileExists;
using ::Utilities::FileHandling::WriteStringToFile;
using ::Utilities::FileHandling::WriteLineToFile;
using ::Utilities::FileHandling::EstablishFile;

//==========================================================
ADGConstraint::ADGConstraint(
    Settings::Optimizer *settings,
    Model::Properties::VariablePropertyContainer *variables,
    ::Reservoir::Grid::Grid *grid,
    RICaseData *ricasedata) {

  // -------------------------------------------------------
  settings_ = settings;
  // variables_ = variables;
  grid_ = grid;
  ricasedata_ = ricasedata;

}

//==========================================================
void ADGConstraint::SnapCaseToConstraints(Case *current_case) {

  // -----------------------------------------------------
  if (settings_->verb_vector()[4] > 2) {
    string str_out = "[con]Launching ADGPRS Constraint Handling";
    cout << endl << endl << BLDON << BRED
         << std::string(120, '=') << endl
         << std::string(120, '=')
         << endl << str_out << "("
         << current_case->get_case_num() << ")" << endl
         << current_case->id_stdstr() << AEND << endl << endl;
  }


  string target_dir;
  string optz;
  string opt_file;
  string nthreads;

  // -------------------------------------------------------
  auto cdir = settings_->sim_dirs_.driver_directory_;

  QString xin = "x_" + QString::fromStdString(current_case->id_stdstr()) + ".in";
  QString schedout = "sched_" + QString::fromStdString(current_case->id_stdstr()) + ".out";
  EstablishFile(xin);

  // -------------------------------------------------------
  // Print out x.in based on current_case
  auto var_map = current_case->GetUUIDSplineVarNameMap();

  for (auto var=var_map.begin(); var != var_map.end(); ++var) {

    // WriteLineToFile(QString::fromStdString(var->first), xin);
    WriteLineToFile(QString::number(var->second), xin);

  }

  // -------------------------------------------------------
  // target_dir = "5spot-cntrlopt";
  optz = "/home/bellout/git/ADGPRS/20161124-ad-gprs-optimizer-ov-src/Optimization20161120/cmake-build-debug/bin/optimize-cmake";
  opt_file = cdir.toStdString() + "/OPT.txt";
  // nthreads = " 1 0 -p " + xin.toStdString() + " " + schedout.toStdString();
  nthreads = " 1 0 -p " + xin.toStdString();

  // -------------------------------------------------------
  if (FileExists(QString::fromStdString(optz))) {

    // -----------------------------------------------------
    chdir(target_dir.c_str());
    system("echo PWD=${PWD}");

    // -----------------------------------------------------
    // optimize OPT.txt 1 0 -p x.in sched.out
    string cmd_in = optz + " " + opt_file + nthreads;
    printf ("Executing: %s\n", cmd_in.c_str());


    // -----------------------------------------------------
    // System cal.
    int i = system( cmd_in.c_str() );

    // -----------------------------------------------------
    printf ("The value returned was: %d.\n", i);

  } else {

    printf ("Execution failed.");
  }

  // -------------------------------------------------------
  // Get ADGPRS to print out feasible x x.out

  // -------------------------------------------------------
  // Read x.out and replace values in current case

  // -----------------------------------------------------
  if (settings_->verb_vector()[4] > 2) {
    string str_out = "[con]Ending ADGPRS Constraint Handling";
    cout << BLDON << BRED
         << std::string(120, '=') << endl
         << std::string(120, '=') << endl
         << str_out << "(" << current_case->get_case_num()
         << ")"<< AEND << endl << endl << endl ;
  }

}


//==========================================================
bool ADGConstraint::CaseSatisfiesConstraint(Case *current_case) {

//  // -------------------------------------------------------
//  if (!distance_constraint_->CaseSatisfiesConstraint(c)){
//    return false;
//  }
//
//  // -------------------------------------------------------
//  for (WellSplineLength *wsl : length_constraints_) {
//    if (!wsl->CaseSatisfiesConstraint(c))
//      return false;
//  }
//
  // return true;
  return false;

}


}
}