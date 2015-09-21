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

#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <QVector>
#include <QObject>
#include <QHash>
#include "case.h"
#include "settings/optimizersettings.h"
#include "exceptionhandler/optimizerhandler.h"

class Case;
class OptimizerSettings;

/*!
 * \brief Virtual parent class for all optimizers. Specifies the basic functions an optimizer
 * should always make available.
 *
 * The basic execution order of the functions should be:
 * 1: initialize.
 * 2: getNewCases.
 * 3: compareCases.
 * 4: isFinished. If isFinished returns false, go to 2.
 * 5. getBestCase().
 *
 */
class Optimizer : public QObject
{
    Q_OBJECT

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

protected:
    Case* best_case;  //!< Best case found thus far.
    QVector<Case*> new_cases;  //!< New cases which have not yet been evaluated.
    int evals;        //!< Evaluations performed.
    int max_evals;    //!< Maximum number of evaluations allowed.
    void emitException(ExceptionSeverity severity, ExceptionType type, QString message);
    void emitProgress(QString message);
public:
//    Optimizer(){}

    /*!
     * \brief Get the case currently considered to be the best one. Should be called _after_ isFinished()
     * has returned true.
     * \return The best case.
     */
    Case* getBestCase() { return best_case; }

    /*!
     * \brief Initialize the optimizer. Sets the base case and the settings. Throws an exception if the settings are insufficient.
     * \param baseCase The base case for the simulation.
     * \param settings Settings object containing the settings required for the specific simulator.
     */
    virtual void initialize(Case* baseCase, OptimizerSettings* settings) = 0;

    /*!
     * \brief Get a new set of cases to be evaluated.
     * \return The new set of cases.
     */
    virtual QVector<Case*> getNewCases() = 0;

    /*!
     * \brief Compare a set of evaluated cases with the current base case.
     *
     * This function will either set one of the input cases as the new best case, or
     * change some setting in the optimizer to allow for a new set of cases to be
     * generated.
     * \param cases The set of cases to be compared.
     */
    virtual void compareCases(QVector<Case*> cases) = 0;

    /*!
     * \brief Checks if the optimizer has finished, i.e. if the maximum number of evaluation
     * or some other limit has been reached.
     *
     * This function should always be called before asking for a new set of cases.
     * \return True if the optimization is finished; otherwise false.
     */
    virtual bool isFinished() = 0;

    /*!
     * \brief Get the current status of the optimizer. This may be used to visualize the progress of the optimizer.
     * \return
     */
    virtual QHash<QString, double>* getStatus() = 0;
};

#endif // OPTIMIZER_H
