#ifndef WELLSPLINELENGTH_H
#define WELLSPLINELENGTH_H

#include "constraint.h"

namespace Optimization { namespace Constraints {

class WellSplineLength : public Constraint
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

    struct Coord {
        QUuid x;
        QUuid y;
        QUuid z;
    };

    struct Well {
        Coord heel;
        Coord toe;
        QString name;
    };

    Well affected_well_;

    void initializeWell(QList<QPair<QUuid, QString>> vars);
    Coord initializeCoord(QList<QPair<QUuid, QString>> point_vars);

};

}}

#endif // WELLSPLINELENGTH_H
