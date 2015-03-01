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

#include "userconstraint.h"

double UserConstraint::upperBound()
{
    QStringList list = m_expression.split("<");
    double max = list.at(list.size() -1).toDouble();
    return max;
}

double UserConstraint::lowerBound()
{
    QStringList list = m_expression.split("<");
    double min = list.at(0).toDouble();
    return min;
}

void UserConstraint::generateArguments()
{
    // Split the expression by '<' and whitespace
    QStringList list = m_expression.split("<");
    QString args = list.at(1).trimmed();
    QStringList arg_list = args.split(QRegExp("\\s+"));

    // Parse the list of arguments
    OPER current_operator = PLUS;
    for(int i = 0; i < arg_list.size(); i += 2)
    {
        m_arguments.push_back(arg_list.at(i));
        m_operators.push_back(current_operator);
        // finding the next operator
        if(i < (arg_list.size() - 1))
        {
            if(arg_list.at(i+1).startsWith("+"))
                current_operator = PLUS;
            else if(arg_list.at(i+1).startsWith("-"))
                current_operator = MINUS;
        }
    }
}

double UserConstraint::resolveArgumentValue(QString arg, bool *ok)
{
    double value = 0;
    bool ok_l = true;

    QStringList list = arg.split("_");
    QString type = list.at(0);                   // well/pipe/separator
    QString id = list.at(1);                     // well name/pipe number
    QString component = list.at(2);              // W/G/O/P/REMOVED (separator only)

    int time_step = list.at(3).toInt(&ok_l);     // time step number according to master schedule
    if(!ok_l) {
        QString message = QString("The time step could not be converted to an integer: %1").arg(list.at(3));
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
    }
    if(time_step >= p_model->numberOfMasterScheduleTimes() || time_step < 0) {
        QString message = QString("The entered time step is not valid: %1").arg(list.at(3));
        emitException(ExceptionSeverity::ERROR, ExceptionType::INCONSISTENT, message);
    }

    if(type.startsWith("WELL"))     // the argument is for a well
    {
        Well *w = 0;

        // looping through the wells, finding the correct one
        for(int i = 0; i < p_model->numberOfWells(); ++i)
        {
            if(p_model->well(i)->name().compare(id) == 0)   // this is the correct well
            {
                w = p_model->well(i);
                break;
            }
        }

        if(w == 0) {
            QString message = QString("Could not fin a well named %1").arg(id);
            emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
        }

        // now checking what type of rate to extract
        if(component.startsWith("G"))
            value = w->stream(time_step)->gasRate();
        else if(component.startsWith("O"))
            value = w->stream(time_step)->oilRate();
        else if(component.startsWith("W"))
            value = w->stream(time_step)->waterRate();
        else if(component.startsWith("P"))
            value = w->stream(time_step)->pressure();
        else if(component.startsWith("L")) // gas lift
        {
            // checking if this is a production well
            ProductionWell *prod_well = dynamic_cast<ProductionWell*>(w);

            if(prod_well != 0)
            {
                // checking if the well has gas lift
                if(prod_well->hasGasLift())
                {
                    // summing together the routing variables
                    double routing = 0;
                    for(int k = 0; k < prod_well->numberOfPipeConnections(); ++k)
                    {
                        routing += prod_well->pipeConnection(k)->variable()->value();
                    }

                    value = prod_well->gasLiftControl(time_step)->controlVar()->value() * routing;
                }
                else {
                    QString message = QString("The well did not have gas lift when L was specified");
                    emitException(ExceptionSeverity::ERROR, ExceptionType::INCONSISTENT, message);
                }
            }
            else {
                QString message = QString("L can only be specified for production wells");
                emitException(ExceptionSeverity::ERROR, ExceptionType::INCONSISTENT, message);
            }
        }
        else {
            QString message = QString("Type of component not recognized: %1").arg(component);
            emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
        }

    } // well
    else if(type.startsWith("PIPE"))
    {
        Pipe *p = 0;

        int pipe_id = id.toInt(&ok_l);
        if(!ok_l) {
            QString message = QString("The pipe id could not be converted to an integer: %1").arg(id);
            emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
        }

        // looping through the pipes, finding the correct one
        for(int i = 0; i < p_model->numberOfPipes(); ++i)
        {
            if(p_model->pipe(i)->number() == pipe_id)   // this is the correct well
            {
                p = p_model->pipe(i);
                break;
            }
        }

        if(p == 0) {
            QString message = QString("Could not find a pipe with id = %1").arg(id);
            emitException(ExceptionSeverity::ERROR, ExceptionType::INCONSISTENT, message);
        }

        // now checking what type of rate to extract
        if(component.startsWith("GAS"))
            value = p->stream(time_step)->gasRate();
        else if(component.startsWith("OIL"))
            value = p->stream(time_step)->oilRate();
        else if(component.startsWith("WAT"))
            value = p->stream(time_step)->waterRate();
        else if(component.startsWith("P"))
            value = p->stream(time_step)->pressure();
        else {
            QString message = QString("Type of component not recognized: %1").arg(component);
            emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
        }
    } // pipe
    else if(type.startsWith("SEP"))
    {
        // first finding the pipe with the correct id, will later try to cast it to separator
        Pipe *p = 0;

        int pipe_id = id.toInt(&ok_l);
        if(!ok_l) {
            QString message = QString("The separator id could not be converted to an integer: %1").arg(id);
            emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
        }

        // looping through the pipes, finding the correct one
        for(int i = 0; i < p_model->numberOfPipes(); ++i)
        {
            if(p_model->pipe(i)->number() == pipe_id)   // this is the correct well
            {
                p = p_model->pipe(i);
                break;
            }
        }

        if(p == 0) {
            QString message = QString("Could not find a separator with id = %1").arg(id);
            emitException(ExceptionSeverity::ERROR, ExceptionType::INCONSISTENT, message);
        }

        // now checking if it actually is a separator
        Separator *s = dynamic_cast<Separator*>(p);

        if(s == 0) {
            QString message = QString("Component #%1 is not a separator.").arg(id);
            emitException(ExceptionSeverity::ERROR, ExceptionType::INCONSISTENT, message);
        }

        // now checking what type of rate to extract
        if(component.startsWith("GAS"))
            value = s->stream(time_step)->gasRate(true);
        else if(component.startsWith("OIL"))
            value = s->stream(time_step)->oilRate(true);
        else if(component.startsWith("WAT"))
            value = s->stream(time_step)->waterRate(true);
        else if(component.startsWith("P"))
            value = s->stream(time_step)->pressure(true);
        else if(component.startsWith("REM")) // the amount of water or gas removed from the separator
        {
            double q_remove = 0;
            if(time_step >= s->installTime()->value())
            {
                // checking if this is a water or gas separator
                if(s->type() == Separator::WATER)
                {
                    // how much water should be removed
                    q_remove = s->stream(time_step)->waterRate(true) * s->removeFraction()->value();
                    if(q_remove > s->removeCapacity()->value())
                        q_remove = s->removeCapacity()->value();
                }
                else if(s->type() == Separator::GAS)
                {
                    // how much gas should be removed
                    q_remove = s->stream(time_step)->gasRate(true) * s->removeFraction()->value();
                    if(q_remove > s->removeCapacity()->value())
                        q_remove = s->removeCapacity()->value();
                }
            }
            value = q_remove;
        }
        else {
            QString message = QString("Type of component not recognized: %1").arg(component);
            emitException(ExceptionSeverity::ERROR, ExceptionType::INCONSISTENT, message);
        }
    } // separator
    else {
        QString message = QString("The type of model component was not recognized: %s").arg(type);
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
    }
    return value;
}

