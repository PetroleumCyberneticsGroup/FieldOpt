//
// Created by bellout on 2/21/18.
//

#ifndef FIELDOPT_DEBUG_HPP
#define FIELDOPT_DEBUG_HPP

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <FieldOpt-WellIndexCalculator/intersected_cell.h>
#include <Settings/model.h>
#include <Utilities/colors.hpp>

using std::vector;
using std::map;
using std::cout;
using std::endl;
using std::setprecision;
using std::fixed;
using std::setw;
using std::stringstream;
using std::ofstream;
using std::setfill;
using std::string;
using std::left;
using namespace Reservoir::WellIndexCalculation;

// -----------------------------------------------------------------
void inline print_dbg_msg_wellspline_wic_coords(
    string fstr, string fn, Settings::Model::Well well_settings,
    vector<IntersectedCell> block_data, int l, int lt) {

  // Dbg file
  ofstream wicalc_file;
  wicalc_file.open (fn, ios::app); // ios::ate, ios::out
  IOFormat CleanFmt(3, 0, " ", "  ", "", "", "[", "] ");
  string GIDXh, IJKh, WIh, DXDYDZh, ENTRYPTh, EXITPTh;

  GIDXh = " GIDX ";
  IJKh = "=== IJK ===   ";
  WIh = "= WI =  ";
  DXDYDZh = "== DXDYDZ ==  ";
  ENTRYPTh = "============ XYZ ENTRY POINT ========   ";
  EXITPTh = "============ XYZ EXIT POINT =========  ";

  int wi_p = 3;
  int dx_p = 1;
  int tx_p = 1;
  int vx_p = 3;
  int lx_p = 1;
  int cx_p = 3;
  int rs_p = 3;

  if (block_data.size() > 0) {

    // Print header
    wicalc_file << GIDXh << IJKh << WIh << ENTRYPTh << EXITPTh << DXDYDZh << endl;

    // Print well name
    wicalc_file << well_settings.name.toStdString() << endl;

    Vector3d entry_pt0 = block_data[0].get_segment_entry_point(0);
    Vector3d exit_pt0 = block_data[0].get_segment_exit_point(0);

    // Turn z values negative
    if (exit_pt0(2) > 0) {
      entry_pt0(2) = -entry_pt0(2);
      exit_pt0(2) = -exit_pt0(2);
    }

    // GIDX / IJK
    wicalc_file << fixed << setw(5) << setprecision(0)
                << right << block_data[0].global_index() << " "
                << block_data[0].ijk_index().to_string();

    // WI
    wicalc_file << fixed << setw(9) << setprecision(wi_p) << right;
    wicalc_file << block_data[0].cell_well_index_matrix();

    // ENTRY / EXIT POINTS
    wicalc_file << setw(2) << " ["
                << setprecision(3) << fixed << right
                << setw(10) << entry_pt0(0) << " "
                << setw(11) << entry_pt0(1) << " "
                << setw(11) << entry_pt0(2) << "] ";
    wicalc_file << setw(2) << " ["
                << setprecision(3) << fixed << right
                << setw(10) << exit_pt0(0) << " "
                << setw(11) << exit_pt0(1) << " "
                << setw(11) << exit_pt0(2) << "] ";

    // PERMXYZ / LXYZ
    wicalc_file << setw(2) << "P=["
                << setprecision(3) << fixed << right
                << setw(10) << block_data[0].permx().at(0) << " "
                << setw(11) << block_data[0].permy().at(0) << " "
                << setw(11) << block_data[0].permz().at(0) << "] ";

    // DX / DY / DZ
    wicalc_file << fixed << setw(5) << setprecision(dx_p) << right;
    wicalc_file << "D=[";
    wicalc_file << setw(5) << right << block_data[0].get_calculation_data()["dx"].at(0) << " "
                << setw(5) << right << block_data[0].get_calculation_data()["dy"].at(0) << " "
                << setw(5) << right << block_data[0].get_calculation_data()["dz"].at(0) << "] ";

    // TX / TY / TZ
    wicalc_file << setw(3) << "T=[";
    wicalc_file << fixed << setprecision(tx_p) << right;
    wicalc_file << setw(6) << right << block_data[0].get_calculation_data()["wx_m"].at(0) << " "
                << setw(6) << right << block_data[0].get_calculation_data()["wy_m"].at(0) << " "
                << setw(6) << right << block_data[0].get_calculation_data()["wz_m"].at(0) << "] ";

    // LX / LY / LZ
    wicalc_file << "L=[";
    wicalc_file << fixed << setprecision(lx_p) << right;
    wicalc_file << setw(5) << right << block_data[0].get_calculation_data()["Lx"].at(0) << " "
                << setw(5) << right << block_data[0].get_calculation_data()["Ly"].at(0) << " "
                << setw(5) << right << block_data[0].get_calculation_data()["Lz"].at(0) << "] ";

    // XV / YV / ZV
    if (fn == "wicalc_pcg.dbg") {
      auto xvec = block_data[0].get_calculation_data_3d()["xvec"].at(0);
      auto yvec = block_data[0].get_calculation_data_3d()["yvec"].at(0);
      auto zvec = block_data[0].get_calculation_data_3d()["zvec"].at(0);
      auto cvec = block_data[0].get_calculation_data_3d()["cvec"].at(0);
      wicalc_file << "V=[";
      wicalc_file << fixed << setprecision(vx_p) << right
                  << setw(7) << right << xvec(0) << " "
                  << setw(7) << right << xvec(1) << " "
                  << setw(7) << right << xvec(2) << "] ["
                  //
                  << setw(7) << right << yvec(0) << " "
                  << setw(7) << right << yvec(1) << " "
                  << setw(7) << right << yvec(2) << "] ["
                  //
                  << setw(7) << right << zvec(0) << " "
                  << setw(7) << right << zvec(1) << " "
                  << setw(7) << right << zvec(2) << "] ["
                  //
                  << setw(7) << right << cvec(0) << " "
                  << setw(7) << right << cvec(1) << " "
                  << setw(7) << right << cvec(2) << "] ";

      // (*** WRONG FOR Z ***) CURRENT_VECTOR(cv) -- PROJECTED ONTO -- XVEC(xv) / YVEC(yv) / ZVEC(zv) CELL_SIDES
      wicalc_file << "L_CV_XV=[";
      wicalc_file << fixed << setprecision(lx_p) << right;
      wicalc_file << setw(10) << right << block_data[0].get_calculation_data()["current_Lx__cv_onto_xv"].at(0) << " "
                  << setw(11) << right << block_data[0].get_calculation_data()["current_Ly__cv_onto_yv"].at(0) << " "
                  << setw(8)  << right << block_data[0].get_calculation_data()["current_Lz__cv_onto_zv"].at(0) << "] ";

      // XVEC(xv) / YVEC(yv) / ZVEC(zv) CELL_SIDES -- PROJECTED ONTO -- CURRENT_VECTOR(cv)
      wicalc_file << "L_XV_CV=[";
      wicalc_file << fixed << setprecision(lx_p) << right;
      wicalc_file << setw(10) << right << block_data[0].get_calculation_data()["current_Lx__xv_onto_cv"].at(0) << " "
                  << setw(11) << right << block_data[0].get_calculation_data()["current_Ly__yv_onto_cv"].at(0) << " "
                  << setw(8)  << right << block_data[0].get_calculation_data()["current_Lz__zv_onto_cv"].at(0) << "] ";

      wicalc_file << "C=[";
      for (int j=0; j < block_data[0].corners().size(); j++){
        wicalc_file << setprecision(cx_p) << "("
                    << setw(10) << right << block_data[0].corners()[j].x() << " "
                    << setw(11) << right << block_data[0].corners()[j].y() << " "
                    << setw(8)  << right << block_data[0].corners()[j].z() << ") ";
      }
      wicalc_file << "]";
    }

    // RADIUS / SKIN
    wicalc_file << fixed << setw(5) << setprecision(rs_p) << right;
    wicalc_file << "R=" << block_data[0].get_segment_radius(0) << "  ";
    wicalc_file << "S=" << block_data[0].get_segment_skin(0) << "  " << endl;

    for (int i = 1; i < block_data.size(); ++i) {

      Vector3d entry_pt = block_data[i].get_segment_entry_point(0);
      Vector3d diff = block_data[i].get_segment_exit_point(0) - entry_pt;
      Vector3d exit_pt = entry_pt + diff;

      // Turn z values negative
      if (exit_pt(2) > 0) {
        entry_pt(2) = -entry_pt(2);
        exit_pt(2) = -exit_pt(2);
      }

      // GIDX / IJK
      wicalc_file << fixed << setw(5) << setprecision(0)
                  << right << block_data[i].global_index() << " "
                  << block_data[i].ijk_index().to_string();

      // WI
      wicalc_file << fixed << setw(9) << setprecision(wi_p) << right;
      wicalc_file << block_data[i].cell_well_index_matrix();

      wicalc_file << setw(2) << " ["
                  << setprecision(3) << fixed << right
                  << setw(10) << entry_pt(0) << " "
                  << setw(11) << entry_pt(1) << " "
                  << setw(11) << entry_pt(2) << "] ";
      wicalc_file << setw(2) << " ["
                  << setprecision(3) << fixed << right
                  << setw(10) << exit_pt(0) << " "
                  << setw(11) << exit_pt(1) << " "
                  << setw(11) << exit_pt(2) << "] ";

      // PERMXYZ / LXYZ
      wicalc_file << setw(2) << "P=["
                  << setprecision(3) << fixed << right
                  << setw(10) << block_data[i].permx()[0] << " "
                  << setw(11) << block_data[i].permy()[0] << " "
                  << setw(11) << block_data[i].permz()[0] << "] ";

      // DX / DY / DZ
      wicalc_file << fixed << setw(5) << setprecision(1) << right;
      wicalc_file << "D=[";
      wicalc_file << setw(5) << right << block_data[i].get_calculation_data()["dx"].at(0) << " "
                  << setw(5) << right << block_data[i].get_calculation_data()["dy"].at(0) << " "
                  << setw(5) << right << block_data[i].get_calculation_data()["dz"].at(0) << "] ";

      // TX / TY / TZ
      wicalc_file << setw(3) << "T=[";
      wicalc_file << fixed << setprecision(tx_p) << right;
      wicalc_file << setw(6) << right << block_data[i].get_calculation_data()["wx_m"].at(0) << " "
                  << setw(6) << right << block_data[i].get_calculation_data()["wy_m"].at(0) << " "
                  << setw(6) << right << block_data[i].get_calculation_data()["wz_m"].at(0) << "] ";

      // LX / LY / LZ
      wicalc_file << "L=[";
      wicalc_file << fixed << setprecision(lx_p) << right;
      wicalc_file << setw(5) << right << block_data[i].get_calculation_data()["Lx"].at(0) << " "
                  << setw(5) << right << block_data[i].get_calculation_data()["Ly"].at(0) << " "
                  << setw(5) << right << block_data[i].get_calculation_data()["Lz"].at(0) << "] ";

      // XV / YV / ZV
      if (fn == "wicalc_pcg.dbg") {
        auto xvec = block_data[i].get_calculation_data_3d()["xvec"].at(0);
        auto yvec = block_data[i].get_calculation_data_3d()["yvec"].at(0);
        auto zvec = block_data[i].get_calculation_data_3d()["zvec"].at(0);
        auto cvec = block_data[i].get_calculation_data_3d()["cvec"].at(0);
        wicalc_file << "V=[";
        wicalc_file << fixed << setprecision(vx_p) << right
                    << setw(7) << right << xvec(0) << " "
                    << setw(7) << right << xvec(1) << " "
                    << setw(7) << right << xvec(2) << "] ["
                    //
                    << setw(7) << right << yvec(0) << " "
                    << setw(7) << right << yvec(1) << " "
                    << setw(7) << right << yvec(2) << "] ["
                    //
                    << setw(7) << right << zvec(0) << " "
                    << setw(7) << right << zvec(1) << " "
                    << setw(7) << right << zvec(2) << "] ["
                    //
                    << setw(7) << right << cvec(0) << " "
                    << setw(7) << right << cvec(1) << " "
                    << setw(7) << right << cvec(2) << "] ";

        // (*** WRONG ***) CURRENT_VECTOR(cv) -- PROJECTED ONTO -- XVEC(xv) / YVEC(yv) / ZVEC(zv) CELL_SIDES
        wicalc_file << "L_CV_XV=[";
        wicalc_file << fixed << setprecision(lx_p) << right;
        wicalc_file << setw(10) << right << block_data[i].get_calculation_data()["current_Lx__cv_onto_xv"].at(0) << " "
                    << setw(11) << right << block_data[i].get_calculation_data()["current_Ly__cv_onto_yv"].at(0) << " "
                    << setw(8)  << right << block_data[i].get_calculation_data()["current_Lz__cv_onto_zv"].at(0) << "] ";

        // XVEC(xv) / YVEC(yv) / ZVEC(zv) CELL_SIDES -- PROJECTED ONTO -- CURRENT_VECTOR(cv)
        wicalc_file << "L_XV_CV=[";
        wicalc_file << fixed << setprecision(lx_p) << right;
        wicalc_file << setw(10) << right << block_data[i].get_calculation_data()["current_Lx__xv_onto_cv"].at(0) << " "
                    << setw(11) << right << block_data[i].get_calculation_data()["current_Ly__yv_onto_cv"].at(0) << " "
                    << setw(8)  << right << block_data[i].get_calculation_data()["current_Lz__zv_onto_cv"].at(0) << "] ";

        wicalc_file << "C=[";
        for (int j=0; j < block_data[i].corners().size(); j++){
          wicalc_file << setprecision(cx_p) << "("
                      << setw(10) << right << block_data[i].corners()[j].x() << " "
                      << setw(11) << right << block_data[i].corners()[j].y() << " "
                      << setw(8) << right << block_data[i].corners()[j].z() << ") ";
        }
        wicalc_file << "]";
      }

      // RADIUS / SKIN
      wicalc_file << fixed << setw(5) << setprecision(rs_p) << right;
      wicalc_file << "R=" << block_data[i].get_segment_radius(0) << "  ";
      wicalc_file << "S=" << block_data[i].get_segment_skin(0) << "  " << endl;

    } // for block_data.size()
  }
  wicalc_file.close();

}

