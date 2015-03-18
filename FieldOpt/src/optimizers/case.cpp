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

#include "case.h"

Case::Case() :
    m_objective_value(0),
    p_objective_derivative(0),
    m_infeasibility(0)
{}


Case::~Case()
{
    if(p_objective_derivative != 0)
        delete p_objective_derivative;

    for(int i = 0; i < m_constraint_derivatives.size(); ++i)
        delete m_constraint_derivatives.at(i);
}

void Case::printToCout()
{
    CasePrinter* cp = new CasePrinter;
    connect(this, SIGNAL(printCase(const Case&)), cp, SLOT(printCase(const Case&)));
    emit printCase(*this);
    disconnect(this, SIGNAL(printCase(const Case&)), cp, SLOT(printCase(const Case&)));
}


Case::Case(Model *m, bool cpy_output)
    : m_objective_value(0),
      p_objective_derivative(0),
      m_infeasibility(0)
{
    for(int i = 0; i < m->realVariables().size(); ++i)
    {
        addRealVariableValue(m->realVariables().at(i)->value());
    }

    for(int i = 0; i < m->binaryVariables().size(); ++i)
    {
        addBinaryVariableValue(m->binaryVariables().at(i)->value());
    }

    for(int i = 0; i < m->integerVariables().size(); ++i)
    {
        addIntegerVariableValue(m->integerVariables().at(i)->value());
    }

    if(cpy_output)
    {
        for(int i = 0; i < m->constraints().size(); ++i)
        {
            addConstraintValue(m->constraints().at(i)->value());
        }
        setObjectiveValue(m->objective()->value());
    }
}


Case::Case(const Case &c, bool cpy_output)
    : m_objective_value(0),
      p_objective_derivative(0),
      m_infeasibility(0)
{
    for(int i = 0; i < c.numberOfRealVariables(); ++i)
    {
        m_real_var_values.push_back(c.m_real_var_values.at(i));
    }

    for(int i = 0; i < c.numberOfBinaryVariables(); ++i)
    {
        m_binary_var_values.push_back(c.m_binary_var_values.at(i));
    }

    for(int i = 0; i < c.numberOfIntegerVariables(); ++i)
    {
        m_integer_var_values.push_back(c.m_integer_var_values.at(i));
    }

    if(cpy_output)
    {
        for(int i = 0; i < c.numberOfConstraints(); ++i)
        {
            m_constraint_values.push_back(c.m_constraint_values.at(i));
        }

        m_objective_value = c.m_objective_value;

        for(int i = 0; i < c.m_constraint_derivatives.size(); ++i)
        {
            m_constraint_derivatives.push_back(new Derivative(*c.m_constraint_derivatives.at(i)));
        }

        if(c.p_objective_derivative != 0)
            p_objective_derivative = new Derivative(*c.p_objective_derivative);

        m_infeasibility = c.m_infeasibility;
    }
}


Case& Case::operator =(const Case &rhs)
{
    if(this != &rhs)
    {
        m_real_var_values = rhs.m_real_var_values;
        m_binary_var_values = rhs.m_binary_var_values;
        m_integer_var_values = rhs.m_integer_var_values;

        m_constraint_values = rhs.m_constraint_values;
        m_objective_value = rhs.m_objective_value;

        for(int i = 0; i < m_constraint_derivatives.size(); ++i)
            delete m_constraint_derivatives.at(i);

        m_constraint_derivatives.resize(0);


        for(int i = 0; i < rhs.m_constraint_derivatives.size(); ++i)
        {
            m_constraint_derivatives.push_back(new Derivative(*rhs.m_constraint_derivatives.at(i)));
        }

        if(p_objective_derivative != 0)
        {
            delete p_objective_derivative;
            p_objective_derivative = 0;
        }

        if(rhs.p_objective_derivative != 0)
            p_objective_derivative =  new Derivative(*rhs.p_objective_derivative);

        m_infeasibility = rhs.m_infeasibility;
    }
    return *this;
}
