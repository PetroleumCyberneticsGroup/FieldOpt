#ifndef GSSOPTIMIZER_H
#define GSSOPTIMIZER_H

#include "gssoptimizer_global.h"
#include <QString>
#include <QVector>

class GSSOPTIMIZERSHARED_EXPORT GSSOptimizer
{
private:
    int dim;                       //!< The dimension of the optimization problem.
    QString f;                     //!< Objective function.
    QVector<double> x0;            //!< Initial coordinate.
    QVector<QString> constraints;  //!< Constraints for the problem.

    QVector<QVector<double>> points_evaluated; //!< Points evaluated in the current run.

    int evals;            //!< The number of function evaluations performed.
    int max_evals;        //!< Maximum number of function evaluations performed.
    bool use_bookkeeper;  //!< Whether or not to use bookeeping.

public:
    GSSOptimizer();

};

#endif // GSSOPTIMIZER_H
