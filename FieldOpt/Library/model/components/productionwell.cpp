/*
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einarjba@stud.ntnu.no>
 *
 * The code is largely based on ResOpt, created by  Aleksander O. Juell <aleksander.juell@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "productionwell.h"

ProductionWell::ProductionWell(const ProductionWell &w) :
    Well(w)
{

    for(int i = 0; i < w.numberOfBhpConstraints(); ++i)
        m_bhp_constraints.push_back(shared_ptr<Constraint>(new Constraint(*w.m_bhp_constraints.at(i))));

    if(w.p_connection_constraint != 0)
        p_connection_constraint = shared_ptr<Constraint>(new Constraint(*w.p_connection_constraint));

    for(int i = 0; i < w.numberOfPipeConnections(); i++)
        m_pipe_connections.push_back(new PipeConnection(*w.m_pipe_connections.at(i)));

    for(int i = 0; i < w.m_gaslift_schedule.size(); i++)
        m_gaslift_schedule.push_back(new WellControl(*w.m_gaslift_schedule.at(i)));
}

ProductionWell::~ProductionWell()
{
    for(int i = 0; i < numberOfPipeConnections(); ++i)
        delete pipeConnection(i);
    for(int i = 0; i < m_gaslift_schedule.size(); ++i)
        delete m_gaslift_schedule.at(i);
}

void ProductionWell::setupConstraints()
{
    m_bhp_constraints.clear();
    for(int i = 0; i < numberOfControls(); ++i)
    {
        shared_ptr<Constraint> bhp_con = shared_ptr<Constraint>(new Constraint(0.5, 1.0, 0.0));
        bhp_con->setName("Bottomhole pressure constraint for well: " + name() + " for time = " + QString::number(control(i)->endTime()));
        m_bhp_constraints.push_back(bhp_con);
    }

    if(numberOfPipeConnections() > 1)
    {
        p_connection_constraint = shared_ptr<Constraint>(new Constraint(1.0, 1.0, 0.0));
        p_connection_constraint->setName("Pipe routing constraint for well: " + name());
    }
    else if(numberOfPipeConnections() == 1)
    {
        if(pipeConnection(0)->variable()->isVariable())
        {
            p_connection_constraint = shared_ptr<Constraint>(new Constraint(1.0, 1.0, 0.0));
            p_connection_constraint->setName("Pipe routing constraint for well: " + name());
        }
    }
}

void ProductionWell::updateBhpConstraint()
{
    // Check that all the connected pipes have the same number of streams as the well
    for(int i = 0; i < numberOfPipeConnections(); ++i)
    {
        Pipe *p = pipeConnection(i)->pipe();
        if(numberOfStreams() != p->numberOfStreams())
        {
            QString message = QString("Well and pipe do not have the same number of arguments. \nWELL: %1, N=%2\nPIPE: %3, N=%4\n")
                    .arg(name().toLatin1().constData()).arg(numberOfStreams()).arg(p->number()).arg(p->numberOfStreams());
            emitException(ExceptionSeverity::ERROR, ExceptionType::OUT_OF_BOUNDS, message);
        }
    }

    // Check that the number of streams is equal to the number of bhp constraints
    if(numberOfStreams() != numberOfBhpConstraints())
    {
        QString message = QString("WELL: %1 \nDoes not have the same number of streams and BHP constraints.").arg(name().toLatin1().constData());
        emitException(ExceptionSeverity::ERROR, ExceptionType::INCONSISTENT, message);
    }

    // Update all constraints
    for(int i = 0; i < numberOfStreams(); i++)
    {
        double c_ts = 0.5;

        if(isInstalled(i))
        {
            // Calculate weighted average pressure
            double p_in = 0;
            double tot_frac = 0;
            for(int j = 0; j < numberOfPipeConnections(); ++j)
            {
                p_in += pipeConnection(j)->variable()->value() * pipeConnection(j)->pipe()->stream(i)->pressure(stream(i)->inputUnits());
                tot_frac += pipeConnection(j)->variable()->value();
            }

            if(tot_frac == 0) // error checking if no routing to the well
            {
                tot_frac = 1;
                p_in =  0.0; // 2 * stream(i)->pressure();
            }

            p_in = p_in / tot_frac;

            // calculating constraint value
            double p_wf = stream(i)->pressure();
            if(p_wf < 0.001)
                p_wf = 0.001;

            c_ts = (p_wf - p_in) / p_wf;
        }

        // updating the value of the constraint for this time
        bhpConstraint(i)->setValue(c_ts);

        // printing if violating
        if(c_ts < -0.0001)
        {
            QString message = QString("BHP constraint for WELL: %1 is violated at time=%2, c=%3 \n Stream information follows.")
                    .arg(name().toLatin1().constData()).arg(stream(i)->time()).arg(c_ts);
            emitException(ExceptionSeverity::WARNING, ExceptionType::CONSTRAINT_VIOLATED, message);
            stream(i)->printToCout();
        }

    }
}

void ProductionWell::updatePipeConnectionConstraint()
{
    if(p_connection_constraint != 0)
    {
        double c = 0;
        for(int i = 0; i < numberOfPipeConnections(); i++)
        {
            c += pipeConnection(i)->variable()->value();
        }
        pipeConnectionConstraint()->setValue(c);
    }
}

double ProductionWell::flowFraction(Pipe *p, bool *ok)
{
    double frac = 0.0;

    for(int i = 0; i < numberOfPipeConnections(); i++)
    {
        // first checking if the well is directly connected to the pipe
        if(p->number() == pipeConnection(i)->pipe()->number())
        {
            frac += pipeConnection(i)->variable()->value();
        }

        // then checking if the well is connected indirectly to the pipe (this only appplies to MidPipes)
        MidPipe *p_mid = dynamic_cast<MidPipe*>(pipeConnection(i)->pipe());
        if(p_mid != 0)
        {
            frac += p_mid->flowFraction(p, ok) * pipeConnection(i)->variable()->value();
        }
    }
    return frac;
}

QString ProductionWell::description() const
{
    QString str("START WELL\n");
    str.append(" NAME " + name() + "\n");
    str.append(" TYPE P \n");
    str.append(" GROUP " + group() + "\n");
    str.append(" BHPLIMIT " + QString::number(bhpLimit()) + "\n");

    if(hasInstallTime())
    {
        str.append(" INSTALLTIME " + QString::number(installTime()->value()) + " " + QString::number(installTime()->max()) + " " + QString::number(installTime()->min()) + "\n");
    }

    if(hasCost())
    {
        str.append(" COST " + QString::number(cost()->constantCost()) + " " + QString::number(cost()->fractionCost()) + " " + QString::number(cost()->capacityCost()) + "\n");
    }

    str.append(" START OUTLETPIPES\n");

    foreach(PipeConnection *pc, m_pipe_connections)
    {
        str.append("  " + QString::number(pc->pipeNumber()) + "   " + QString::number(pc->variable()->value()) + "  ");
        if(pc->variable()->isVariable()) str.append("BIN\n");
        else str.append("\n");
    }

    str.append(" END OUTLETPIPES\n\n");

    if(numberOfConstantConnections() > 0)
    {
        str.append(" START CONNECTIONS\n");
        for(int i = 0; i < numberOfConnections(); ++i)
        {
            WellConnection *wc = constantConnection(i);
            if(wc->cell() >= 0) str.append("  " + QString::number(wc->cell()) + " " + QString::number(wc->wellIndex()) + "\n");
            else str.append("  " + QString::number(wc->i()) + " " +
                            QString::number(wc->j()) + " " +
                            QString::number(wc->k1()) + " " +
                            QString::number(wc->k2()) + " " +
                            QString::number(wc->wellIndex()) + "\n");
        }
        str.append(" END CONNECTIONS\n\n");
    }

    if(numberOfVariableConnections() > 0)
    {
        str.append(" START VARCONNECTIONS\n");
        for(int i = 0; i < numberOfVariableConnections(); ++i)
        {
            WellConnectionVariable *wcv = variableConnection(i);
            str.append("  " + QString::number(wcv->iVariable()->value()) + " " +
                       QString::number(wcv->iVariable()->max()) + " " +
                       QString::number(wcv->iVariable()->min()) + " " +
                       QString::number(wcv->jVariable()->value()) + " " +
                       QString::number(wcv->jVariable()->max()) + " " +
                       QString::number(wcv->jVariable()->min()) + " " +
                       QString::number(wcv->wellConnection()->k1()) + " " +
                       QString::number(wcv->wellConnection()->k2()) + " " +
                       QString::number(wcv->wellConnection()->wellIndex()) + "\n");
        }
        str.append(" END VARCONNECTIONS\n");
    }

    str.append(" START SCHEDULE\n");

    for(int i = 0; i < numberOfControls(); ++i)
        str.append(control(i)->description());

    str.append(" END SCHEDULE\n\n");
    str.append("END WELL\n\n");
    return str;
}
