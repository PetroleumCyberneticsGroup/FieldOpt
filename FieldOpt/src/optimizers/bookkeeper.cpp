#include "bookkeeper.h"


double Bookkeeper::getTolerance() const
{
    return tolerance;
}

void Bookkeeper::setTolerance(double value)
{
    tolerance = value;
}
double Bookkeeper::calculateDistance(QVector<double> *entry1, QVector<double> *entry2)
{
    double sqsum = 0;
    for (int i = 0; i < entry1->size(); ++i)
        sqsum += (entry1->at(i) - entry2->at(i)) * (entry1->at(i) - entry2->at(i));
    return qSqrt(sqsum);
}

QVector<double> *Bookkeeper::createEntry(Case *c)
{
    QVector<double>* newEntry = new QVector<double>();

    // Add integers
    if (c->numberOfIntegerVariables() > 0) {
        for (int i = 0; i < c->numberOfIntegerVariables(); ++i)
            newEntry->push_back((double)c->integerVariableValue(i));
    }

    // Add reals
    if (c->numberOfRealVariables() > 0) {
        for (int i = 0; i < c->numberOfRealVariables(); ++i)
            newEntry->push_back(c->realVariableValue(i));
    }

    // Add binaries
    if (c->numberOfBinaryVariables() > 0) {
        for (int i = 0; i < c->numberOfBinaryVariables(); ++i)
            newEntry->push_back(c->binaryVariableValue(i));
    }
    return newEntry;
}

void Bookkeeper::addEntry(Case* c)
{
    entries.push_back(createEntry(c));
}

bool Bookkeeper::isCalculated(Case *c)
{
    if (entries.size() == 0)
            return false;

    QVector<double>* newCase = createEntry(c);

    for (int i = 0; i < entries.size(); ++i) {
        if (calculateDistance(newCase, entries.at(i)) <= tolerance)
            return true;
    }
    return false;
}
