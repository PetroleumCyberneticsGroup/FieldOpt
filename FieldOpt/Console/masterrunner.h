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

#ifndef MASTERRUNNER_H
#define MASTERRUNNER_H

#include <boost/mpi.hpp>
#include <QVector>
#include <QString>

//#include "transferobjects/modelperturbation.h"
//#include "transferobjects/simulationresults.h"
//#include "transferobjects/perturbation.h"
#include "optimizers/optimizer.h"
#include "optimizers/compasssearchoptimizer.h"
#include "optimizers/case.h"
#include "model/model.h"
#include "model/adjointscoupledmodel.h"
#include "model/coupledmodel.h"
#include "fileio/readeres/driverreader.h"
#include "parallelprinter.h"
#include "broker.h"
#include "resultslogger.h"

namespace mpi = boost::mpi;

/*!
 * \brief The MasterRunner class is responsible for controlling the program flow.
 * This includes as well as perturbing, distributing and comparing models.
 *
 * This class perturbs the initial model and distributes ModelPerturbation objects to SimulationLaunchers.
 * It also receives SimulationResult objects from SimulationLaunchers, compares them, and determines the next set of perturbations.
 */
class MasterRunner
{
public:
    MasterRunner(mpi::communicator *comm);
    ~MasterRunner(){}

    void initialize(QString driverPath);
    void start();

private:
    QString driverPath;
    Model* model;
    mpi::communicator* world;
    DriverReader* driverReader;
    ParallelPrinter* printer;
    Optimizer* opt;
    Broker* broker;
    int perturbationId;
    QVector<int> getCaseIds(int length);
    ResultsLogger* logger;

};

#endif // MASTERRUNNER_H
