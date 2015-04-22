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

#include "simulationlauncher.h"

void SimulationLauncher::setupWorkingDirectory()
{    
    // Create output directory
    if (workingDirectory.length() == 0)
        workingDirectory = driverPath.remove(driverPath.lastIndexOf("/"), driverPath.length());
    QDir dir(workingDirectory);

    // Remove existing output directory
    QString outputdirName = "output_p" + QString::number(world->rank());

    if (!dir.exists(outputdirName))
        dir.mkdir(outputdirName);
    if (!dir.exists(outputdirName))
        printer->eprint("Unable to create output directory.");
    outputPath = dir.absolutePath() + "/" + outputdirName;

    // Copy reservoir description file to output folder
    QString newResFile = outputPath + "/" + model->reservoir()->file();
    QFile::remove(newResFile);
    QFile::copy(workingDirectory + "/" + model->reservoir()->file(), newResFile);
}

void SimulationLauncher::returnResults()
{
    int id = perturbation->getPerturbation_id();
    double result = model->objective()->value();
    MPI_Send(&id, 1, MPI_INT, 0, 20, MPI_COMM_WORLD);
    MPI_Send(&result, 1, MPI_DOUBLE, 0, 21, MPI_COMM_WORLD);
}

SimulationLauncher::SimulationLauncher(mpi::communicator *comm)
{
    world = comm;
    printer = new ParallelPrinter(comm->rank());
}

void SimulationLauncher::initialize(QString driverPath)
{
    this->driverPath = driverPath;
    driverReader = new DriverReader(this->driverPath);
    model = driverReader->readDriverFile();
    model->readPipeFiles();
    model->resolveCapacityConnections();
    model->resolvePipeRouting();
    model->initialize();
}

void SimulationLauncher::start()
{
    while (true) {
        receivePerturbations();
        startSimulation();
        returnResults();
    }
}

void SimulationLauncher::receivePerturbations()
{
    std::vector<int> header;
    header.reserve(4);
    MPI_Recv(&header[0], 4, MPI_INT, 0, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    std::vector<double> binaries;
    if (header[1] > 0) {
        binaries.reserve(header[1]);
        MPI_Recv(&binaries[0], header[1], MPI_DOUBLE, 0, 11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    std::vector<int> integers;
    if (header[2] > 0) {
        integers.reserve(header[2]);
        MPI_Recv(&integers[0], header[2], MPI_INT, 0, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    std::vector<double> reals;
    if (header[3] > 0) {
        reals.reserve(header[3]);
        MPI_Recv(&reals[0], header[3], MPI_DOUBLE, 0, 13, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    perturbation = new Perturbation(header, binaries, integers, reals);
    Case* c = perturbation->getCase(model);

    AdjointsCoupledModel* acm = dynamic_cast<AdjointsCoupledModel*>(model);
    if (acm != 0) {
        acm->applyCaseVariables(c);
        model = acm;
    }
}

void SimulationLauncher::startSimulation()
{
    if (this->model->getRuntimeSettings()->getSimulatorSettings()->getSimulator() == MRST) {
        printer->print("Starting MRST simulation.", true);
        setupWorkingDirectory();
        MrstBatchSimulator sim = MrstBatchSimulator();
        sim.setFolder(outputPath);
        if(!sim.generateInputFiles(model))
            printer->eprint("Failed to generate simulator input files.");
        sim.launchSimulator();
        sim.readOutput(model);
        model->process();
        printer->print("Simulation done.", true);
    }
}
