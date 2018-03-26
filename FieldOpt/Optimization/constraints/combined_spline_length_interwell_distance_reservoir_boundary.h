#ifndef COMBINED_SPLINE_LENGTH_INTERWELL_DISTANCE_RESERVOIR_BOUNDARY_H
#define COMBINED_SPLINE_LENGTH_INTERWELL_DISTANCE_RESERVOIR_BOUNDARY_H

// ---------------------------------------------------------
#include "constraint.h"
#include "well_spline_length.h"
#include "interwell_distance.h"
#include "reservoir_boundary.h"

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// ---------------------------------------------------------
/*!
 * \brief
 * CombinedSplineLengthInterwellDistanceReservoirBoundary
 * class checks ReservoirBoundary constraint, WellSplineLength
 * and InterwellDistance constraints in a sequential manner
 * within a loop.
 *
 * The constraints are enforced until all are satisfied or
 * until a maximum number of iterations is reached. The class
 * instantiates one well length constraint per well, _one_
 * distance constraint and _one_ reservoir boundary constraint.
 */

// ---------------------------------------------------------
class CombinedSplineLengthInterwellDistanceReservoirBoundary :
    public Constraint {

 public:
  CombinedSplineLengthInterwellDistanceReservoirBoundary(
      Settings::Optimizer::Constraint settings,
      Model::Properties::VariablePropertyContainer *variables,
      Reservoir::Grid::Grid *grid);

  // -------------------------------------------------------
  bool IsBoundConstraint() const override;
  Eigen::VectorXd GetLowerBounds(QList<QUuid> id_vector) const override;
  Eigen::VectorXd GetUpperBounds(QList<QUuid> id_vector) const override;

  // -------------------------------------------------------
  string name() override { return
        "CombinedSplineLengthInterwellDistanceReservoirBoundary"; }

  // -------------------------------------------------------
  // Constraint interface

 public:
  bool CaseSatisfiesConstraint(Case *c);
  void SnapCaseToConstraints(Case *c);

 private:
  // -------------------------------------------------------
  int max_iterations_;
  QList<WellSplineLength *> length_constraints_;
  QList<ReservoirBoundary *> boundary_constraints_;
  InterwellDistance *distance_constraint_;
};

}
}

#endif // COMBINED_SPLINE_LENGTH_INTERWELL_DISTANCE_RESERVOIR_BOUNDARY_H
