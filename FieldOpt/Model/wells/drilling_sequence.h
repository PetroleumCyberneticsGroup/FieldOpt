/***********************************************************
 Copyright (C) 2018-
 Mathias C. Bellout <mathias.bellout@ntnu.com>

 Created by bellout on 4/25/18.

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

#ifndef FIELDOPT_DRILLING_SEQUENCE_H
#define FIELDOPT_DRILLING_SEQUENCE_H

// ---------------------------------------------------------
// QT / STD
//#include <QString>
//#include <QList>
//#include <iomanip>
#include <vector>
#include <map>

// ---------------------------------------------------------
#include "Settings/model.h"

// ---------------------------------------------------------
namespace Model {

// ---------------------------------------------------------
using std::map;
using std::vector;
using std::multimap;
using std::pair;
using std::string;

// =========================================================
/*!
 * \brief Collects data maps used to define drilling sequence
 * for all wells
 */
class DrillingSequence {

 public:
  DrillingSequence() {};

  // -------------------------------------------------------
  Settings::Model::DrillingMode mode;

  // -------------------------------------------------------
  // Main orderings
  vector<pair<string, pair<int, int>>> name_vs_order;
  map<string, int> name_vs_num;
  multimap<string, double> name_vs_time;

  // -------------------------------------------------------
  // Transformation map
  multimap<int, pair<int, string>> mp_wells_into_groups;

  // -------------------------------------------------------
  // Resulting vecotrs
  vector<vector<pair<int, string>>> wseq_grpd_sorted_name;

  // -------------------------------------------------------
  // Resulting aux vectors
  vector<int> drill_groups_;
  vector<pair<string, double>> wseq_grpd_sorted_vs_time;
  map<string, double> wseq_grpd_sorted_vs_tstep;

};

}

#endif //FIELDOPT_DRILLING_SEQUENCE_H
