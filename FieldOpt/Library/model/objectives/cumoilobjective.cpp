#include "cumoilobjective.h"


QString CumoilObjective::description() const
{
    QString str("START OBJECTIVE\n");
    str.append(" TYPE CUMOIL \n");
    str.append("END OBJECTIVE\n\n");
    return str;
}

void CumoilObjective::calculateValue(QVector<Stream *> s, QVector<Cost *> c)
{
    double cumoil = 0;
    for(int i = 0; i < s.size(); i++)
    {
        double dt;
        if(i == 0)
            dt = s.at(i)->time();
        else
            dt = s.at(i)->time() - s.at(i-1)->time();

        // adding the time step oil production to the cum
        cumoil += dt * s.at(i)->oilRate(s.at(i)->inputUnits());

        if(isnan(cumoil))
        {
            QString message = QString("Objective is NaN.\n Stream information is shown above.");
            s.at(i)->printToCout();
            emitException(ExceptionSeverity::ERROR, ExceptionType::COMPUTATION_ERROR, message);
        }
    }
    // setting the cum oil to the objective value
    setValue(cumoil);
}
