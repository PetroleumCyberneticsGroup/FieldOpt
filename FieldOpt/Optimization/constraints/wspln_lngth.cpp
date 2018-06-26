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
#include "Utilities/math.hpp"
#include "wspln_lngth.h"
#include "ConstraintMath/well_constraint_projections/well_constraint_projections.h"

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// =========================================================
WellSplineLength::WellSplineLength(
    Settings::Optimizer::Constraint &settings,
    Model::Properties::VariablePropertyContainer *variables) {

  // -------------------------------------------------------
  min_length_ = settings.min;
  max_length_ = settings.max;
  penalty_weight_ = settings.penalty_weight;

  // -------------------------------------------------------
  affected_well_ =
      initializeWell(variables->GetWellSplineVariables(settings.well));

  // -------------------------------------------------------
  if (verbosity_level_>2) {
    std::cout << "... ... initialized length constraint for well: "
              << settings.well.toStdString() << std::endl;
  }
}

// =========================================================
bool WellSplineLength::CaseSatisfiesConstraint(Case *c) {

  // -------------------------------------------------------
  double heel_x_val = c->real_variables()[affected_well_.heel.x];
  double heel_y_val = c->real_variables()[affected_well_.heel.y];
  double heel_z_val = c->real_variables()[affected_well_.heel.z];

  // -------------------------------------------------------
  double toe_x_val = c->real_variables()[affected_well_.toe.x];
  double toe_y_val = c->real_variables()[affected_well_.toe.y];
  double toe_z_val = c->real_variables()[affected_well_.toe.z];

  // -------------------------------------------------------
  Eigen::Vector3d heel_vals;
  Eigen::Vector3d toe_vals;

  // -------------------------------------------------------
  heel_vals << heel_x_val, heel_y_val, heel_z_val;
  toe_vals << toe_x_val, toe_y_val, toe_z_val;

  // -------------------------------------------------------
  QList<Eigen::Vector3d> projection =
      WellConstraintProjections::well_length_projection(
          heel_vals, toe_vals, max_length_, min_length_, 0.001);

  // -------------------------------------------------------
  return heel_vals.isApprox(projection.first(), 0.01)
      && toe_vals.isApprox(projection.last(), 0.01);
}

// =========================================================
void WellSplineLength::SnapCaseToConstraints(Case *c) {

  // -------------------------------------------------------
  double heel_x_val = c->real_variables()[affected_well_.heel.x];
  double heel_y_val = c->real_variables()[affected_well_.heel.y];
  double heel_z_val = c->real_variables()[affected_well_.heel.z];

  // -------------------------------------------------------
  double toe_x_val = c->real_variables()[affected_well_.toe.x];
  double toe_y_val = c->real_variables()[affected_well_.toe.y];
  double toe_z_val = c->real_variables()[affected_well_.toe.z];

  // -------------------------------------------------------
  Eigen::Vector3d heel_vals;
  Eigen::Vector3d toe_vals;

  // -------------------------------------------------------
  heel_vals << heel_x_val, heel_y_val, heel_z_val;
  toe_vals << toe_x_val, toe_y_val, toe_z_val;

  // -------------------------------------------------------
  QList<Eigen::Vector3d> projection =
      WellConstraintProjections::well_length_projection(
          heel_vals, toe_vals, max_length_, min_length_, 0.001);

  // -------------------------------------------------------
  c->set_real_variable_value(
      affected_well_.heel.x, projection.first()(0));
  c->set_real_variable_value(
      affected_well_.heel.y, projection.first()(1));
  c->set_real_variable_value(
      affected_well_.heel.z, projection.first()(2));

  // -------------------------------------------------------
  c->set_real_variable_value(
      affected_well_.toe.x, projection.last()(0));
  c->set_real_variable_value(
      affected_well_.toe.y, projection.last()(1));
  c->set_real_variable_value(
      affected_well_.toe.z, projection.last()(2));
}

// =========================================================
void WellSplineLength::InitializeNormalizer(QList<Case *> cases) {

  // -------------------------------------------------------
  vector<double> well_lengths;
  for (auto c : cases) {
    auto endpts = GetEndpointValueVectors(c, affected_well_);
    well_lengths.push_back( (endpts.first - endpts.second).norm() );
  }

  // -------------------------------------------------------
  long double med_well_length = calc_median(well_lengths);
  normalizer_.set_max(1.0L);
  normalizer_.set_midpoint(med_well_length);
  normalizer_.set_steepness(1.0L / (max_length_ - min_length_));
}

// =========================================================
double WellSplineLength::Penalty(Case *c) {

  // -------------------------------------------------------
  auto endpts = GetEndpointValueVectors(c, affected_well_);
  double well_length =  (endpts.first - endpts.second).norm();

  // -------------------------------------------------------
  if (well_length > max_length_)
    return well_length - max_length_;
  else if (well_length < min_length_)
    return min_length_ - well_length;
  else
    return 0.0;
}

// =========================================================
long double WellSplineLength::PenaltyNormalized(Case *c) {
  return normalizer_.normalize(Penalty(c));
}

}
}
