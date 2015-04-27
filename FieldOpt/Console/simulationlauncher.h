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

#ifndef SIMULATIONLAUNCHER_H
#define SIMULATIONLAUNCHER_H

#include <boost/mpi.hpp>
#include <QString>
#include <QDir>
#include <vector>

//#include "transferobjects/modelperturbation.h"
//#include "transferobjects/simulationresults.h"
#include "transferobjects/perturbation.h"
#include "transferobjects/result.h"
#include "model/model.h"
#include "model/coupledmodel.h"
#include "simulators/simulator.h"
#include "simulators/mrstbatchsimulator.h"
#include "parallelprinter.h"
#include "fileio/readeres/driverreader.h"

class Result;
class Perturbation;

namespace mpi = boost::mpi;

/*!
 * \brief The SimulationLauncher class is responsible for launching simulations.
 *
 * This class reads the model from the driver file, and applies a perturbation
 * described by a ModelPerturbation object which is reveived from the MasterRunner.
 *
 * It then performs a simulation on the model, before returning the results as
 * as SimulationResults object to the MasterRunner.
 */
class SimulationLauncher
{
private:
//    ModelPerturbation* perturbationOLD;
    Perturbation* perturbation;
    DriverReader* driverReader;
    Model* model;
    mpi::communicator* world;
    ParallelPrinter* printer;
    QString driverPath;
    QString workingDirectory;
    QString outputPath;
    void setupWorkingDirectory();

    void returnResults();

    bool receivePerturbations();  //!< Returns false if the termination signal is received; otherwise true.
    void startSimulation();

    void finalize();

public:
    SimulationLauncher(mpi::communicator *comm);
    ~SimulationLauncher() {}

    void initialize(QString driverPath);
    void start();

};

#endif // SIMULATIONLAUNCHER_H
