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
#ifndef FIELDOPT_ADG_CONSTRAINT_H
#define FIELDOPT_ADG_CONSTRAINT_H

// ---------------------------------------------------------

//==========================================================
class ADGConstraint : public Constraint {
 public:

  // -------------------------------------------------------
  // Constructor
  ADGConstraint(Settings::Optimizer* settings,
  Model::Properties::VariablePropertyContainer *variables,
      ::Reservoir::Grid::Grid *grid,
  RICaseData *ricasedata);

  // -------------------------------------------------------
  string name() override { return "ADGConstraint"; }

  // -------------------------------------------------------
  // Input var to ADGPRS Optimizer
  Settings::Optimizer *settings_;
  Model::Properties::VariablePropertyContainer *variables_;
  Case *current_case_;

};

#endif //FIELDOPT_ADG_CONSTRAINT_H
