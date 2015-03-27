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
}

MasterRunner::~MasterRunner()
{}

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
    printer->print(model->getRuntimeSettings()->toString(), true);
}

void MasterRunner::perturbModel()
{
    perturbations.clear();
    for (int i = 0; i < world->size()-1; ++i) {
        ModelPerturbation p = ModelPerturbation(i, 2, i+100.0);
        perturbations.push_back(p);
    }
}

void MasterRunner::distributePerturbations()
{
    sendPerturbations();

    results.clear();
    recvResults();
}

void MasterRunner::determineOptimal()
{
    printer->print("Determining best perturbation.", false);
}

void MasterRunner::sendPerturbations()
{

    for (int i = 0; i < perturbations.size(); ++i) {
        int id = perturbations.at(i).getModel_id();
        int var_id = perturbations.at(i).getPerturbation_variable();
        float var_val = perturbations.at(i).getPerturbation_value();
        MPI_Send(&id, 1, MPI_INT, i+1, 101, MPI_COMM_WORLD);
        MPI_Send(&var_id, 1, MPI_INT, i+1, 102, MPI_COMM_WORLD);
        MPI_Send(&var_val, 1, MPI_FLOAT, i+1, 103, MPI_COMM_WORLD);
    }
}

void MasterRunner::recvResults()
{
    for (int i = 0; i < perturbations.size(); ++i) {
        int id;
        float fopt;
        MPI_Recv(&id, 1, MPI_INT, i+1, 201, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&fopt, 1, MPI_FLOAT, i+1, 202, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        SimulationResults res = SimulationResults(id ,fopt);
        results.push_back(res);
        printer->print(QString("Got result:\n" + res.toString()), true);
    }
}

