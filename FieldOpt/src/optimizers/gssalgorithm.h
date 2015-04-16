#ifndef GSSALGORITHM_H
#define GSSALGORITHM_H

#include <QVector>
#include "case.h"

class Case;

/*!
 * \brief Virtual class for Generating Set Search algorithms.
 * This class should be extended by specific optimization algorithms.
 *
 * This class is based on algorithm 3.2 in the paper "Optimization by Direct
 * Search: New Perspectives on Some Classical and Modern Methods" by
 * Kolda, Lewis and Torczon, published in 2003.
 */
class GSSAlgorithm
{
protected:
    double step_length;     //!< Current step length
    double minimum_step_length;  //!< Step length convergence tolerance.

    virtual QVector<Case*> perturb(Case* c);  //!< Generate perturbed cases from the input case.
    virtual bool isBetter(Case* c);           //!< Check whether or not a case is better than the current best case.

public:
    GSSAlgorithm();
};

#endif // GSSALGORITHM_H
