#ifndef WELLSPLINELENGTH_H
#define WELLSPLINELENGTH_H

#include "constraint.h"
#include "well_spline_constraint.h"

namespace Optimization {
namespace Constraints {

/*!
 * \brief The WellSplineLength class defines a constraint on the maximum and minimum length
 * of a well defined by a WellSpline. It uses the WellIndexCalculation library.
 */
class WellSplineLength : public Constraint, WellSplineConstraint
{
 public:
  WellSplineLength(::Settings::Optimizer::Constraint settings, ::Model::Properties::VariablePropertyContainer *variables);
  string name() override { return "WellSplineLength"; };

  // Constraint interface
 public:
  bool CaseSatisfiesConstraint(Case *c);
  void SnapCaseToConstraints(Case *c);
  void InitializeNormalizer(QList<Case *> cases) override;
  double Penalty(Case *c) override;
  long double PenaltyNormalized(Case *c) override;

 private:
  double min_length_;
  double max_length_;
  Well affected_well_;

};

}
}

#endif // WELLSPLINELENGTH_H
