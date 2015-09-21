#ifndef COMPASSSEARCHOPTIMIZER_H
#define COMPASSSEARCHOPTIMIZER_H

#include "gssalgorithm.h"
#include "optimizer.h"
#include "constraints/integerboundaryconstraint.h"
#include "constraints/doubleboundaryconstraint.h"
#include "bookkeeper.h"

/*!
 * \brief The CompassSearchOptimizer class is based on the Compass Search algorithm
 * described in the paper "Optimization by Direct Search: New Perspectives on Some
 * Classical and Modern Methods" by Kolda, Lewis and Torczon, published in 2003.
 *
 * The main principle is to check points by increasing/decreasing each of the
 * variables by some amount \f$\Delta_x\f$. If a better objective value is found,
 * new perturbations are created from that point. If no better values are found,
 * new perturbations are generated from the original point by reducing
 * \f$\Delta_x\f$ and changing the variables according to the new size. This
 * process repeats untill \f$\Delta_x\f$ reaches a set minimum value.
 */
class CompassSearchOptimizer : public Optimizer, public GSSAlgorithm
{
private:
    QVector<Case*> perturb(Case *c);  //!< Create perturbed Cases from Case c according to the current value of \f$\Delta_x\f$.
    bool isBetter(Case *c);  //!< Check if a Case c is better than the current best case.
    bool isValid(Case *c);  //!< Check if a Case is valid; i.e. that it does not break boundary conditions, and that it has not already been evaluated.
    Bookkeeper* bookkeeper;  //!< A bookkeeper holding coordinates which have already been evaluated.
    void reduceStepLength();  //!< Reduce the value of \f$\Delta_x\f$.

public:
    CompassSearchOptimizer(){ evals = 0; }

    void initialize(Case* baseCase, OptimizerSettings* settings);
    QVector<Case*> getNewCases();
    void compareCases(QVector<Case*> cases);
    bool isFinished();
    QHash<QString, double>* getStatus();

    QString getStatusString();
};

#endif // COMPASSSEARCHOPTIMIZER_H
