/******************************************************************************
 *
 *
 *
 * Created: 16.12.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
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
 *****************************************************************************/

#ifndef ABSTRACTRUNNER_H
#define ABSTRACTRUNNER_H

#include "runtime_settings.h"
#include "Model/model.h"
#include "Optimization/optimizer.h"
#include "Optimization/case.h"
#include "Optimization/objective/objective.h"
#include "Simulation/simulator_interfaces/simulator.h"
#include "Utilities/settings/settings.h"
#include "bookkeeper.h"
#include "Runner/logger.h"

namespace Runner {

class MainRunner;

/*!
 * \brief The AbstractRunner class is the abstract parent class for all runners. It should only be constructed by the MainRunner class.
 *
 * This class initializes the primary objects needed and provides some utility functions for logging.
 *
 * It also defines the purely virtual Execute() method which should be implemented by all concrete
 * runners.
 */
class AbstractRunner
{
    friend class MainRunner;
private:

    /*!
     * \brief Execute starts the actual optimization run and should not return until the optimization is done.
     */
    virtual void Execute() = 0;
    const double sentinel_value_ = 0.0001; //!< Value to be used as a sentinel value for the objective function of cases that cannot be evaluated.

protected:
    AbstractRunner(RuntimeSettings *runtime_settings);

    Bookkeeper *bookkeeper_;
    Model::Model *model_;
    Utilities::Settings::Settings *settings_;
    RuntimeSettings *runtime_settings_;
    Optimization::Case *base_case_;
    Optimization::Optimizer *optimizer_;
    Optimization::Objective::Objective *objective_function_;
    Simulation::SimulatorInterfaces::Simulator *simulator_;
    Logger *logger_;

    /*!
     * \brief sentinelValue Get the sentinel value to be used as objective function values for cases
     * that cannot be evaluated.
     *
     * When maximizing, this will be 0.0001; when minimizing, this will be -0.0001.
     * \return
     */
    double sentinelValue() const;

    void InitializeSettings(QString output_subdirectory="");
    void InitializeModel();
    void InitializeSimulator();
    void EvaluateBaseModel();
    void InitializeObjectiveFunction();
    void InitializeBaseCase();
    void InitializeOptimizer();
    void InitializeBookkeeper();

    void InitializeLogger();
};

}

#endif // ABSTRACTRUNNER_H
