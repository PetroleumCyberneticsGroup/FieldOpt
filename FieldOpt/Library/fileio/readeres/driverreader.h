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

#ifndef DRIVERREADER_H
#define DRIVERREADER_H

#include <QString>
#include <QStringList>
#include <QFile>
#include <QVector>
#include <tr1/memory>
#include <QFileInfo>
#include <QDir>

#include "model/model.h"
#include "model/coupledmodel.h"
#include "model/decoupledmodel.h"
#include "model/adjointscoupledmodel.h"

#include "model/stream.h"
#include "model/wellcontrol.h"
#include "model/wellconnection.h"
#include "model/wellconnectionvariable.h"
#include "model/wellpath.h"
#include "model/pipeconnection.h"
#include "model/reservoir.h"

#include "model/components/well.h"
#include "model/components/productionwell.h"
#include "model/components/injectionwell.h"
#include "model/components/pipe.h"
#include "model/components/midpipe.h"
#include "model/components/endpipe.h"
#include "model/components/separator.h"
#include "model/components/pressurebooster.h"

#include "model/objectives/objective.h"
#include "model/objectives/npvobjective.h"
#include "model/objectives/cumoilobjective.h"
#include "model/objectives/cumgasobjective.h"

#include "constraints/constraint.h"
#include "constraints/userconstraint.h"
#include "constraints/capacity.h"

#include "variables/intvariable.h"
#include "variables/realvariable.h"
#include "variables/binaryvariable.h"

#include "utilities/stringutilities.h"

#include "model/cost.h"

#include "pressuredropcalculators/beggsbrillcalculator.h"

/// #include "gprssimulator.h" \todo Implement gprssimulator

/// \todo Implement this: #include "optimizers/runonceoptimizer.h"
/// \todo Implement this: #include "optimizers/bonminoptimizer.h"
/// \todo Implement this: #include "optimizers/nomadoptimizer.h"
/// \todo Implement this: #include "optimizers/ipoptoptimizer.h"
/// \todo Implement this: #include "optimizers/lshoptimizer.h"
/// \todo Implement this: #include "optimizers/nomadipoptoptimizer.h"
/// \todo Implement this: #include "optimizers/eroptoptimizer.h"

/// \todo Implement this: #include "simulators/gprssimulator.h"
/// \todo Implement this: #include "simulators/vlpsimulator.h"
#include "simulators/mrstbatchsimulator.h"

#include "reader.h"

class Model;
class Well;
class ProductionWell;
class Reservoir;
class Pipe;
class MidPipe;
class Objective;
class Capacity;
class WellPath;

using std::tr1::shared_ptr;

/*!
 * \brief Reads the driver file and creates a Model object.
 *
 * \todo Expand documentation.
 *
 * \todo This class includes the Runner class in ResOpt. Might need adjustments.
 *
 * \todo This class does way too much logic work.
 *
 * \todo This class used to take Runner as a parameter to the readOptimizer() function. The fields this used to set in the Runner is moved to the Model. This needs to be accounted for.
 *
 * \todo Optimizer used to be created here. This needs to be done elsewhere.
 *
 * \todo Simulator used to be created here. This needs to be done elsewhere.
 */
class DriverReader : Reader
{
private:
    QString m_driver_file_name;
    QFile m_driver_file;
    QString m_path;

    QStringList processLine(const QString& line);
    bool isEmpty(const QStringList &list);

    QVector<double> readMasterSchedule();  //!< Reads the MASTERSCHEDULE part of the driver file

    /*!
     * \brief Reads a RESERVOIR section of the driver file.
     *
     * Generates a Reservoir object based on the information in the driver file.
     * This object should be sent to a Model for storage.
     *
     * \return Reservoir Pointer to the generated Reservoir
     */
    Reservoir* readReservoir();
    Well* readWell();  //!< Reads a WELL section of the driver file


    /*!
     * \brief Reads the SCHEDULE part of a well definition.
     *
     * \param w pointer to the well where the schedule should be added
     * \return true if everything went ok, false if errors were enocuntered
     */
    bool readWellSchedule(Well *w);

    /*!
     * \brief Reads the GASLIFT part of a well definition.
     *
     * \param w pointer to the well where the gas lift schedule should be added
     * \return true if everything went ok, false if errors were enocuntered
     */
    bool readGasLiftSchedule(ProductionWell *w);


    /*!
     * \brief Reads the CONNECTIONS part of a well definition
     *
     * \param w pointer to the well where the connections should be added
     * \return true if everything went ok, false if errors were enocuntered
     */
    bool readWellConnections(Well *w);
    bool readVariableWellConnections(Well *w);
    bool readWellPath(Well *w);
    bool readWellPathHeel(WellPath *wp, Well *w);
    bool readWellPathToe(WellPath *wp, Well *w);
    bool readWellPathOptions(WellPath *wp, Well *w);

    bool readPipeConnections(ProductionWell *w);  //!< Reads the OUTLETPIPES part of a well definition
    bool readPipeConnections(MidPipe *p);         //!< Reads the OUTLETPIPES part of a pipe definition
    Objective* readObjective();                   //!< Reads the objective definition in the driver file
    Pipe* readPipe();                             //!< Reads a PIPE definition from the driver file
    Capacity* readCapacity();                     //!< Reads a CAPACITY definition from the driver file
    Pipe* readSeparator();                        //!< Reads a SEPARATOR definition from the driver file
    Pipe* readPressureBooster();                  //!< Reads a BOOSTER definition from the driver file
    void readOptimizer(Model *m);                //!< Reads the optimizer section of the driver file, and sets the optimizer for the runner.
    void readUserDefinedConstraints(Model *m);    //!< Reads the CONSTRAINTS section of the driver file

public:
    DriverReader(const QString &driver);                           //!< Default constructor. Sets the driver file path.
    Model* readDriverFile();                             //!< Read the driver file, and sets up all the data structures
    void setDriverFilePath(const QString &path) {m_path = path;}  //!< Set the driver file path.
    const QString& driverFilePath() {return m_path;}              //!< Get the driver file path.
};

#endif // DRIVERREADER_H
