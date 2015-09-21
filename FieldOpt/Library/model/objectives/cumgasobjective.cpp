#include "cumgasobjective.h"

QString CumgasObjective::description() const
{
    QString str("START OBJECTIVE\n");
    str.append(" TYPE CUMGAS \n");
    str.append("END OBJECTIVE\n\n");
    return str;
}

void CumgasObjective::calculateValue(QVector<Stream *> s, QVector<Cost *> c)
{
    double cumgas = 0;
    for(int i = 0; i < s.size(); i++)
    {
        double dt;
        // calculate time step duration
        if(i == 0)
            dt = s.at(i)->time();
        else
            dt = s.at(i)->time() - s.at(i-1)->time();
        cumgas += dt * s.at(i)->gasRate(s.at(i)->inputUnits());
    }
    setValue(cumgas);
}
