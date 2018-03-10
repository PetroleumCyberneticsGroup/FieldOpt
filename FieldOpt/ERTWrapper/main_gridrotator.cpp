/******************************************************************************
   Copyright (C) 2017 Mathias Bellout [Created on 20170729]
   <mathias.bellout@ntnu.no, chakibbb@gmail.com>

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

#include "eclgridrotator.h"

using namespace std;

namespace ERTWrapper {
namespace ECLGrid {

}
}

int main(int argc, const char *argv[]) {
  ECLGridRotator ecl_grid_rotator = ECLGridRotator();
  ECLGridRotator *ecl_grid_rotator_p = &ecl_grid_rotator;

  ecl_grid_rotator_p->GetParametersFromJSON(argc, argv);
  ecl_grid_rotator_p->ecl_grid_reader_->ReadEclGrid(
      ecl_grid_rotator_p->ECL_GRID_FILE_PATH);

  cout << FCYAN << "GRID INFO:" << AEND << endl;
  ecl_grid_rotator_p->ecl_grid_reader_->GetGridSummary();

  ecl_grid_rotator_p->ecl_grid_reader_->GetCOORDZCORNData();
  ecl_grid_rotator_p->RotateCOORD();
  ecl_grid_rotator_p->RotateZCORN(true);

  ecl_grid_rotator_p->PrintCoordZcornData();

}

