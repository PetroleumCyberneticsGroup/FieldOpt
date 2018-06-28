/***********************************************************
 Copyright (C) 2018-
 Mathias C. Bellout <mathias.bellout@ntnu.com>

 Created by bellout on 4/23/18.

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
#ifndef FIELDOPT_OPTIMIZATION_H
#define FIELDOPT_OPTIMIZATION_H

// ---------------------------------------------------------
#include "settings.h"

// ---------------------------------------------------------
// QT / STD
#include <QString>
#include <QList>
#include <QJsonArray>
#include <assert.h>
#include <string>
#include <map>
#include <vector>

// ---------------------------------------------------------
using std::vector;

// ---------------------------------------------------------
namespace Settings {

// =========================================================
class Optimization {
  // ---------------------------------------------------------
  friend class Settings;

 public:
  // ---------------------------------------------------------
  Optimization(QJsonObject json_model);

  struct Optimizers {
    QString name;
  };

  struct VariableTypes {
    QString name;
  };

//  QJsonArray json_optimizers;
//  QJsonArray json_var_assignment;

  // -------------------------------------------------------
  QString output_directory_;
  vector<int> verb_vector_ = vector<int>(11,0);

};

}

#endif //FIELDOPT_OPTIMIZATION_H
