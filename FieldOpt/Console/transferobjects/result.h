#ifndef RESULT_H
#define RESULT_H

#include "optimizers/case.h"

/*!
 * \brief The Result class is a convenience class used for handling the result of
 * a Case evaluation before and after transfering them using MPI.
 */
class Result
{
private:
    const int perturbation_id;  //!< A unique id for the result. Should correspond to the id of a Perturbation object.
    const double result;        //!< The value of the result.

public:
    Result(Case* c, int id);  //!< Create a Result object from an evaluated Case and an id.

    int getPerturbation_id() const { return perturbation_id; }  //!< Get the id of this result.
    double getResult() const { return result; }                 //!< Get the value of the result.
};

#endif // RESULT_H
