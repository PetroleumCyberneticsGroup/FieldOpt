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
    std::cout << "Perturbing the model before simulating." << std::endl;
    std::cout << perturbation->toString() << std::endl;
}

void SimulationLauncher::returnResults()
{
    std::cout << "Returning results." << std::endl;
    SimulationResults res = SimulationResults(perturbation->getModel_id(), 100.0+world->rank());
    world->send(0, 1, res);
}


SimulationLauncher::SimulationLauncher(mpi::communicator *comm)
{
    world = comm;
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
    std::cout << "Receiving perturbation... ";
    ModelPerturbation p = ModelPerturbation();
    world->recv(0, 0, p);
    std::cout << "Received perturbation: " << std::endl;
    perturbation = new ModelPerturbation();
    perturbation->setModel_id(p.getModel_id());
    perturbation->setPerturbation_variable(p.getPerturbation_variable());
    perturbation->setPerturbation_value(p.getPerturbation_value());
}

void SimulationLauncher::startSimulation()
{
    perturbModel();
    std::cout << "Starting simulation...." << std::endl;
    std::cout << "Simulation done." << std::endl;
    returnResults();
}
