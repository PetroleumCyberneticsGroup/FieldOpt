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
 A PARTICULAR PURPOSE. See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#include "wspln_interw_dist_anl.h"
#include "ConstraintMath/well_constraint_projections/well_constraint_projections.h"

// ---------------------------------------------------------
#include <boost/lexical_cast.hpp>
#include <cmath>

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// =========================================================
InterwellDistance::InterwellDistance(
    Settings::Optimizer::Constraint settings,
    Model::Properties::VariablePropertyContainer *variables) {

  // -------------------------------------------------------
  distance_ = settings.min;
  penalty_weight_ = settings.penalty_weight;

  // -------------------------------------------------------
  for (QString name : settings.wells) {

    // -----------------------------------------------------
    // Build affected well QList
    affected_wells_.append(
        initializeWell(variables->GetWellSplineVariables(name))
    );

  }

  // -------------------------------------------------------
  // Check if dealing with something else than a pair of wells
  if (affected_wells_.length() != 2) {
    throw std::runtime_error(
        "Currently, the Interwell Distance constraint "
            "must be applied to exactly two wells. Found "
            + boost::lexical_cast<std::string>(affected_wells_.length()));
  }
}

// =========================================================
bool InterwellDistance::CaseSatisfiesConstraint(Case *c) {

  QList<Eigen::Vector3d> points;

  // -------------------------------------------------------
  for (Well well : affected_wells_) {

    // -----------------------------------------------------
    double heel_x_val = c->real_variables()[well.heel.x];
    double heel_y_val = c->real_variables()[well.heel.y];
    double heel_z_val = c->real_variables()[well.heel.z];

    // -----------------------------------------------------
    double toe_x_val = c->real_variables()[well.toe.x];
    double toe_y_val = c->real_variables()[well.toe.y];
    double toe_z_val = c->real_variables()[well.toe.z];

    // -----------------------------------------------------
    Eigen::Vector3d heel_vals;
    Eigen::Vector3d toe_vals;

    // -----------------------------------------------------
    heel_vals << heel_x_val, heel_y_val, heel_z_val;
    toe_vals << toe_x_val, toe_y_val, toe_z_val;

    // -----------------------------------------------------
    points.append(heel_vals);
    points.append(toe_vals);

  }

  // -------------------------------------------------------
  // Get the projection
  QList<Eigen::Vector3d> projection =
      WellConstraintProjections::interwell_constraint_projection(
          points, distance_);

  // -------------------------------------------------------
  if (projection.length() == 0) return false; // No solution was found

  // -------------------------------------------------------
  // Check if the projection is (approximately)
  // equal to the input case
  for (int i = 0; i < projection.length(); ++i) {
    if (!points[i].isApprox(projection[i], 0.01))
      return false;
  }

  // -------------------------------------------------------
  return true;
}

// =========================================================
void InterwellDistance::SnapCaseToConstraints(Case *c) {

  QList<Eigen::Vector3d> points;

  // -------------------------------------------------------
  // Convert from well structure to vector
  for (Well well : affected_wells_) {

    // -----------------------------------------------------
    double heel_x_val = c->real_variables()[well.heel.x];
    double heel_y_val = c->real_variables()[well.heel.y];
    double heel_z_val = c->real_variables()[well.heel.z];

    // -----------------------------------------------------
    double toe_x_val = c->real_variables()[well.toe.x];
    double toe_y_val = c->real_variables()[well.toe.y];
    double toe_z_val = c->real_variables()[well.toe.z];

    // -----------------------------------------------------
    Eigen::Vector3d heel_vals;
    Eigen::Vector3d toe_vals;

    // -----------------------------------------------------
    heel_vals << heel_x_val, heel_y_val, heel_z_val;
    toe_vals << toe_x_val, toe_y_val, toe_z_val;

    // -----------------------------------------------------
    points.append(heel_vals);
    points.append(toe_vals);
  }

  // -------------------------------------------------------
  // Apply projection to vector of well coords
  QList<Eigen::Vector3d> projection =
      WellConstraintProjections::interwell_constraint_projection(
      points, distance_);

  // -------------------------------------------------------
  if (projection.length() == 0) return; // No solution was found

  // -------------------------------------------------------
  // Update projected values in well structure
  for (int i = 0; i < affected_wells_.length(); ++i) {

    // -----------------------------------------------------
    c->set_real_variable_value(
        affected_wells_[i].heel.x, projection[i*2](0));

    c->set_real_variable_value(
        affected_wells_[i].heel.y, projection[i*2](1));

    c->set_real_variable_value(
        affected_wells_[i].heel.z, projection[i*2](2));

    // -----------------------------------------------------
    c->set_real_variable_value(
        affected_wells_[i].toe.x, projection[i*2+1](0));

    c->set_real_variable_value(
        affected_wells_[i].toe.y, projection[i*2+1](1));

    c->set_real_variable_value(
        affected_wells_[i].toe.z, projection[i*2+1](2));
  }
}

// =========================================================
void
InterwellDistance::InitializeNormalizer(QList<Case *> cases) {

  long double minimum_distance = 1e20;

  // -------------------------------------------------------
  for (auto c : cases) {

    vector<double> endp_dist = endpointDistances(c);
    double min_dist = *min_element(endp_dist.begin(), endp_dist.end());
    if (min_dist < minimum_distance){
      minimum_distance = min_dist;
    }

  }

  // -------------------------------------------------------
  normalizer_.set_max(1.0L);
  normalizer_.set_steepness(1.0L/minimum_distance);
  normalizer_.set_midpoint(minimum_distance/2.0L);
}

// =========================================================
double InterwellDistance::Penalty(Case *c) {

  // -------------------------------------------------------
  vector<double> endpoint_distances = endpointDistances(c);
  double violation = 0.0;

  // -------------------------------------------------------
  for (auto distance : endpoint_distances) {
    if (distance < distance_) {
      violation += abs(distance - distance_);
    }
  }
  return violation;
}

// =========================================================
vector<double> InterwellDistance::endpointDistances(Case *c) {

  vector<double> endpoint_distances;

  // -------------------------------------------------------
  for (int i = 0; i < affected_wells_.size(); i += 2) { // Even numbers

    if (i >= affected_wells_.size()) break;

    for (int j = 1; j < affected_wells_.size(); j += 2) { // Odd numbers

      if (j >= affected_wells_.size()) break;
      QPair<Eigen::Vector3d, Eigen::Vector3d> well_i = GetEndpointValueVectors(c, affected_wells_[i]);
      QPair<Eigen::Vector3d, Eigen::Vector3d> well_j = GetEndpointValueVectors(c, affected_wells_[j]);
      endpoint_distances.push_back( (well_i.first  - well_j.first).norm()  ); // heel_i -> heel_j
      endpoint_distances.push_back( (well_i.second - well_j.second).norm() ); //  toe_i ->  toe_j
      endpoint_distances.push_back( (well_i.first  - well_j.second).norm() ); // heel_i ->  toe_j
      endpoint_distances.push_back( (well_i.second - well_j.first).norm()  ); //  toe_i -> heel_j

    } // end odd numbers

  } // end even numbers

  return endpoint_distances;
}

// =========================================================
long double InterwellDistance::PenaltyNormalized(Case *c) {
  return normalizer_.normalize(Penalty(c));
}

}
}
