#ifndef INTERWELLDISTANCE_H
#define INTERWELLDISTANCE_H

#include "constraint.h"
#include "well_spline_constraint.h"
#include <Eigen/Core>

namespace Optimization { namespace Constraints {

class InterwellDistance : public Constraint, WellSplineConstraint
{
public:
    InterwellDistance(Utilities::Settings::Optimizer::Constraint settings,
                      Model::Properties::VariablePropertyContainer *variables);


    // Constraint interface
public:
    bool CaseSatisfiesConstraint(Case *c);
    void SnapCaseToConstraints(Case *c);

private:
    double distance_;
    QList<Well> affected_wells_;

};

}}

#endif // INTERWELLDISTANCE_H
