#include "separator.h"

Separator::Separator() :
    m_type(WATER),
    p_cost(0),
    p_outlet_connection(0)
{}

Separator::Separator(const Separator &s) :
    Pipe(s)
{
    m_type = s.m_type;
    m_remaining_capacity = s.m_remaining_capacity;

    p_cost = new Cost(*s.p_cost);
    p_outlet_connection = new PipeConnection(*s.p_outlet_connection);

    p_install_time = shared_ptr<IntVariable>(new IntVariable(*s.p_install_time));
    p_remove_fraction = shared_ptr<RealVariable>(new RealVariable(*s.p_remove_fraction));
    p_remove_capacity = shared_ptr<RealVariable>(new RealVariable(*s.p_remove_capacity));

}

Separator::~Separator()
{
    if(p_cost != 0)
        delete p_cost;
    if(p_outlet_connection != 0)
        delete p_outlet_connection;
}

void Separator::initialize(const QVector<double> &schedule)
{
    Pipe::initialize(schedule);
    m_remaining_capacity.fill(p_remove_capacity->value(), schedule.size());
}

void Separator::emptyStreams()
{
    Pipe::emptyStreams();
    m_remaining_capacity.fill(p_remove_capacity->value());
}

void Separator::reduceRemainingCapacity(int i, double q)
{
    double remaining = m_remaining_capacity.at(i);
    remaining = (q > remaining) ? 0 : (remaining - q);
    m_remaining_capacity.replace(i, remaining);
}

void Separator::calculateInletPressure()
{
    if(p_outlet_connection == 0)
    {
        QString message = QString("No outlet pipe set for Separator #%1.").arg(number());
        emitException(ExceptionSeverity::ERROR, ExceptionType::INCONSISTENT, message);
    }

    if(numberOfStreams() != outletConnection()->pipe()->numberOfStreams())
    {
        QString message = QString("Separator #%1 and upstream pipe #%2 do not have the same number of time steps.").arg(number()).arg(outletConnection()->pipe()->number());
        emitException(ExceptionSeverity::ERROR, ExceptionType::INCONSISTENT, message);
    }


    // looping through the time steps
    for(int i = 0; i < numberOfStreams(); i++)
    {
        double p_out = outletConnection()->pipe()->stream(i)->pressure(stream(i)->inputUnits());
        stream(i)->setPressure(p_out);
    }
}

QString Separator::description() const
{
    QString str("START SEPARATOR\n");
    str.append(" NUMBER " + QString::number(number()) + "\n");
    str.append(" TYPE ");
    if(type() == WATER) str.append("WATER\n");
    else if(type() == GAS) str.append("GAS\n");
    if(p_install_time != 0)
    {
        str.append(" INSTALLTIME " + QString::number(p_install_time->value()) + " " + QString::number(p_install_time->max()) + " " + QString::number(p_install_time->min()) + "\n");
    }

    str.append(" COST " + QString::number(p_cost->constantCost()) + " " + QString::number(p_cost->fractionCost()) + " " + QString::number(p_cost->capacityCost()) + "\n");
    str.append(" OUTLETPIPE " + QString::number(p_outlet_connection->pipeNumber()) + "\n");
    str.append(" REMOVE " + QString::number(p_remove_fraction->value()) + " " + QString::number(p_remove_fraction->max()) + " " + QString::number(p_remove_fraction->min()) + "\n");
    str.append(" CAPACITY " + QString::number(p_remove_capacity->value()) + " " + QString::number(p_remove_capacity->max()) + " " + QString::number(p_remove_capacity->min()) + "\n");

    str.append("END SEPARATOR\n\n");
    return str;
}

