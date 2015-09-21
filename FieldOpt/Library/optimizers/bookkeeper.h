#ifndef BOOKKEEPER_H
#define BOOKKEEPER_H

#include <QVector>
#include <math.h>
#include "case.h"

class Bookkeeper
{
private:
    QVector<QVector<double>*> entries;  //!< _All_ variables representing a point.
    double tolerance;  //!< A threshold for how large deviation from an already calculated point is allowed.

    double calculateDistance(QVector<double>* entry1, QVector<double>* entry2);  //!< Calculate the distance between two entries.
    QVector<double>* createEntry(Case* c);  //!< Combine and cast all variables in a case into one vector of doubles.

public:
    Bookkeeper(double tolerance) { this->tolerance = tolerance; }
    void addEntry(Case* c);  //!< Add an entry to the Bookkeeper.
    bool isCalculated(Case* c);    //!< Check whether a value has already been calculated and stored in the Bookkeeper.
    double getTolerance() const;
    void setTolerance(double value);
};

#endif // BOOKKEEPER_H
