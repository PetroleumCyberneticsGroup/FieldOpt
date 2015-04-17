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
    workingDirectory = driverPath.remove(driverPath.lastIndexOf("/"), driverPath.length());
    QDir dir(workingDirectory);

    // Remove existing output directory
    QString outputdirName = "output_p" + QString::number(world->rank());

    printer->print(QString("Creating output directory in %1").arg(dir.absolutePath()), false);
    if (!dir.exists(outputdirName))
        dir.mkdir(outputdirName);
    if (!dir.exists(outputdirName))
        printer->eprint("Unable to create output directory.");
    outputPath = dir.absolutePath() + "/" + outputdirName;

    // Copy reservoir description file to output folder
    QString newResFile = outputPath + "/" + model->reservoir()->file();
    printer->print("Copying reservoir description file to" + newResFile, false);
    QFile::remove(newResFile);
    QFile::copy(workingDirectory + "/" + model->reservoir()->file(), newResFile);
}

void SimulationLauncher::returnResults()
{
    printer->print("Returning results.", false);
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
    printer->print("Reading driver file...", false);
    model = driverReader->readDriverFile();
    model->readPipeFiles();
    model->resolveCapacityConnections();
    model->resolvePipeRouting();
    model->initialize();
    printer->print("Model object created.", false);
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
    printer->print("Receiving perturbation...", false);
    std::vector<int> header;
    header.reserve(4);
    MPI_Recv(&header, 4, MPI_INT, 0, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printer->print("Received header.", false);

    std::vector<double> binaries;
    binaries.reserve(header[1]);
    MPI_Recv(&binaries, header[1], MPI_DOUBLE, 0, 11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printer->print("Received binaries.", false);

    std::vector<int> integers;
    integers.reserve(header[2]);
    MPI_Recv(&integers, header[2], MPI_INT, 0, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printer->print("Received integers.", false);

    std::vector<double> reals;
    reals.reserve(header[3]);
    MPI_Recv(&reals, header[3], MPI_DOUBLE, 0, 13, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printer->print("Received reals.", false);

    perturbation = new Perturbation(header, binaries, integers, reals);
    Case* c = perturbation->getCase(model);

    AdjointsCoupledModel* acm = dynamic_cast<AdjointsCoupledModel*>(model);
    if (acm != 0) {
        acm->applyCaseVariables(c);
        model = acm;
    }

    printer->print("Received perturbation.", false);
}

void SimulationLauncher::startSimulation()
{
    if (this->model->getRuntimeSettings()->getSimulatorSettings()->getSimulator() == MRST) {
        printer->print("Starting MRST simulation.", true);
        setupWorkingDirectory();
        MrstBatchSimulator sim = MrstBatchSimulator();
        sim.setFolder(outputPath);
        printer->print("Generating simulator input files.", true);
        if(!sim.generateInputFiles(model))
            printer->eprint("Failed to generate simulator input files.");
        printer->print("Starting Launching simulator.", true);
        sim.launchSimulator();
        sim.readOutput(model);
        model->process();
        printer->print("Simulation done.", true);
        printer->print("OBJECTIVE:\n" + model->objective()->description() + "\n Value: " + QString::number(model->objective()->value()) + "\n", true);
    }
}