// -----------------------------------------------------------------
void inline print_dbg_msg_snoptsolver(string fstr, // function str
                                      string cstr, // comment str
                                      double t, int l, int lt,
                                      string astr = "",
                                      Vector3d eigvec = Vector3d(0)) {
  stringstream ss0, ss1;
  ss0 << "[opt:snoptsolver]" << fstr << ":";
  ss1 << setw(40) << left << setfill('-') << ss0.str();

  if (cstr == "initsnopt" && l > lt) {
    cout << FLBLUE << ss1.str()
         << " initializing SNOPTSolver." << AEND << endl;
  }
  else if (cstr == "initsnhandler" && l > lt) {
    cout << FLBLUE << ss1.str()
         << " initializing SNOPTHandler." << AEND << endl;
  }
  else if (cstr == "snoptusrfg" && l > lt) {
    cout << FLBLUE << ss1.str()
         << " calling SNOPTusrFG_ - x vector:" << AEND << endl;
    if (astr.size() > 0){
      cout << FLBLUE << astr << AEND << endl;
    }
    IOFormat CleanFmt(1, 0, "", "", "", "", "[", "]");
    cout << setw(10) << eigvec.format(CleanFmt) << endl;
    cout << fixed << setprecision(8);

  }

}

// -----------------------------------------------------------------
void inline print_dbg_msg_wellspline(string fstr, // function str
                                     string cstr, // comment str
                                     double t, int l, int lt) {

  stringstream ss0, ss1;
  ss0 << "[mod:wellspline]" << fstr << ":";
  ss1 << setw(40) << left << setfill('-') << ss0.str();

  if (cstr == "gwb" && l > lt) {
    cout << FLBLUE << ss1.str()
         << " computing well blocks" << AEND << endl;
  }
  else if ((cstr == "cwb-rins" || cstr == "cwb-rinx") && l > lt) {
    cout << FLBLUE << ss1.str()
         << " time computing well blocks (rins-wic): "
         << t << "ms  -  " << t / 1000.0 << "secs  -  "
         << t / 1000 / 60.0 << "mins" << AEND << endl;
  }
  else if (cstr == "cwb-pcg" && l > lt) {
    cout << FLBLUE << ss1.str()
         << " time computing well blocks (pcg-wic): "
         << t << "secs  -  "
         << t / 60.0 << "mins" << AEND << endl;
  }

}

// -----------------------------------------------------------------
void inline print_dbg_msg_wic_ri(string fstr, // function str
                                 string cstr, // comment str
                                 double t, // time var
                                 int l, // 0:NONE, 1:START, 2:END
                                 bool el = true, // print function header
                                 int cl = 1, // current level for dbg msg to print
                                 int tl = 2 // threshhold level for dbg msg to print
) {

  if (cl > tl) {

    stringstream ss0, ss1;
    if (el) {
      ss0 << "[wic-ri]" << fstr << ":";
      ss1 << setw(40) << left << setfill('-') << ss0.str();
    }

    if( l==0 ) {
      cout << FLYELLOW << ss1.str() << " " << cstr
           << AEND;
    }
    else if( l==1 ) {
      cout << FLYELLOW << ss1.str() << " " << cstr
           <<  "-- TSTART" << AEND;
    }
    else if( l==2 ) {
      cout << FLYELLOW << ss1.str() << " " << cstr
           <<  "-- TEND=" << t << "ms" << AEND;
    }

    if (el) cout << endl;


  }

}

#endif //FIELDOPT_DEBUG_HPP
