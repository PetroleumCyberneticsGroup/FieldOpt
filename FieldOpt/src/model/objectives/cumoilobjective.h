#ifndef CUMOILOBJECTIVE_H
#define CUMOILOBJECTIVE_H

#include "objective.h"
#include "model/stream.h"

class CumoilObjective : public Objective
{
public:
    CumoilObjective(){};  //!< Default constructor.

    virtual Objective* clone() {return new CumoilObjective(*this);}     //!< Get a copy of this objective.
    virtual QString description() const;                                //!< Generate a description for the driver file.
    virtual void calculateValue(QVector<Stream*> s, QVector<Cost*> c);  //!< Calculate the cumulative oil produced from the input streams
};

#endif // CUMOILOBJECTIVE_H
