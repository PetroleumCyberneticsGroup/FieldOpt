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

// -----------------------------------------------------------------
// STD
#include <iostream>
#include <fstream>
//#include <time.h>

// -----------------------------------------------------------------
// EIGEN
#include <Eigen/Core>

// -----------------------------------------------------------------
// Qt
#include <QtCore/QDateTime>

// -----------------------------------------------------------------
// FieldOpt: Utilities
#include "wellspline.h"
#include <Utilities/time.hpp>
#include <Utilities/debug.hpp>
#include <wells/well_exceptions.h>

// -----------------------------------------------------------------
// FieldOpt: WIC
//#include <FieldOpt-WellIndexCalculator/wicalc_rins.h>
//#include <FieldOpt-WellIndexCalculator/wicalc_rinx.h>
#include <FieldOpt-WellIndexCalculator/wicalc_rixx.h>

namespace Model {
namespace Wells {
namespace Wellbore {
using namespace Reservoir::WellIndexCalculation;

// -----------------------------------------------------------------
WellSpline::WellSpline(Settings::Model::Well well_settings,
                       Properties::VariablePropertyContainer *variable_container,
                       Reservoir::Grid::Grid *grid) {

  grid_ = grid;
  well_settings_ = well_settings;
  if (well_settings_.verb_vector_[5] > 1) // idx:5 -> mod (Model)
    std::cout << "[mod]Define well spline.----- " << std::endl;

  heel_x_ = new Model::Properties::ContinousProperty(well_settings.spline_heel.x);
  heel_y_ = new Model::Properties::ContinousProperty(well_settings.spline_heel.y);
  heel_z_ = new Model::Properties::ContinousProperty(well_settings.spline_heel.z);
  toe_x_ = new Model::Properties::ContinousProperty(well_settings.spline_toe.x);
  toe_y_ = new Model::Properties::ContinousProperty(well_settings.spline_toe.y);
  toe_z_ = new Model::Properties::ContinousProperty(well_settings.spline_toe.z);
  time_cwb_wic_pcg_ = 0;

  if (well_settings.spline_heel.is_variable) {
    heel_x_->setName(well_settings.spline_heel.name + "#x");
    heel_y_->setName(well_settings.spline_heel.name + "#y");
    heel_z_->setName(well_settings.spline_heel.name + "#z");
    variable_container->AddVariable(heel_x_);
    variable_container->AddVariable(heel_y_);
    variable_container->AddVariable(heel_z_);
  }
  if (well_settings.spline_toe.is_variable) {
    toe_x_->setName(well_settings.spline_toe.name + "#x");
    toe_y_->setName(well_settings.spline_toe.name + "#y");
    toe_z_->setName(well_settings.spline_toe.name + "#z");
    variable_container->AddVariable(toe_x_);
    variable_container->AddVariable(toe_y_);
    variable_container->AddVariable(toe_z_);
  }
}

// -----------------------------------------------------------------
QList<WellBlock *> *WellSpline::GetWellBlocks(int rank) {

  int lvl = well_settings_.verb_vector_[5];
  print_dbg_msg_wellspline(__func__, "gwb", 0.0, lvl, 1);

  auto heel = Eigen::Vector3d(heel_x_->value(),
                              heel_y_->value(),
                              heel_z_->value());
  auto toe = Eigen::Vector3d(toe_x_->value(),
                             toe_y_->value(),
                             toe_z_->value());

  // -----------------------------------------------------------------
  vector<WellDefinition> welldefs;
  welldefs.push_back(WellDefinition());
  welldefs[0].wellname = well_settings_.name.toStdString();
  welldefs[0].radii.push_back(well_settings_.wellbore_radius);
  welldefs[0].skins.push_back(0.0);
  welldefs[0].heels.push_back(heel);
  welldefs[0].toes.push_back(toe);
  welldefs[0].well_length.push_back(sqrt((toe - heel).norm()));
  welldefs[0].heel_md.push_back(heel(2));
  welldefs[0].toe_md.push_back(welldefs[0].heel_md.back() + welldefs[0].well_length.back());

  // ResInsight-based WIC --------------------------------------------
  auto start = QDateTime::currentDateTime();
  Reservoir::WellIndexCalculation::wicalc_rixx wicalc_rixx =
      Reservoir::WellIndexCalculation::wicalc_rixx(well_settings_, grid_);

  map<string, vector<IntersectedCell>> well_block_data_rixx;
  wicalc_rixx.ComputeWellBlocks(well_block_data_rixx, welldefs, rank);
  auto block_data_rixx = well_block_data_rixx[well_settings_.name.toStdString()];

  // Dbg file
  time_cwb_wic_rixx_ = time_span_msecs(start, QDateTime::currentDateTime());
  print_dbg_msg_wellspline(__func__, "cwb-rixx", time_cwb_wic_rixx_, lvl, 1);
  print_dbg_msg_wellspline_wic_coords(__func__, "wicalc_rixx.dbg", well_settings_,
                                      block_data_rixx, lvl, 1);


  // PCG WIC ---------------------------------------------------------
  start = QDateTime::currentDateTime();
  auto wic = WellIndexCalculator(grid_);

  map<string, vector<IntersectedCell>> well_block_data_pcg;
  wic.ComputeWellBlocks(well_block_data_pcg, welldefs, rank);
  auto block_data_pcg = well_block_data_pcg[well_settings_.name.toStdString()];

  // Dbg file
  time_cwb_wic_pcg_ = time_span_secs(start, QDateTime::currentDateTime());
  print_dbg_msg_wellspline(__func__, "cwb-pcg", time_cwb_wic_pcg_, lvl, 1);
  print_dbg_msg_wellspline_wic_coords(__func__, "wicalc_pcg.dbg", well_settings_,
                                      block_data_pcg, lvl, 1);

  // Collect: select b/e pcg or rins data --------------------------
  auto block_data = block_data_pcg;
//  auto block_data = block_data_rinx;
  QList<WellBlock *> *blocks = new QList<WellBlock *>();
  for (int i = 0; i < block_data.size(); ++i) {
    blocks->append(getWellBlock(block_data[i]));
  }

  if (blocks->size() == 0) {
    throw WellBlocksNotDefined("WIC could not compute.");
  }

  return blocks;
}

// -----------------------------------------------------------------
WellBlock *WellSpline::getWellBlock(IntersectedCell block_data) {

  auto wb = new WellBlock(block_data.ijk_index().i()+1,
                          block_data.ijk_index().j()+1,
                          block_data.ijk_index().k()+1);

  auto comp = new Completions::Perforation();
  comp->setTransmissibility_factor(block_data.cell_well_index_matrix());
  wb->AddCompletion(comp);

  return wb;
}

}
}
}
