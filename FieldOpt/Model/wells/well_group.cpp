/***********************************************************
 Copyright (C) 2018-
 Mathias C. Bellout <mathias.bellout@ntnu.com>

 Created by bellout on 4/21/18.

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation, either version
 3 of the License, or (at your option) any later version.

 FieldOpt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty
 of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the
 GNU General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#include "well_group.h"
#include <iostream>

// ---------------------------------------------------------
namespace Model {
namespace WellGroups {

// ---------------------------------------------------------
using std::pair;
using std::vector;
using std::string;
using std::cout;
using std::endl;

// =========================================================
WellGroup::WellGroup(
    Settings::Model well_settings,
    int group_nr,
    ::Model::DrillingSequence drilling,
    ::Model::Properties::VariablePropertyContainer *variable_container,
    ::Reservoir::Grid::Grid *grid,
    RICaseData *ricasedata) {

  // -------------------------------------------------------
  // Select wells from that group; these wells are sorted
  // by drill sequence (which is given, i.e., non-variable)
  vector<pair<int, string>> wells_in_group_ =
      drilling.wseq_grpd_sorted_name[group_nr];

  // -------------------------------------------------------
  // Set up variable name (concatenate well names)
  string wname;
  for (int i = 0; i < wells_in_group_.size(); ++i)
    wname = wname + wells_in_group_[i].second;

  // -------------------------------------------------------
  QString gname = "WellGroup#" +
      QString::fromStdString(wname) +
      "#" + QString::number(group_nr);

  // -------------------------------------------------------
  // Set up variable corresponding to well group
  drill_seq_ = new ::Model::Properties::DiscreteProperty(
      drilling.drill_groups_[group_nr]);

  drill_seq_->setName(gname);
  variable_container->AddVariable(drill_seq_);

  // -------------------------------------------------------
  // Empty QList to contain wells in group
  group_wells_ = new QList<Wells::Well *>();

  // -------------------------------------------------------
  for (int wnr = 0; wnr < wells_in_group_.size(); ++wnr) {

    // -----------------------------------------------------
    string wn = wells_in_group_[wnr].second;
    int orig_wnum = drilling.name_vs_num[wn];

    // -----------------------------------------------------
    if (well_settings.verb_vector_[5] >= 1) // idx:5 -> mod
      cout << BCYAN << FBLUE << "Well=" << wn
           << "-------------------- " << AEND << endl;

    // -----------------------------------------------------
    auto well = new Wells::Well(well_settings,
                                orig_wnum,
                                variable_container,
                                grid,
                                ricasedata);
    group_wells_->append(well);

  }

}

}
}