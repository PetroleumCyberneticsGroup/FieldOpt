/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include <iostream>
#include "well.h"

// ---------------------------------------------------------
namespace Model {
namespace Wells {

// =========================================================
Well::Well(Settings::Model settings,
           int well_number,
           Properties::VariablePropertyContainer *variable_container,
           Reservoir::Grid::Grid *grid) {

  // -------------------------------------------------------
  Settings::Model::Well
      well_settings = settings.wells().at(well_number);
  well_settings.verb_vector_ = settings.verb_vector();

  // -------------------------------------------------------
  verb_vector_ = settings.verb_vector();
  if (verb_vector_[5] > 1) // idx:5 -> mod (Model)
    std::cout << FCYAN << "[mod]Reading well settings.-- "
              << AEND << std::endl;

  // -------------------------------------------------------
  grid_ = grid;

  // -------------------------------------------------------
  // Set name and type (prod/inj, well block/spline)
  name_ = well_settings.name;
  type_ = well_settings.type;
  deftype_ = well_settings.definition_type;

  // -------------------------------------------------------
  // Drilling dependent on well length
  // drilling_time_ = well_settings.drilling_time;

  // -------------------------------------------------------
  //drilling_order_ = well_settings.drilling_order;

  // -------------------------------------------------------
  if (well_settings.group.length() >= 1)
    group_ = well_settings.group;
  else group_ = "";

  // -------------------------------------------------------
  preferred_phase_ = well_settings.preferred_phase;
  wellbore_radius_ =
      new Properties::ContinousProperty(
          well_settings.wellbore_radius);

  // -------------------------------------------------------
  // Set trajectory
  trajectory_ = new Wellbore::Trajectory(well_settings,
                                         variable_container,
                                         grid);

  // -------------------------------------------------------
  // Set default drilling time
  drilling_order_ = well_settings.drilling_order;
  drilling_time_ = well_settings.drilling_time;

  // -------------------------------------------------------
  // Recompute drilling time dependent on well length
  ComputeDrillingTime(); // updates drilling_time_

  UpdateHeelToeIJK();

  // -------------------------------------------------------
  // Set controls
  controls_ = new QList<Control *>();
  for (int i = 0; i < well_settings.controls.size(); ++i) {

    // -----------------------------------------------------
    controls_->append(new Control(well_settings.controls[i],
                                  well_settings,
                                  variable_container));

  }

}

// ---------------------------------------------------------
void Well::UpdateHeelToeIJK() {

  // -------------------------------------------------------
  heel_.i = trajectory_->GetWellBlocks()->first()->i();
  heel_.j = trajectory_->GetWellBlocks()->first()->j();
  heel_.k = trajectory_->GetWellBlocks()->first()->k();

  toe_.i = trajectory_->GetWellBlocks()->last()->i();
  toe_.j = trajectory_->GetWellBlocks()->last()->j();
  toe_.k = trajectory_->GetWellBlocks()->last()->k();

  // -----------------------------------------------------
  if (verb_vector_[5] > 2) // idx:5 -> mod (Model)
    cout << "[mod]Heel/Toe IJK: --------- "
         << "Heel: I=" << heel_.i
         << " J=" << heel_.j
         << " K=" << heel_.k
         << " Toe: I=" << toe_.i
         << " J=" << toe_.j
         << " K=" << toe_.k
         << endl;
}

// ---------------------------------------------------------
void Well::ComputeDrillingTime() {

  if (drilling_time_ < 0) {

    // int testing = false;
    double dx = 0, dy = 0, ze = 0, dxdy = 0;

    // -----------------------------------------------------
    if (deftype_
        == Settings::Model::WellDefinitionType::WellBlocks) {

      Eigen::Vector3d heel_c =
          grid_->GetCell(heel_.i-1, heel_.j-1, heel_.k-1).center();

      Eigen::Vector3d toe_c =
          grid_->GetCell(toe_.i-1, toe_.j-1, toe_.k-1).center();

      dx = toe_c(0) - heel_c(0);
      dy = toe_c(1) - heel_c(1);
      ze = toe_c(2);

      // Drilling and completion time for vert.well at 2000m
      // depth: 30 days, cost: 10 million $.
      // if(testing) ze = 2000; dxdy = 0;

      // ---------------------------------------------------
    } else if (deftype_
        == Settings::Model::WellDefinitionType::WellSpline) {

      dx = trajectory_->GetWellSpline()->GetSplineDx();
      dy = trajectory_->GetWellSpline()->GetSplineDy();
      ze = trajectory_->GetWellSpline()->GetSplineZe();

      // Drilling and completion time for hz.well w/ end point
      // at 2000m depth and 2000m offset from the platform:
      // 70 days, cost: 30 million $.
      // if(testing) ze = 2000; dxdy = 2000;

    }

    // -----------------------------------------------------
    // Drilling time according to Olympus case formula
    // tD = 0.015 * Ze + 0.02 * |DXDY|
    // where
    // |DXDY| = sqrt( DX^2 + DY^2 )
    // with
    // DX, DY: hz. offset of well endpoint from kick-off point
    // Ze: z coord of well end point

    //if(!testing)
    dxdy = sqrt(pow(dx, 2.0) + pow(dy, 2.0));
    drilling_time_ = 0.015 * ze + 0.02 * dxdy;

    // -----------------------------------------------------
    if (verb_vector_[5] > 1) // idx:5 -> mod (Model)
      std::cout << "[mod]Drilling time for well: "
                << name_.toStdString() << " -> "
                << drilling_time_ << std::endl;

  }
}

// =========================================================
bool Well::IsProducer() {
  return type_ == ::Settings::Model::WellType::Producer;
}

// =========================================================
bool Well::IsInjector() {
  return type_ == ::Settings::Model::WellType::Injector;
}

// =========================================================
void Well::Update(int rank) {

  // -------------------------------------------------------
  if (verb_vector_[5] > 1) // idx:5 -> mod (Model)
    cout << FCYAN << "[mod]Updating: TRJ, IJK & DT. " << AEND << endl;

  // -------------------------------------------------------
  trajectory_->UpdateWellBlocks(rank);

  // -------------------------------------------------------
  UpdateHeelToeIJK();
  ComputeDrillingTime();

}


}
}
