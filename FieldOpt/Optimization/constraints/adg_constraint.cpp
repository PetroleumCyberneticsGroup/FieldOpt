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
using ::Utilities::FileHandling::CopyFile;
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
  // Dbg
  if (settings_->verb_vector()[4] > 2) {
    string str_out = "[con]Launching ADGPRS Constraint Handling";
    cout << endl << endl;
    cout << BLDON << BRED << std::string(120, '=') << AEND << endl;
    cout << BLDON << BRED << std::string(120, '=') << AEND << endl;
    cout << BLDON << BRED
         << str_out << "(" << current_case->get_case_num() << ")"
         << AEND << endl << endl;
  }

  // -------------------------------------------------------
  // Set input/output file names
  auto id_str = QString::fromStdString(current_case->id_stdstr());

  QString xin = "x_" + id_str + ".in";
  QString xout = "x_" + id_str + ".out";
  QString snout = "snopt_" + id_str + ".out";
  QString snlog = "snlog_" + id_str + ".out";

  // -------------------------------------------------------
  // Print out x.in based on current_case
  EstablishFile(xin);
  vector<double> invec, outvec;
  map<string, double> var_map = current_case->GetSplineVarNameMap();

  // -------------------------------------------------------
  for (auto var = var_map.begin(); var != var_map.end(); ++var) {

    // -----------------------------------------------------
    // Print well order (dbg)
    if(current_case->get_case_num() == 1) {
      WriteLineToFile(QString::fromStdString(var->first),
                      "var_order.in");
    }

    // -----------------------------------------------------
    // Print actual values
    // cout << var->second << endl; // dbg
    invec.push_back(var->second);
    WriteLineToFile(QString::number(var->second, 'E', 16), xin);

  }

  // -------------------------------------------------------
//  string target_dir;
//  string optz;
//  string opt_file;
//  string nthreads;

  // -------------------------------------------------------
  // target_dir = "5spot-cntrlopt";

//  optz_name = "/home/bellout/git/ADGPRS/20161124-ad-gprs-optimizer-ov-src"
//      "/Optimization20161120/cmake-build-debug/bin/optimize-cmake-mb";
  // nthreads = " 1 0 -p " + xin.toStdString() + " " + schedout.toStdString();
  // nthreads = " 1 0 -p " + xin.toStdString();

  // -------------------------------------------------------
  auto cdir = settings_->sim_dirs_.driver_directory_;
  string optz_name = "optimize-cmake-mb";
  string optz_file = cdir.toStdString() + "/OPT.txt";
  string nthreads = "1 0 --uof-projection " + xin.toStdString();

  // -------------------------------------------------------
  // Get ADGPRS to print out feasible x x.out
  if ( FileExists(QString::fromStdString(optz_name))
      || FileExists(QString::fromStdString(optz_file)) ) {

    // -----------------------------------------------------
    // chdir(target_dir.c_str());
    system("echo PWD=${PWD}");

    // -----------------------------------------------------
    // optimize OPT.txt 1 0 -p x.in sched.out
    string cmd_in = optz_name + " " + optz_file + " " + nthreads;
    printf("Executing: %s%s%s\n", FRED, cmd_in.c_str(), AEND);

    // -----------------------------------------------------
    // System call
    int i = system( cmd_in.c_str() );

    // -----------------------------------------------------
    printf ("The value returned was: %d.\n", i);
    // sleep(2);

  } else {

    printf ("Files not found.");
  }

  // -------------------------------------------------------
  // Read x.out
  std::ifstream rxout;
  rxout.open("x.out", std::ios_base::in);
  rxout.scientific;

  double coord;
  while (rxout >> coord) {
    outvec.push_back(coord);
  }

  rxout.close();

  // -------------------------------------------------------
  // Introduce x.out values to current case
  auto it = 0;
  map<string, QUuid> name_map = current_case->GetUUIDNameMap();
  QHash<QUuid, string> var_names = current_case->GetRealVarNames();

  // -------------------------------------------------------
  for (auto var=var_map.begin(); var != var_map.end(); ++var) {

    assert(var_names[name_map[var->first]] == var->first); // dbg
    current_case->set_real_variable_value(name_map[var->first],
                                          outvec[it]);

    // Updates real_wpline_nfbck variables with real_nfbck
    // vector (not updated by set_real_variable_value)
    current_case->UpdateWSplineVarValues();
    it++;

  }

  // -------------------------------------------------------
  // Save xout file for debug
  CopyFile(QString::fromStdString("x.out"), xout, true);

  if (FileExists(QString::fromStdString("input_errors.log"))) {
    CopyFile(QString::fromStdString("input_errors.log"), snout, true);
  }

  if (FileExists(QString::fromStdString("snopt_errors.log"))) {
    CopyFile(QString::fromStdString("snopt_errors.log"), snlog, true);
  }

  // -------------------------------------------------------
  // Dbg: Print diff b/t input and output vectors
  assert(outvec.size() == invec.size());
  int i = 0;
  cout << std::setprecision(16) << std::scientific << endl;

  for (auto var = var_map.begin(); var != var_map.end(); ++var) {
    cout << FCYAN << var->first << ":\t" << AEND
         << invec[i] - outvec[i] << endl;
    i++;
  }

  // -----------------------------------------------------
  // Dbg
  if (settings_->verb_vector()[4] > 2) {
    string str_out = "[con]Ending ADGPRS Constraint Handling";
    cout << endl << endl;
    cout << BLDON << BRED << std::string(120, '=') << AEND << endl;
    cout << BLDON << BRED << std::string(120, '=') << AEND << endl;
    cout << BLDON << BRED
         << str_out << "(" << current_case->get_case_num() << ")"
         << AEND << endl << endl << endl;
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