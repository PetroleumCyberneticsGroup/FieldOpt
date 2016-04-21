#ifndef WELLSPLINELENGTH_H
#define WELLSPLINELENGTH_H

#include "constraint.h"
#include "well_spline_constraint.h"

namespace Optimization { namespace Constraints {

/*!
 * \brief The WellSplineLength class defines a constraint on the maximum and minimum length
 * of a well defined by a WellSpline. It uses the WellIndexCalculator library.
 */
class WellSplineLength : public Constraint, WellSplineConstraint
{
public:
    WellSplineLength(::Utilities::Settings::Optimizer::Constraint settings, ::Model::Properties::VariablePropertyContainer *variables);

    // Constraint interface
public:
    bool CaseSatisfiesConstraint(Case *c);
    void SnapCaseToConstraints(Case *c);

private:
    double min_length_;
    double max_length_;
    Well affected_well_;

};

}}

#endif // WELLSPLINELENGTH_H
