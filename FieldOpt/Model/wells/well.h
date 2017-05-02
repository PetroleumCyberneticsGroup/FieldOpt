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

#ifndef WELL_H
#define WELL_H

#include "Settings/settings.h"
#include "Settings/model.h"
#include "Model/wells/wellbore/completions/completion.h"
#include "Model/wells/wellbore/completions/perforation.h"
#include "Model/properties/variable_property_container.h"
#include "Model/properties/discrete_property.h"
#include "Model/wells/control.h"
#include "Model/wells/wellbore/trajectory.h"
#include "Reservoir/grid/eclgrid.h"

#include <QList>

namespace Model {
namespace Wells {

/*!
 * \brief The Well class represents any well in the model.
 */
class Well
{
 public:
  /*!
   * \brief Well Default constructor.
   * \param settings The settings object to create a well from.
   * \param well_number The index of the sepcific well in the Model.Wells list to create a well from.
   * \param variables The variables object to add all new variable variables to.
   */
  Well(Settings::Model settings,
       int well_number,
       ::Model::Properties::VariablePropertyContainer *variable_container,
       ::Reservoir::Grid::Grid *grid);

  struct Heel { int i; int j; int k; };

  enum PreferredPhase { Oil, Gas, Water, Liquid };

  QString name() const { return name_; }
  ::Settings::Model::WellType type() const { return type_; }
  QString group() const { return group_; }
  bool IsProducer();
  bool IsInjector();
  ::Settings::Model::PreferredPhase preferred_phase() const { return preferred_phase_; }
  double wellbore_radius() const { return wellbore_radius_->value(); }
  Wellbore::Trajectory *trajectory() { return trajectory_; }
  QList<Control *> *controls() { return controls_; }
  int heel_i() const { return heel_.i; }
  int heel_j() const { return heel_.j; }
  int heel_k() const { return heel_.k; }
  void Update();
  int GetTimeSpentInWIC() const { return trajectory_->GetTimeSpentInWic(); }

 private:
  QString name_;
  ::Settings::Model::WellType type_;
  QString group_;
  ::Settings::Model::PreferredPhase preferred_phase_;
  Properties::ContinousProperty *wellbore_radius_;
  Wellbore::Trajectory *trajectory_;

  Heel heel_;
  QList<Control *> *controls_;
};

}
}

#endif // WELL_H
