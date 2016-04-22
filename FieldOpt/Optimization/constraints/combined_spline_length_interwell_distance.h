#ifndef COMBINEDSPLINELENGTHINTERWELLDISTANCE_H
#define COMBINEDSPLINELENGTHINTERWELLDISTANCE_H

#include "constraint.h"
#include "well_spline_length.h"
#include "interwell_distance.h"

namespace Optimization { namespace Constraints {

/*!
 * \brief The CombinedSplineLengthInterwellDistance class combines the WellSplineLength and
 * InterwellDistance constraints. It instantiates one well length constraint per well and
 * _one_ distance constraint. All the constraints are applied cyclically in a loop untill
 * either all constraints are satisfied or the maximum number of iterations is reached.
 */
class CombinedSplineLengthInterwellDistance : public Constraint
{
public:
    CombinedSplineLengthInterwellDistance(Utilities::Settings::Optimizer::Constraint settings,
                                          Model::Properties::VariablePropertyContainer *variables);



    // Constraint interface
public:
    bool CaseSatisfiesConstraint(Case *c);
    void SnapCaseToConstraints(Case *c);

private:
    int max_iterations_;
    QList<WellSplineLength *> length_constraints_;
    InterwellDistance *distance_constraint_;
};

}}

#endif // COMBINEDSPLINELENGTHINTERWELLDISTANCE_H
