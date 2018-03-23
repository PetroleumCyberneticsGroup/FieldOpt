
// ---------------------------------------------------------------
#include "combined_spline_length_interwell_distance_reservoir_boundary.h"

// ---------------------------------------------------------------
// STD
#include <iostream>

// ---------------------------------------------------------------
namespace Optimization {
namespace Constraints {

// ---------------------------------------------------------------
CombinedSplineLengthInterwellDistanceReservoirBoundary
::CombinedSplineLengthInterwellDistanceReservoirBoundary(
    Settings::Optimizer::Constraint settings,
    Model::Properties::VariablePropertyContainer *variables,
    Reservoir::Grid::Grid *grid) {

  // -------------------------------------------------------------
  max_iterations_ = settings.max_iterations;
  Settings::Optimizer::Constraint dist_constr_settings;
  dist_constr_settings.wells = settings.wells;
  dist_constr_settings.min = settings.min_distance;

  // -------------------------------------------------------------
  distance_constraint_ = new InterwellDistance(dist_constr_settings,
                                               variables);
  Settings::Settings global_settings_;

  // -------------------------------------------------------------
  length_constraints_ = QList<WellSplineLength *>();
  for (QString wname : settings.wells) {
    Settings::Optimizer::Constraint len_constr_settings;
    len_constr_settings.well = wname;
    len_constr_settings.min = settings.min_length;
    len_constr_settings.max = settings.max_length;
    length_constraints_.
        append(new WellSplineLength(len_constr_settings,
                                    variables));

    // -----------------------------------------------------------
    Settings::Optimizer::Constraint boundary_constraint_settings;
    boundary_constraint_settings.box_imin = settings.box_imin;
    boundary_constraint_settings.box_imax = settings.box_imax;
    boundary_constraint_settings.box_jmin = settings.box_jmin;
    boundary_constraint_settings.box_jmax = settings.box_jmax;
    boundary_constraint_settings.box_kmin = settings.box_kmin;
    boundary_constraint_settings.box_kmax = settings.box_kmax;
    boundary_constraint_settings.well = wname;
    boundary_constraints_.
        append(new ReservoirBoundary(boundary_constraint_settings,
                                     variables,
                                     grid));
  }
}

// ---------------------------------------------------------------
bool CombinedSplineLengthInterwellDistanceReservoirBoundary::
CaseSatisfiesConstraint(Case *c) {

  // -------------------------------------------------------------
  if (!distance_constraint_->CaseSatisfiesConstraint(c))
    return false;

  // -------------------------------------------------------------
  for (WellSplineLength *wsl : length_constraints_) {
    if (!wsl->CaseSatisfiesConstraint(c))
      return false;
  }

  // -------------------------------------------------------------
  for (ReservoirBoundary *rb : boundary_constraints_) {
    if (!rb->CaseSatisfiesConstraint(c))
      return false;
  }

  return true;
}

// ---------------------------------------------------------------
void CombinedSplineLengthInterwellDistanceReservoirBoundary::
SnapCaseToConstraints(Case *c) {

  // -------------------------------------------------------------
  for (int i = 0; i < max_iterations_; ++i) {

    // -----------------------------------------------------------
    if (CaseSatisfiesConstraint(c)) {
      return;
    }
    else {
      distance_constraint_->SnapCaseToConstraints(c);

      // ---------------------------------------------------------
      for (WellSplineLength *wsl : length_constraints_) {
        wsl->SnapCaseToConstraints(c);
      }

      // -----------------------------------------------------------
      for (ReservoirBoundary *rb : boundary_constraints_) {
        rb->SnapCaseToConstraints(c);
      }

    }
  }
}

// ---------------------------------------------------------------
bool CombinedSplineLengthInterwellDistanceReservoirBoundary::
IsBoundConstraint() const {
  return true;
}

// ---------------------------------------------------------------
Eigen::VectorXd CombinedSplineLengthInterwellDistanceReservoirBoundary::
GetLowerBounds(QList<QUuid> id_vector) const {

  Eigen::VectorXd lbounds(id_vector.size());
  lbounds.fill(0);
  for (auto con : boundary_constraints_) {
    lbounds = lbounds + con->GetLowerBounds(id_vector);
  }
  return lbounds;
}

// ---------------------------------------------------------------
Eigen::VectorXd CombinedSplineLengthInterwellDistanceReservoirBoundary::
GetUpperBounds(QList<QUuid> id_vector) const {

  Eigen::VectorXd ubounds(id_vector.size());
  ubounds.fill(0);
  for (auto con : boundary_constraints_) {
    ubounds = ubounds + con->GetUpperBounds(id_vector);
  }
  return ubounds;
}

}
}
