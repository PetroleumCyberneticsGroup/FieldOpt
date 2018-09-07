/******************************************************************************
   Created by einar on 8/23/18.
   Copyright (C) 2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#ifndef FIELDOPT_WELLDEFINITION_H
#define FIELDOPT_WELLDEFINITION_H

#include <string>
#include <vector>
#include <Eigen/Core>

using namespace std;
using namespace Eigen;

namespace Reservoir {
namespace WellIndexCalculation {


class WellDefinition {
 public:
  string wellname;
  vector<Vector3d> heels;
  vector<Vector3d> toes;
  vector<double> heel_md;
  vector<double> toe_md;
  vector<double> well_length;
  vector<double> radii;
  vector<double> skins;

 public:
  static void ReadWellsFromFile(string file_path,
                                vector<WellDefinition>& wells);
};

}
}
#endif //FIELDOPT_WELLDEFINITION_H
