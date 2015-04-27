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

#include "masterrunner.h"

MasterRunner::MasterRunner(mpi::communicator *comm)
{
    world = comm;
    printer = new ParallelPrinter(comm->rank());
    perturbationId = 0;
}


void MasterRunner::initialize(QString driverPath)
{
    this->driverPath = driverPath;
    driverReader = new DriverReader(this->driverPath);
    printer->print("Reading driver file...", false);
    model = driverReader->readDriverFile();
    if (!model->validate()) {
        printer->eprint("Failed to validate model.");
    }
    printer->print("Model object successfully created and validated.", false);

    switch (model->getRuntimeSettings()->getOptimizerSettings()->getOptimizer()) {
    case COMPASS:
        printer->print("Compass search optimizer selected.", false);
        opt = new CompassSearchOptimizer();
        opt->initialize(new Case(model), model->getRuntimeSettings()->getOptimizerSettings());
        break;
    default:
        printer->eprint("Optmimzer not recognized. Terminating.");
        break;
    }
    printer->print(model->getRuntimeSettings()->toString(), true);    
    logger = new ResultsLogger(driverPath.remove(driverPath.lastIndexOf("/"), driverPath.length()), model);
}


void MasterRunner::start()
{
    broker = new Broker(world, model, logger);
    while (!opt->isFinished()) {
        printer->print("Starting optimizer iteration.", false);
        QVector<Case*> newCases = opt->getNewCases();
        QVector<int> ids = getCaseIds(newCases.size());
        broker->setPerturbations(newCases, ids);
        broker->evaluatePerturbations();
        opt->compareCases(broker->getResults());
        broker->reset();
        printer->print("Optimizer iteration finished.", false);
    }
    printer->print("Optimization completed.", true);
    finalize();
}

void MasterRunner::finalize() {
    int data = 1;
    for (int i = 1; i < world->size(); ++i) {
        MPI_Send(&data, 1, MPI_INT, i, 999, MPI_COMM_WORLD);
    }
    MPI_Finalize(); // Finalize root process
}


QVector<int> MasterRunner::getCaseIds(int length)
{
    QVector<int> ids;
    for (int i = 0; i < length; ++i) {
        ids.append(perturbationId);
        perturbationId++;
    }
    return ids;
}
