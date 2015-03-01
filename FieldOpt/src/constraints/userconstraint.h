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

#ifndef USERCONSTRAINT_H
#define USERCONSTRAINT_H

#include <QString>
#include <QStringList>
#include <QList>
#include <tr1/memory>

#include "constraint.h"
#include "exceptionhandler/constrainthandler.h"
#include "model/components/well.h"
#include "model/components/productionwell.h"
#include "model/components/pipe.h"
#include "model/components/separator.h"

using std::tr1::shared_ptr;

class Model;
class Constraint;
class Well;
class Pipe;
class Separator;

/*!
 * \brief Describes a constraint defined by the user in the driver file.
 *
 * This class makes it possible for the user to specify a constaint that is not explicitly included by the model through an equation in the driver file.
 */
class UserConstraint : QObject
{
    Q_OBJECT
public:
    enum OPER {PLUS, MINUS};

signals:
    /*!
     * \brief Error signal.
     *
     * Emitted when an error occurs.
     *
     * \param severity The severity of the event. May be WARNING or ERROR.
     * \param type The type of exception.
     * \param message The message to be printed in addition to the severity and the type.
     */
    void error(ExceptionSeverity severity, ExceptionType type, const QString message);

private:
    QString m_expression;                     //!< The expression for the constraint
    QStringList m_arguments;                  //!< List of arguments in the user constraint.
    QList<UserConstraint::OPER> m_operators;  //!< List of operators in the user constraint.
    Model *p_model;                           //!< The model this constraint belongs to.
    shared_ptr<Constraint> p_constraint;      //!< The Constraint object generated from user defined expression.

    double upperBound();                                 //!< Parse the expression and get the upper bound.
    double lowerBound();                                 //!< Parse the expression and get the lower bound.
    void generateArguments();                            //!< Parse the expression and generate a list of arguments.

    /*!
     * \brief Resolves the value of an argument.
     *
     * Expects an argument-string on the following form:
     *
     * type_id_comp_ts
     *
     * Examples:
     *
     * WELL_prod1_OIL_1
     * PIPE_1_GAS_2
     * SEP_3_REMOVED_3
     *
     * \todo Split resolve well/pipe/separator out into separate private functions.
     * \param arg The argument to be resolved.
     * \param ok \todo This parameter does not seem to be used for anything.
     * \return
     */
    double resolveArgumentValue(QString arg, bool *ok);  //!< Resolves the value of an argument.

public:
    UserConstraint(Model *m);                                    //!< Default constructor. Sets the model pointer and creates a new Constraint.
    UserConstraint(const UserConstraint &uc, Model *new_model);  //!< Creates a copy of this contraint in a new model.

    void initialize();  //!< Sets the bounds and name, genereates the arguments string list.
    bool update();      //!< Parses the arguments and updates the m_value field in the constraint.

    void setExpression(const QString &e) {m_expression = e;}  //!< Set the expression.

    const QString& expression() const {return m_expression;}    //!< Get the expression string.
    shared_ptr<Constraint> constraint() {return p_constraint;}  //!< Get the constraint corresponding to the user defined expression.
    Model* model() {return p_model;}                            //!< Get the model this constraint is connected to.

protected:
    /*!
     * \brief Convenience class used by the component subclasses to emit exceptions.
     * \param severity The severity of the event.
     * \param type The type of exception.
     * \param message The message to be printed.
     */
    void emitException(ExceptionSeverity severity, ExceptionType type, QString message);

};

#endif // USERCONSTRAINT_H
