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

#ifndef MRSTBATCHSIMULATOR_H
#define MRSTBATCHSIMULATOR_H

#include "simulator.h"
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QDir>

#include "model/components/well.h"
#include "model/components/injectionwell.h"
#include "model/components/productionwell.h"
#include "model/wellcontrol.h"
#include "model/model.h"
#include "model/adjointscoupledmodel.h"
#include "model/reservoir.h"
#include "model/wellconnection.h"
#include "model/wellcontrol.h"
#include "model/wellpath.h"
#include "variables/realvariable.h"
#include "variables/intvariable.h"
#include "model/stream.h"
#include "optimizers/adjoint.h"
#include "optimizers/adjointcollection.h"
#include "utilities/posixutilities.h"

/*!
 * \brief Interface for the MRST reservoir simulator.
 *
 * \todo The file writing should be moved to a separate class.
 */
class MrstBatchSimulator : public Simulator
{
private:

    bool m_first_launch;                                       //!< Indicates whether or not this is the first simulation launch. It is set to true during initialization, and it is set to false after generating input files.
    int run_number;                                            //!< Indicates the current run number.
    QString m_matlab_path;                                     //!< Path to the matlab executable.
    QString m_script;                                          //!< Path to matlab script. \todo Why is this set to test2?

    bool generateControlInputFile(Model *m);                   //!< Generate a control input file. Called when using the auto-generated MRST script.
    bool generateEclIncludeFile(Model *m);                     //!< Generate the ECL include file. \todo Could be used for general ECL printing.
    bool generateMRSTScript(Model *m, bool adjoints = false);  //!< Generate the MRST Run2 function.
    void writeWellPaths(Model *m);                             //!< Write well paths to file if they are present.
    bool readAdjoints(AdjointsCoupledModel *m);                //!< Read the adjoint output file.
    bool readStandardOutput(Model *m);                         //!< Read the standard output file.
    bool generateMRSTScriptAdjoints(QTextStream *out_mrst);    //!< Generate the Adjoints part of the MRST runSim2 function. \todo Should this be Run2, lige for the generateMRSTScript function?

public:
    MrstBatchSimulator();                             //!< Default constructor. Sets m_first_launch to true, run number to 1 and m_script to test2.
    MrstBatchSimulator(const MrstBatchSimulator &m);  //!< Copy constructor. Calls the Simulator constructor with the simulator as parameter, then does the same as the default constructor.
    virtual ~MrstBatchSimulator(){}                   //!< Default destructor.

    virtual Simulator* clone() const {return new MrstBatchSimulator(*this);}  //!< Get a copy of this simulator.
    virtual QString description() const {return QString("SIMULATOR MRST_BATCH\n\n");}  //!< Get a description of the simulator.
    virtual bool generateInputFiles(Model *m);
    virtual bool launchSimulator();
    virtual bool readOutput(Model *m);  //!< Read simulator output files.
};

#endif // MRSTBATCHSIMULATOR_H
