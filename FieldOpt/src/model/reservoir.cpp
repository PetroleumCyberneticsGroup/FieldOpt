#include "reservoir.h"

Reservoir::Reservoir()
{

}

QString Reservoir::description() const
{
    QString str("START RESERVOIR\n");
    str.append(" NAME " + name() + "\n");
    str.append(" FILE " + file() + "\n");
    str.append(" MRST " + mrstPath() + "\n");
    str.append(" MATLAB " + matlabPath() + "\n");
    str.append(" SCRIPT " + mrstScript() + "\n");

    str.append(" TIME " + QString::number(endTime()) + "\n");

    str.append(" PHASES " + QString::number(m_gas_phase) + " " +
               QString::number(m_oil_phase) + " " +
               QString::number(m_wat_phase) + "\n");

    str.append(" DENS " + QString::number(m_den_gas) + " " +
               QString::number(m_den_oil) + " " +
               QString::number(m_den_wat) + "\n");

    str.append("END RESERVOIR\n\n");

    return str;
}

void Reservoir::setPhases(bool gas, bool oil, bool water)
{
    m_gas_phase = gas;
    m_oil_phase = oil;
    m_wat_phase = water;
}

void Reservoir::setDensities(double gas, double oil, double water)
{
    m_den_gas = gas;
    m_den_oil = oil;
    m_den_wat = water;
}
