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

#ifndef CASE_H
#define CASE_H

#include <QVector>
#include <QObject>
#include "case.h"
#include "model/model.h"
#include "variables/realvariable.h"
#include "variables/binaryvariable.h"
#include "variables/intvariable.h"
#include "constraints/constraint.h"
#include "model/objectives/objective.h"
#include "optimizers/derivative.h"
#include "printers/caseprinter.h"

/*!
 * \brief Container class for storing variable, constraint and objective values.
 *
 * This class is used by the Optimizer and the main program to set up cases to be evaluated.
 * The Optimizer should set the variable values that should be used when running the given Case.
 * The number of binary and real variables must match what is in the model.
 *
 * When the evaluation of the case has finished, the constraint and objective values
 * must be set to the Case so that they may be accessed by the Optimizer.
 *
 * \todo This class should likely be rewritten to limit the amount of calls needed to use it.
 */
class Case : public QObject
{
    Q_OBJECT
private:
    QVector<double> m_real_var_values;
    QVector<double> m_binary_var_values;
    QVector<int> m_integer_var_values;
    QVector<double> m_constraint_values;
    double m_objective_value;
    QVector<Derivative*> m_constraint_derivatives;
    Derivative *p_objective_derivative;
    double m_infeasibility;

signals:
    void printCase(const Case& c) const; //!< Received by the CasePrinter class. Print a representation of the Case to the console.


public:
    Case();
    Case(Model *m, bool cpy_output = false);       //!< Constructs a Case based on the current variable values in the model
    Case(const Case &c, bool cpy_output = false);  //!< Copy constructor. Only copies objective and conconstraints if cpy_output = true
    ~Case();                                       //!< Default destructor. Deletes the derivatives.

    void clearConstraints() {m_constraint_values.resize(0);}
    void printToCout();

    void addRealVariableValue(double v) {m_real_var_values.push_back(v);}
    void addBinaryVariableValue(double v) {m_binary_var_values.push_back(v);}
    void addIntegerVariableValue(int v) {m_integer_var_values.push_back(v);}
    void addConstraintValue(double v) {m_constraint_values.push_back(v);}
    void addConstraintDerivative(Derivative *d) {m_constraint_derivatives.push_back(d);}

    void setObjectiveValue(double v) {m_objective_value = v;}
    void setObjectiveDerivative(Derivative *d) {p_objective_derivative = d;}
    void setRealVariableValue(int i, double v) {m_real_var_values.replace(i,v);}
    void setBinaryVariableValue(int i, double v) {m_binary_var_values.replace(i,v);}
    void setIntegerVariableValue(int i, int v) {m_integer_var_values.replace(i,v);}
    void setInfeasibility(double i) {m_infeasibility = i;}

    int numberOfRealVariables() const {return m_real_var_values.size();}
    int numberOfBinaryVariables() const {return m_binary_var_values.size();}
    int numberOfIntegerVariables() const {return m_integer_var_values.size();}
    int numberOfConstraints() const {return m_constraint_values.size();}
    int numberOfConstraintDerivatives() const {return m_constraint_derivatives.size();}
    double realVariableValue(int i) const {return m_real_var_values.at(i);}
    double binaryVariableValue(int i) const {return m_binary_var_values.at(i);}
    int integerVariableValue(int i) const {return m_integer_var_values.at(i);}
    double constraintValue(int i) const {return m_constraint_values.at(i);}
    double objectiveValue() const {return m_objective_value;}
    Derivative* constraintDerivative(int i) {return m_constraint_derivatives.at(i);}
    Derivative* objectiveDerivative() const {return p_objective_derivative;}  //!< This has been made const. Could cause errors.
    double infeasibility() {return m_infeasibility;}

    Case& operator=(const Case &rhs);
};

#endif // CASE_H