UserConstraint::UserConstraint(Model *m) :
    p_model(m)
{
    p_constraint = shared_ptr<Constraint>(new Constraint());
}

UserConstraint::UserConstraint(const UserConstraint &uc, Model *new_model)
{
    m_expression = uc.m_expression;
    m_arguments = uc.m_arguments;
    m_operators = uc.m_operators;

    p_model = new_model;

    p_constraint = shared_ptr<Constraint>(new Constraint(*uc.p_constraint));
}

void UserConstraint::initialize()
{
    p_constraint->setMax(upperBound());
    p_constraint->setMin(lowerBound());
    p_constraint->setName("User defined constraint: " + m_expression);
    generateArguments();
}

bool UserConstraint::update()
{
    bool ok = true;
    double value = 0;
    for(int i = 0; i < m_arguments.size(); ++i)
    {
        if(m_operators.at(i) == PLUS) value += resolveArgumentValue(m_arguments.at(i), &ok);
        else if(m_operators.at(i) == MINUS) value -= resolveArgumentValue(m_arguments.at(i), &ok);
    }
    p_constraint->setValue(value);
    return ok;
}

void UserConstraint::emitException(ExceptionSeverity severity, ExceptionType type, QString message)
{
    ConstraintHandler* ch = new ConstraintHandler;
    connect(this, SIGNAL(error(ExceptionSeverity, ExceptionType, QString)),
            ch, SLOT(handleException(ExceptionSeverity, ExceptionType, QString)));
    emit error(severity, type, message);
    disconnect(this, SIGNAL(error(ExceptionSeverity, ExceptionType, QString)),
               ch, SLOT(handleException(ExceptionSeverity, ExceptionType, QString)));
}
