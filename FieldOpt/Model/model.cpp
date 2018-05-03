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
#include "model.h"
#include <boost/lexical_cast.hpp>
#include <FieldOpt-WellIndexCalculator/resinxx/rixx_prj_viz/RivIntersectionGeometryGenerator.h>

// ---------------------------------------------------------
using std::cout;
using std::endl;

// ---------------------------------------------------------
namespace Model {

// =========================================================
Model::Model(Settings::Model* settings, Logger *logger) {

  // -------------------------------------------------------
  settings_ = settings;

  // -------------------------------------------------------
  if (settings_->verb_vector()[5] > 1) { // idx:5 -> mod
    cout << BRED
         << "[mod]Building Model.---------" << AEND << endl;
    cout << fstr("[mod]Init ECLGrid_.",5) << endl;
  }

  // -------------------------------------------------------
  grid_ = new Reservoir::Grid::ECLGrid(
      settings_->reservoir().path.toStdString());

// -------------------------------------------------------
  RIReaderECL *rireaderecl = new RIReaderECL();
  ricasedata_ = new RICaseData(grid_->GetFilePath());
  rireaderecl->open(grid_->GetFilePathQString(), ricasedata_);

  // -------------------------------------------------------
  ricasedata_->computeActiveCellBoundingBoxes();

  rigridbase_ = ricasedata_->grid(0); // Same outer bb as mainGrid

  rigrid_ = ricasedata_->mainGrid();

  rigrid_->computeCachedData();
  // ricasedata_->mainGrid()->computeCachedData();

  rigrid_->calculateFaults(
      ricasedata_->activeCellInfo(MATRIX_MODEL));
  // ricasedata_->mainGrid()->calculateFaults(
  //    ricasedata_->activeCellInfo(MATRIX_MODEL));

  // -------------------------------------------------------------
  std::vector<cvf::Vec3d> ccv, ccc;
  size_t idx;
//  for (idx = 0; idx < ricasedata_->mainGrid()->cellCount(); idx++) {
  for (idx = 0; idx < 4; idx++) {

    size_t i, j, k;
    rigrid_->ijkFromCellIndex(idx, &i, &j, &k);
    // ricasedata_->mainGrid()->ijkFromCellIndex(idx, &i, &j, &k);
    cout << "i:" << i << " j:" << j << " k:" << k << endl;
    //cout << ricasedata_->mainGrid()->cellCentroid(idx).x();

    std::array<cvf::Vec3d, 8> hc;
    rigrid_->cellCornerVertices(idx, hc.data());
    // ricasedata_->mainGrid()->cellCornerVertices(idx, hc.data());
    cout << "hc_x:" << hc[0].x() << " hc_y:" << hc[0].y() << " hc_z:" << hc[0].z() << endl;
    ccc.push_back(hc[0]);

    cvf::Vec3d cc = rigrid_->cell(idx).center();
    // cvf::Vec3d cc = ricasedata_->mainGrid()->cell(idx).center();
    cout << "cc_x:" << cc.x() << " cc_y:" << cc.y() << " cc_z:" << cc.z() << endl;
    ccv.push_back(cc);

  }

  std::array<cvf::Vec3d, 8> hc;
  rigrid_->cellCornerVertices(0, hc.data());
  // ricasedata_->mainGrid()->cellCornerVertices(0, hc.data());
  ccc.push_back(hc[0]);

  // -------------------------------------------------------
  rimintersection_ = new RimIntersection(rigrid_,
                                         ricasedata_,
                                         settings);

//   95 25 -2005
//   5 55 -2005
//  cvf::Vec3d p1 = cvf::Vec3d(95, 25, -2005);
//  cvf::Vec3d p2 = cvf::Vec3d(5, 55, -2005);
//  rimintersection_->appendPointToPolyLine(p1);
//  rimintersection_->appendPointToPolyLine(p2);

//  95 35 -2005
//  35 5 -2005
//  5 35 -2005
//  35 95 -2005
//  95 35 -2005

  cvf::Vec3d p1 = cvf::Vec3d(95, 35, -2005);
  cvf::Vec3d p2 = cvf::Vec3d(35, 5, -2005);
  cvf::Vec3d p3 = cvf::Vec3d(5, 35, -2005);
  cvf::Vec3d p4 = cvf::Vec3d(35, 95, -2005);
  cvf::Vec3d p5 = cvf::Vec3d(95, 35, -2005);
  rimintersection_->appendPointToPolyLine(p1);
  rimintersection_->appendPointToPolyLine(p2);
  rimintersection_->appendPointToPolyLine(p3);
  rimintersection_->appendPointToPolyLine(p4);
  rimintersection_->appendPointToPolyLine(p5);

//  for (int ii=0; ii < 2; ++ii) {
//    cout << "Polypoint " << ii <<  ": ";
//    cout << ccc[ii].x() << " " << ccc[ii].y() << " " << ccc[ii].z() << endl;
//    // cout << "appendPointToPolyLine(ccc[" << ii << "])" << endl;
//    rimintersection_->appendPointToPolyLine(ccc[ii]);
//  }

  RivIntersectionPartMgr* imgr =
      rimintersection_->intersectionPartMgr();

  RivIntersectionGeometryGenerator*
      icsec = imgr->getCrossSectionGenerator();

  size_t vx_count = icsec->m_cellBorderLineVxes.p()->size();

  for (size_t ivx = 0; ivx < vx_count; ivx++) {
    auto vx_x = icsec->m_cellBorderLineVxes.p()->val(ivx).x();
    auto vx_y = icsec->m_cellBorderLineVxes.p()->val(ivx).y();
    auto vx_z = icsec->m_cellBorderLineVxes.p()->val(ivx).z();
    cout << "vx_x: " << vx_x << " "
         << "vx_y: " << vx_y << " "
         << "vx_z: " << vx_z << endl;
  }

  print_ri_hck_vec3f("", "", "", cvf::Vec3f::ZERO, true, false);
  for (size_t ivx = 0; ivx < vx_count; ivx++) {
    print_ri_hck_vec3f("", "", "",
                       icsec->m_cellBorderLineVxes.p()->val(ivx));
  }

  // -------------------------------------------------------
  if (settings->verb_vector()[5] > 1) // idx:5 -> mod (Model)
    cout << fstr("[mod]Init RIGrid_.",5) << "RICell& cell" << endl;

  size_t cellcount = rigrid_->cellCount();
  const RICell& cell = rigrid_->globalCellArray()[cellcount];
  // size_t cellcount = ricasedata_->mainGrid()->cellCount();
  // const RICell& cell = ricasedata_->mainGrid()->globalCellArray()[cellcount];

  cout << "volume:" << cell.volume() << " count:" << cellcount<< endl;

  cvf::Vec3d startp = rigrid_->cell(0).center();
  cvf::Vec3d endp = rigrid_->cell(cellcount-1).center();
  // cvf::Vec3d startp = ricasedata_->grid(0)->cell(0).center();
  // cvf::Vec3d endp = ricasedata_->grid(0)->cell(cellcount-1).center();

  cout << "x:" << startp.x() << " y:" << startp.y() << " z:" << startp.z() << endl;
  cout << "x:" << endp.x() << " y:" << endp.y() << " z:" << endp.z() << endl;

  // -------------------------------------------------------
  // BOUNDING BOX EXPERIMENTS

  cvf::BoundingBox bb;
  bb.add(startp);
  bb.add(endp);

  cout << fstr("[mod]bb.debugString()",5)
       << bb.debugString().toStdString() << endl;
  cout << fstr("bb.extent():")
       << show_Ved3d("", bb.extent()) << endl;

  // cvf::BoundingBox bbg = ricasedata_->mainGrid()->boundingBox();
  cvf::BoundingBox bbg = rigrid_->boundingBox();

  cout << fstr("[mod]bbg.debugString()",5)
       << bbg.debugString().toStdString() << endl;

  cout << fstr("bbg.extent():")
       << show_Ved3d("", bbg.extent()) << endl;

  // FIND TIGHT GRID BB
  // Find grid extremities
  auto gmincoord = rigrid_->minCoordinate();
  auto gmaxcoord = rigrid_->maxCoordinate();

  cvf::BoundingBox bbminmax = rigridbase_->boundingBox();
//  bbminmax.add(gmincoord);
//  bbminmax.add(gmaxcoord);

  cout << fstr("[mod]rigrid_->maxCoordinate()",5)
       << show_Ved3d("", gmaxcoord) << endl;

  cout << fstr("[mod]rigrid_->minCoordinate()",5)
       << show_Ved3d("", gmincoord) << endl;

  cout << fstr("[mod]bbminmax.debugString()",5)
       << bbminmax.debugString().toStdString() << endl;
  cout << fstr("bbminmax.extent():")
       << show_Ved3d("", bbminmax.extent()) << endl;

  for (size_t ii=0; ii < cellcount; ++ii) {

  }


  cvf::Vec3d cornerVerts[8];
  cvf::Vec3d tightboxmin, tightboxmax;

  rigrid_->cellCornerVertices(cellcount-1, cornerVerts);
  tightboxmax = cornerVerts[0];
  tightboxmin = cornerVerts[0];

  // Loop through each cell
  for (size_t i = 0; i < cellcount; i++) {
    rigrid_->cellCornerVertices(i, cornerVerts);

    // Loop through each vertex
    for (size_t j = 0; j < 8; j++) {

      // X-axis
      if (cornerVerts[j].x() < tightboxmin.x()) {
        tightboxmin.x() = cornerVerts[j].x();
      }

      if (cornerVerts[j].x() > tightboxmax.x()) {
        tightboxmax.x() = cornerVerts[j].x();
      }

      // Y-axis
      if (cornerVerts[j].y() < tightboxmin.y()) {
        tightboxmin.y() = cornerVerts[j].y();

      }

      if (cornerVerts[j].x() > tightboxmax.x()) {
        tightboxmax.y() = cornerVerts[j].y();
      }

      // Z-axis
      if (cornerVerts[j].z() < tightboxmin.z()) {
        tightboxmin.z() = cornerVerts[j].z();

      }

      if (cornerVerts[j].x() > tightboxmax.x()) {
        tightboxmax.z() = cornerVerts[j].z();
      }

    }
  }


  cvf::BoundingBox bbtight;
  bbtight.add(tightboxmin);
  bbtight.add(tightboxmax);

  cout << fstr("[mod]bbtight.debugString()",5)
       << bbtight.debugString().toStdString() << endl;
  cout << fstr("bbminmax.extent():")
       << show_Ved3d("", bbtight.extent()) << endl;

  stringstream istr;
  bbtight.cornerVertices(cornerVerts);
  for (int j=0; j < cornerVerts->length(); j++) {
    istr << "bbtight.cornerVertices[" << j << "]:";
    cout << fstr(istr.str())
         << show_Ved3d("", bbtight.extent()) << endl;
  }








  throw std::runtime_error("STOP EXP");




  // -------------------------------------------------------
  if (settings_->verb_vector()[5] > 1) // idx:5 -> mod
    cout << fstr("[mod]Init var container.",5) << endl;

  variable_container_ =
      new Properties::VariablePropertyContainer();

  // -------------------------------------------------------
  drilling_seq_ = new DrillingSequence();
  SetDrillingSeq(); // Establishes all fields in drilling_seq_
  // GetDrillingStr(); // Dbg

  // -------------------------------------------------------
  CreateWellGroups();

  // -------------------------------------------------------
  if (settings_->verb_vector()[5] > 1) // idx:5 -> mod
    cout << fstr("[mod]Setting up well_ QList.",5) << endl;

  // Set up regular well QList for subsequent calculations
  // that are group-independent -> this carries over the
  // order of wells set up in well groups
  wells_ = new QList<Wells::Well *>();
  for (WellGroups::WellGroup* group : *well_groups()) {
    for (Wells::Well* well : *group->group_wells()) {
      // Wells::Well* cp = well;
      wells_->append(well);
    }
  }

  // -------------------------------------------------------
  // Update main name_vs_time map and wseq_grpd_sorted_vs_time
  // aux vector in drilling_ object with drilling_times computed
  // at each well creation (wells_ must be set before this)
  UpdateNamevsTimeMap();

  // -------------------------------------------------------
  // Function: create cumulative drilling time steps based on
  // current drill sequence (implicitly given by well_ ordering)
  // + add these time steps for main control_time vector
  // + add these timesteps to each well, set time steps previous
  // to these equal to SHUT or CLOSE
  InsertDrillingTStep();
  GetDrillingStr(); // Dbg

  // -------------------------------------------------------
  variable_container_->CheckVariableNameUniqueness();
  logger_ = logger;
  logger_->AddEntry(new Summary(this));

// -------------------------------------------------------
  if (settings_->verb_vector()[5] > 1) // idx:5 -> mod
    cout << BRED
         << "[mod]Finished building Model." << AEND << endl;

}

// =========================================================
void Model::InsertDrillingTStep() {

  // -------------------------------------------------------
  if (settings_->verb_vector()[5] > 3) // idx:5 -> mod
    cout << fstr("[mod]InsertDrillingTStep().",5) << endl;

  // -------------------------------------------------------
  // Add control entry defining drill time
  for (Wells::Well *w : *wells()) {

    // -----------------------------------------------------
    if (settings_->verb_vector()[5] > 3) // idx:5 -> mod
      cout << fstr("[mod]Looping through wells.",5) << endl;

    // -----------------------------------------------------
    // Associate drilling time with new control
    double d_time_step =
        drilling_seq_->
            wseq_grpd_sorted_vs_tstep[w->name().toStdString()];

    // -----------------------------------------------------
    if (settings_->verb_vector()[5] > 3) // idx:5 -> mod
      cout << fstr("[mod]Make new control entry.",5) << endl;

    // -----------------------------------------------------
    // Careful with uninitialized variables in Settings::Model
    // Adjust Setting::Control
    ::Settings::Model::Well well_entry;
    well_entry = settings_->getWell(w->name());
    well_entry.verb_vector_ = settings_->verb_vector();

    ::Settings::Model::Well::ControlEntry
        control_entry = well_entry.controls[0];
    control_entry.time_step = d_time_step;

    // -----------------------------------------------------
    if (settings_->verb_vector()[5] > 3) // idx:5 -> mod
      cout << fstr("[mod]Make new control step.",5) << endl;

    // -----------------------------------------------------
    // Make Well::Control
    Wells::Control *d_control_tstep =
        new Wells::Control(control_entry,
                           well_entry,
                           variable_container_);

    // -----------------------------------------------------
    if (settings_->verb_vector()[5] > 3) // idx:5 -> mod
      cout << fstr("[mod]Append to control list.",5) << endl;

    // -----------------------------------------------------
    // Append control to list of well controls
    w->controls()->append(d_control_tstep);

    // -----------------------------------------------------
    // Shut down wells for all control steps prior
    // to drilling step
    for (Wells::Control *c : *w->controls()) {
      if(c->time_step() < d_control_tstep->time_step()) {
        c->setOpen(false);
      }
    }

    // -----------------------------------------------------
    // Add drilling tstep to overall control tstep vector
    settings_->control_times().append(d_time_step);
  }

  // -------------------------------------------------------
  // Sort overall control tstep vector
  settings_->sort_control_steps();

  // -------------------------------------------------------
  if (settings_->verb_vector()[5] > 3) // idx:5 -> mod
    cout << fstr("[mod]InsertDrillingTStep():",5)
         << "Done." << endl;

}

// =========================================================
void Model::CreateWellGroups() {

  // -------------------------------------------------------
  // Empty QList to contain groups of wells
  well_groups_ = new QList<WellGroups::WellGroup *>();
  for (int gnr = 0; gnr < drilling_seq_->drill_groups_.size(); ++gnr) {

    // -----------------------------------------------------
    if (settings_->verb_vector()[5] >= 1) { // idx:5 -> mod
      cout << endl << BCYAN << FRED << "Group=" << gnr
           << "----------------------" << AEND << endl;
    }

    // -----------------------------------------------------
    well_groups_->append(
        new WellGroups::WellGroup(*settings_,
                                  gnr,
                                  *drilling_seq_,
                                  variable_container_,
                                  grid_,
                                  ricasedata_));
  }
}

// =========================================================
void Model::Finalize() {

  logger_->AddEntry(this); // Removing this causes
  // the last case to not be in the JSON file
  logger_->AddEntry(new Summary(this));
}

// =========================================================
void Model::ApplyCase(Optimization::Case *c, int rank) {

  // -------------------------------------------------------
  if (settings_->verb_vector()[5] > 1) // idx:5 -> mod (Model)
    cout << FCYAN << "[mod]Applying case.---------- " << AEND << endl;

  // -------------------------------------------------------
  for (QUuid key : c->binary_variables().keys()) {
    variable_container_->SetBinaryVariableValue(
        key, c->binary_variables()[key]);
  }

  // -------------------------------------------------------
  for (QUuid key : c->integer_variables().keys()) {
    variable_container_->SetDiscreteVariableValue(
        key, c->integer_variables()[key]);
  }

  // -------------------------------------------------------
  for (QUuid key : c->real_variables().keys()) {
    variable_container_->SetContinousVariableValue(
        key, c->real_variables()[key]);
  }

  // -------------------------------------------------------
  if (settings_->verb_vector()[5] > 1) // idx:5 -> mod (Model)
    cout << FCYAN << "[mod]Updating wells.--------- " << AEND << endl;

  int cumulative_wic_time = 0;
  for (Wells::Well *w : *wells()) {
    w->Update(rank);
    cumulative_wic_time += w->GetTimeSpentInWIC();
  }

  // -------------------------------------------------------
  c->SetWICTime(cumulative_wic_time);
  verify();

  // -------------------------------------------------------
  // Notify the logger, and after that clear the results.
  // First check that we have results (if not, this is the
  // first evaluation, and we have nothing to notify the
  // logger about).
  if (results_.size() > 0){
    logger_->AddEntry(this);
  }

  // -------------------------------------------------------
  current_case_id_ = c->id();
  results_.clear();
}

// =========================================================
void Model::verify() {
  verifyWells();
}

// =========================================================
void Model::verifyWells() {
  for (Wells::Well *w : *wells()) {
    verifyWellTrajectory(w);
  }
}

// =========================================================
void Model::verifyWellTrajectory(Wells::Well *w) {

  // -------------------------------------------------------
  for (Wells::Wellbore::WellBlock
        *wb : *w->trajectory()->GetWellBlocks()) {
    verifyWellBlock(wb);
  }
}

// =========================================================
void Model::verifyWellBlock(Wells::Wellbore::WellBlock *wb) {

  // -------------------------------------------------------
  if (wb->i() < 1 || wb->i() > grid()->Dimensions().nx ||
      wb->j() < 1 || wb->j() > grid()->Dimensions().ny ||
      wb->k() < 1 || wb->k() > grid()->Dimensions().nz)
    throw std::runtime_error(
        "Invalid well block detected: ("
            + boost::lexical_cast<std::string>(wb->i()) + ", "
            + boost::lexical_cast<std::string>(wb->j()) + ", "
            + boost::lexical_cast<std::string>(wb->k()) + ")"
    );
}

// =========================================================
void Model::SetResult(const std::string key,
                      std::vector<double> vec) {
  results_[key] = vec;
}

// =========================================================
Loggable::LogTarget Model::GetLogTarget() {
  return Loggable::LogTarget::LOG_EXTENDED;
}

// =========================================================
map<string, string> Model::GetState() {
  map<string, string> statemap;
  statemap["COMPDAT"] = compdat_.toStdString();
  return statemap;
}

// =========================================================
QUuid Model::GetId() {
  return current_case_id_;
}

// =========================================================
map<string, vector<double>> Model::GetValues() {

  // -------------------------------------------------------
  map<string, vector<double>> valmap;
  for (auto const item : results_) {
    valmap["Res#"+item.first] = item.second;
  }

  // -------------------------------------------------------
  for (auto const var :
      variable_container_->GetContinousVariables()->values()) {
    valmap["Var#"+var->name().toStdString()] =
        vector<double>{var->value()};
  }

  // -------------------------------------------------------
  for (auto const var :
      variable_container_->GetDiscreteVariables()->values()) {
    valmap["Var#"+var->name().toStdString()] =
        vector<double>{var->value()};
  }

  // -------------------------------------------------------
  for (auto const var :
      variable_container_->GetBinaryVariables()->values()) {
    valmap["Var#"+var->name().toStdString()] =
        vector<double>{var->value()};
  }

  return valmap;
}

// =========================================================
Loggable::LogTarget Model::Summary::GetLogTarget() {
  return LOG_SUMMARY;
}

// =========================================================
map<string, string> Model::Summary::GetState() {
  map<string, string> statemap;
  statemap["compdat"] = model_->compdat_.toStdString();
  return statemap;
}

// =========================================================
QUuid Model::Summary::GetId() {
  return nullptr;
}

// =========================================================
map<string, vector<double>> Model::Summary::GetValues() {
  map<string, vector<double>> valmap;
  return valmap;
}

// =========================================================
map<string, Loggable::WellDescription>
Model::Summary::GetWellDescriptions() {

  // -------------------------------------------------------
  map<string, Loggable::WellDescription> wellmap;

  // -------------------------------------------------------
  for (auto well : *model_->wells()) {

    // -----------------------------------------------------
    Loggable::WellDescription wdesc;
    wdesc.name = well->name().toStdString();
    wdesc.group = well->group().toStdString();
    wdesc.wellbore_radius =
        boost::lexical_cast<string>(well->wellbore_radius());
    wdesc.type = well->IsProducer() ? "Producer" : "Injector";

    // -----------------------------------------------------
    switch (well->preferred_phase()) {
      case Settings::Model::PreferredPhase::
        Oil:wdesc.pref_phase = "Oil";
        break;

      case Settings::Model::PreferredPhase::
        Gas:wdesc.pref_phase = "Gas";
        break;

      case Settings::Model::PreferredPhase::
        Water:wdesc.pref_phase = "Water";
        break;

      case Settings::Model::PreferredPhase::
        Liquid:wdesc.pref_phase = "Liquid";
        break;
    }

    // -----------------------------------------------------
    // Spline
    if (model_->variables()->
        GetWellSplineVariables(well->name()).size() > 0) {
      wdesc.def_type = "Spline";

      // ---------------------------------------------------
      for (auto prop :
          model_->variables()->
              GetWellSplineVariables(well->name())) {

        // -------------------------------------------------
        if (prop->propertyInfo().spline_end ==
            Properties::Property::SplineEnd::Heel) {

          switch (prop->propertyInfo().coord) {
            case Properties::Property::Coordinate::x:
              wdesc.spline.heel_x = prop->value();
              break;

            case Properties::Property::Coordinate::y:
              wdesc.spline.heel_y = prop->value();
              break;

            case Properties::Property::Coordinate::z:
              wdesc.spline.heel_z = prop->value();
              break;
          }
        } else {
          switch (prop->propertyInfo().coord) {
            case Properties::Property::Coordinate::x:
              wdesc.spline.toe_x = prop->value();
              break;

            case Properties::Property::Coordinate::y:
              wdesc.spline.toe_y = prop->value();
              break;

            case Properties::Property::Coordinate::z:
              wdesc.spline.toe_z = prop->value();
              break;
          }
        }
      }

    } else {
      wdesc.def_type = "Blocks";
    }

    // -----------------------------------------------------
    // Controls
    for (Wells::Control *cont : *well->controls()) {
      Loggable::ControlDescription cd;
      if (cont->mode() == Settings::Model::ControlMode::RateControl) {
        cd.control = "Rate";
        cd.value = cont->rate();
      } else {
        cd.control = "BHP";
        cd.value = cont->bhp();
      }
      cd.state = cont->open() ? "Open" : "Shut";
      cd.time_step = cont->time_step();
      wdesc.controls.push_back(cd);
    }
    wellmap[well->name().toStdString()] = wdesc;
  }

  return wellmap;
}

// =========================================================
void Model::GetDrillingStr() {

  // -------------------------------------------------------
  string str_out = "[dbg]GetDrillingStr          ";
  cout << "\n" << BLDON << str_out << AEND << "\n"
       << string(str_out.length(), '-') << endl;

  // -------------------------------------------------------
  for(int i=0; i < drilling_seq_->drill_groups_.size(); ++i) {

    cout << "Group: [ "
         << drilling_seq_->drill_groups_[i] << " ]" << endl;

  }

  // -------------------------------------------------------
  auto seq_vec_group = drilling_seq_->wseq_grpd_sorted_name;
  for(int i=0; i < seq_vec_group.size(); ++i) {

    for (int j=0; j<seq_vec_group[i].size(); ++j) {

      cout << "IntSeq: [ "
           << seq_vec_group[i][j].first << " -- "
           << seq_vec_group[i][j].second << " ]"
           << endl;

    }

  }

  // -------------------------------------------------------
  auto seq_vec_time = drilling_seq_->wseq_grpd_sorted_vs_time;
  for(int i=0; i < seq_vec_time.size(); ++i) {

    cout << "TimeSeq: [ "
         << seq_vec_time[i].first << " -- "
         << setprecision(3) << fixed
         << seq_vec_time[i].second << " ]"
         << endl;

  }

  // -------------------------------------------------------
  cout << "ControlTimes: [ ";
  for(int i=0; i < settings_->control_times().size(); ++i) {
    cout << fixed << setprecision(1)
         << settings_->control_times()[i] << "  ";
  }

  cout << "]" << endl << endl;
}

// =========================================================
void Model::SetDrillingSeq() {

  // -------------------------------------------------------
  if (settings_->verb_vector()[5] >= 2) // idx:5 -> mod
    cout << "[mod]Computing drilling seq.- " << endl;
  drilling_seq_->mode = settings_->drillingMode_;

  // -------------------------------------------------------
  // Main drilling name_vs_order maps (pairs)
  if (settings_->verb_vector()[5] >= 4) // idx:5 -> mod
    cout << "[mod]DrillSeq: set up maps.-- " << endl;

  // -------------------------------------------------------
  for (int i = 0; i < settings_->wells().size(); ++i) {

    // -----------------------------------------------------
    pair<string, pair<int, int>>
        well_order(settings_->wells().at(i).name.toStdString(),
                   settings_->wells().at(i).GetDrillingOrder());
    drilling_seq_->name_vs_order.push_back(well_order);

    // -----------------------------------------------------
    drilling_seq_->name_vs_time.emplace(
        settings_->wells().at(i).name.toStdString(),
        settings_->wells().at(i).GetDrillingTime());

    // -----------------------------------------------------
    drilling_seq_->name_vs_num
    [settings_->wells().at(i).name.toStdString()] = i;

  }

  // -------------------------------------------------------
  // Use multimap to group well pairs (<int, pair<int, string>>)
  // into groups
  if (settings_->verb_vector()[5] >= 4) // idx:5 -> mod
    cout << "[mod]DrillSeq: multimap.----- " << endl;

  // -------------------------------------------------------
  std::set<int> groups;
  for (int i = 0; i < drilling_seq_->name_vs_order.size(); ++i) {

    // -----------------------------------------------------
    pair<int, string> p(drilling_seq_->name_vs_order[i].second.second,
                        drilling_seq_->name_vs_order[i].first);

    // -----------------------------------------------------
    drilling_seq_->mp_wells_into_groups.emplace(
        drilling_seq_->name_vs_order[i].second.first, p);

    // -----------------------------------------------------
    groups.emplace(drilling_seq_->name_vs_order[i].second.first);

  }

  // -------------------------------------------------------
  drilling_seq_->drill_groups_ =
      std::vector<int> (groups.begin(), groups.end());

  // -------------------------------------------------------
  // Order each multimap group into a (pair<int, string>)
  // vector with wells within each group sorted
  decltype(drilling_seq_->mp_wells_into_groups.equal_range(int())) range;

  // -----------------------------------------------------
  // Find equal ranges in multimap
  if (settings_->verb_vector()[5] >= 4) // idx:5 -> mod
    cout << "[mod]DrillSeq: find ranges.-- " << endl;

  // -------------------------------------------------------
  for (auto i = drilling_seq_->mp_wells_into_groups.begin();
       i != drilling_seq_->mp_wells_into_groups.end(); i = range.second) {

    range = drilling_seq_->mp_wells_into_groups.equal_range(i->first);

    // ---------------------------------------------------
    // Insert each well in group into vector
    vector<pair<int, string>> group_vec;
    for(auto d = range.first; d != range.second; ++d) {

      // -------------------------------------------------
      // Assemble well pair
      pair<int, string> well_pair(d->second.first,
                                  d->second.second);
      group_vec.push_back(well_pair);
    }

    // ---------------------------------------------------
    // Sort group vector
    sort(group_vec.begin(), group_vec.end());

    // ---------------------------------------------------
    drilling_seq_->wseq_grpd_sorted_name.push_back(group_vec);
  }

  // -------------------------------------------------------
  if (settings_->verb_vector()[5] >= 4) // idx:5 -> mod
    cout << "[mod]DrillSeq: set timevec.-- " << endl;

  SetDrillTimeVec();

}

// =========================================================
void Model::SetDrillTimeVec() {

  // -------------------------------------------------------
  if (settings_->verb_vector()[5] > 3) // idx:5 -> mod
    cout << "[mod]SetDrillTimeVec().------ " << endl;

  drilling_seq_->wseq_grpd_sorted_vs_time.clear();
  double d_tstep = 0;

  // -------------------------------------------------------
  for( int i=0; i < drilling_seq_->wseq_grpd_sorted_name.size(); ++i ) {

    for (int j=0; j< drilling_seq_->wseq_grpd_sorted_name[i].size(); j++) {

      string wn = drilling_seq_->wseq_grpd_sorted_name[i][j].second;

      // ---------------------------------------------------
      pair<string, double>
          p(wn,
            drilling_seq_->name_vs_time.find(wn)->second);
      drilling_seq_->wseq_grpd_sorted_vs_time.push_back(p);

      // ---------------------------------------------------
      d_tstep = d_tstep + drilling_seq_->name_vs_time.find(wn)->second;
      // pair<string, double> c(wn, d_tstep);
      // drilling_seq_->wseq_grpd_sorted_vs_time_cum.push_back(c);
      drilling_seq_->wseq_grpd_sorted_vs_tstep[wn] = d_tstep;

    }

  }
}

// =========================================================
void Model::UpdateNamevsTimeMap() {

  // -------------------------------------------------------
  if (settings_->verb_vector()[5] > 3) // idx:5 -> mod
    cout << "[mod]UpdateNamevsTimeMap().-- " << endl;

  drilling_seq_->name_vs_time.clear();

  // -------------------------------------------------------
  for (Wells::Well *w : *wells()) {
    drilling_seq_->name_vs_time.emplace(
        w->name().toStdString(),
        w->GetDrillingTime());
  }

  // -------------------------------------------------------
  SetDrillTimeVec();

}

}
