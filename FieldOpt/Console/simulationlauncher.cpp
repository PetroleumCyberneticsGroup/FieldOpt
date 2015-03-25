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

void SimulationLauncher::initialize()
{
    model = new CoupledModel();
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
    printer->print("Starting simulation...", false);
    printer->print("Simulation done.", false);
    returnResults();
}
