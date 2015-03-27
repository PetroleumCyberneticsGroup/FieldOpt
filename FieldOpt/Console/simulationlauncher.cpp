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
    printer->print(QString("Creating output directory in %1").arg(dir.absolutePath()), false);
    if (!dir.exists("output"))
        dir.mkdir("output");
    if (!dir.exists("output"))
        printer->eprint("Unable to create output directory.");
    outputPath = dir.absolutePath() + "/output";

    // Copy reservoir description file to output folder
    QString newResFile = outputPath + "/" + model->reservoir()->file();
    printer->print("Copying reservoir description file to" + newResFile, false);
    QFile::remove(newResFile);
    QFile::copy(workingDirectory + "/" + model->reservoir()->file(), newResFile);
}

void SimulationLauncher::perturbModel()
{
    printer->print("Perturbing the model before simulating: \n" + perturbation->toString(), true);
}

void SimulationLauncher::returnResults()
{
    printer->print("Returning results.", false);
    SimulationResults res = SimulationResults(perturbation->getModel_id(), 100.0+world->rank());
    int id = res.getModel_id();
    float fopt = res.getModel_fopt();
    MPI_Send(&id, 1, MPI_INT, 0, 201, MPI_COMM_WORLD);
    MPI_Send(&fopt, 1, MPI_FLOAT, 0, 202, MPI_COMM_WORLD);
}

SimulationLauncher::SimulationLauncher(mpi::communicator *comm)
{
    world = comm;
    printer = new ParallelPrinter(comm->rank());
}

SimulationLauncher::~SimulationLauncher()
{

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

void SimulationLauncher::receivePerturbations()
{
    printer->print("Reveiving perturbation...", false);
    int id;
    int var_id;
    float var_val;
    MPI_Recv(&id, 1, MPI_INT, 0, 101, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&var_id, 1, MPI_INT, 0, 102, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&var_val, 1, MPI_FLOAT, 0, 103, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    ModelPerturbation p = ModelPerturbation(id, var_id, var_val);

    printer->print("Received perturbation.", false);
    perturbation = new ModelPerturbation();
    perturbation->setModel_id(p.getModel_id());
    perturbation->setPerturbation_variable(p.getPerturbation_variable());
    perturbation->setPerturbation_value(p.getPerturbation_value());
}

void SimulationLauncher::startSimulation()
{
    perturbModel();
    if (this->model->getRuntimeSettings()->getSimulatorSettings()->getSimulator() == MRST) {
        printer->print("Starting MRST simulation.", true);
        setupWorkingDirectory();
        MrstBatchSimulator sim = MrstBatchSimulator();
        sim.setFolder(outputPath);
        if(!sim.generateInputFiles(model))
            printer->eprint("Failed to generate simulator input files.");
        sim.launchSimulator();
        sim.readOutput(model);
        printer->print("Simulation done.", true);
    }
    returnResults();
}
