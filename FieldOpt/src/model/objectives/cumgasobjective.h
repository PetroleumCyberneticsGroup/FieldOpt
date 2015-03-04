#ifndef CUMGASOBJECTIVE_H
#define CUMGASOBJECTIVE_H

#include "model/objectives/objective.h"

/*!
 * \brief Class for Objectives that maximize gas cumulative production.
 */
class CumgasObjective : public Objective
{
public:
    CumgasObjective(){};

    virtual Objective* clone() {return new CumgasObjective(*this);}    //!< Get a clone of this objective.
    virtual QString description() const;                               //!< Generate a description for the driver file.
    virtual void calculateValue(QVector<Stream*> s, QVector<Cost*> c); //!< Calculate the cumulative gas produced from the input streams
};

#endif // CUMGASOBJECTIVE_H
