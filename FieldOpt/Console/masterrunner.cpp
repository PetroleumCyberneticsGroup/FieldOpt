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
}


void MasterRunner::start()
{
    broker = new Broker(world, model);
    while (!opt->isFinished()) {
        printer->print("STARTING ITERATION...", true);
        printer->print("Getting new cases.", false);
        QVector<Case*> newCases = opt->getNewCases();
        printer->print("Creating new case ids.", false);
        QVector<int> ids = getCaseIds(newCases.size());
        printer->print("Setting perturbations to broker.", false);
        broker->setPerturbations(newCases, ids);
        printer->print("Asking broker to evaluate perturbations.", false);
        broker->evaluatePerturbations();
        printer->print("Comapring cases...", false);
        opt->compareCases(broker->getResults());
        printer->print("Resetting broker.", false);
        broker->reset();
        printer->print("ITERATION FINISHED.", true);
    }
    printer->print("Optimization completed.", true);
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
