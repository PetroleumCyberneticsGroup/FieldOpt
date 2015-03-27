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

#include "driverreader.h"

DriverReader::DriverReader(const QString &driver)
    : m_driver_file_name(driver)
{}


Model* DriverReader::readDriverFile()
{
    if(m_driver_file_name.isEmpty())
        emitException(ExceptionSeverity::ERROR, ExceptionType::FILE_NOT_FOUND, QString("Could not open driver file."));

    m_driver_file.setFileName(m_driver_file_name);

    if(!m_driver_file.open(QIODevice::ReadOnly | QIODevice::Text))
        emitException(ExceptionSeverity::ERROR, ExceptionType::FILE_NOT_FOUND, QString("Could not open driver file."));

    QFileInfo info(m_driver_file);
    m_path = info.absoluteDir().absolutePath();
    printProgress(QString("Path to driver file: %1").arg(m_path.toLatin1().constData()));

    Model *p_model = 0;
    QStringList list;
    bool ok = true;
    list = StringUtilities::processLine(m_driver_file.readLine());

    // the first keyword in the driver file must specify the type of model (COUPLED, DECOUPLED)
    bool found_model_def = false;
    while(!found_model_def && !m_driver_file.atEnd() && !list.at(0).startsWith("EOF"))
    {
        if(list.at(0).startsWith("COUPLED"))
        {
            printProgress(QString("Generating a COUPLED model."));
            p_model = new CoupledModel();
            found_model_def = true;
        }
        else if(list.at(0).startsWith("DECOUPLED"))
        {
            printProgress(QString("Generating a DECOUPLED model."));
            p_model = new DecoupledModel();
            found_model_def = true;
        }
        else if(list.at(0).startsWith("ADJOINTS_COUPLED"))
        {
            printProgress(QString("Generating a COUPLED model with ADJOINTS."));
            AdjointsCoupledModel *am = new AdjointsCoupledModel();
            p_model = am;
            found_model_def = true;
        }
        else
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("The first keyword must specify the model type.\n Possible types are COUPLED, DECOUPLEDM ADJOINTS_COUPLED.\n Last line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        list = StringUtilities::processLine(m_driver_file.readLine());
    }  // Read model type

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("EOF"))
    {
        if(list.at(0).startsWith("START")) // the start of a new well or reservoir
        {
            if(list.at(1).startsWith("RESERVOIR")) p_model->setReservoir(readReservoir());                      // reservoir
            else if(list.at(1).startsWith("WELL")) p_model->addWell(readWell());                                // new well
            else if(list.at(1).startsWith("OBJECTIVE")) p_model->setObjective(readObjective());                 // objective
            else if(list.at(1).startsWith("PIPE")) p_model->addPipe(readPipe());                                // pipe
            else if(list.at(1).startsWith("CAPACITY")) p_model->addCapacity(readCapacity());                    // capacity
            else if(list.at(1).startsWith("OPTIMIZER")) readOptimizer(p_model);                                 // optimizer
            else if(list.at(1).startsWith("MASTERSCHEDULE")) p_model->setMasterSchedule(readMasterSchedule());  // master schedule
            else if(list.at(1).startsWith("SEPARATOR")) p_model->addPipe(readSeparator());                      // separator
            else if(list.at(1).startsWith("BOOSTER")) p_model->addPipe(readPressureBooster());                  // booster
            else if(list.at(1).startsWith("CONSTRAINTS")) readUserDefinedConstraints(p_model);                  // user defined constraints
        }
        else if(list.at(0).startsWith("DEBUG"))
        {
            p_model->getRuntimeSettings()->setDebugFilename(m_path + "/" + list.at(1));
        }
        else if(list.at(0).startsWith("SIMULATOR"))     // reading the type of reservoir simulator to use
        {
            emitException(ExceptionSeverity::WARNING, ExceptionType::PROGRESS, QString("Reading simulator type. Line: %1").arg(list.at(1)));
            if(list.at(1).startsWith("GPRS"))
                p_model->getRuntimeSettings()->getSimulatorSettings()->setSimulator(GPRS);
            else if(list.at(1).startsWith("VLP"))
                p_model->getRuntimeSettings()->getSimulatorSettings()->setSimulator(VLP);
            else if(list.at(1).startsWith("MRST_BATCH")){
                emitException(ExceptionSeverity::WARNING, ExceptionType::PROGRESS, "Detected MRST setting in driver file.");
                p_model->getRuntimeSettings()->getSimulatorSettings()->setSimulator(MRST);
            }
            else
            {
                QString message = QString("Type of SIMULATOR not understood.\nPossible types are: GPRS, MRST_BATCH, VLP\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        else
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("Keyword: %1\nNot understood in current context.").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }

        if(!ok) break;
        list = StringUtilities::processLine(m_driver_file.readLine());
    }
    if(ok)
        printProgress(QString("Done reading input. All OK."));
    else
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, QString("Malformed input."));
    // setting the driver file path to the model
    p_model->setDriverPath(m_path);
    return p_model;
}


QVector<double> DriverReader::readMasterSchedule()
{
    printProgress(QString("Reading master schedule."));
    QStringList list;
    QVector<double> l_schedule;
    bool ok = true;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {
        if(!StringUtilities::isEmpty(list))
            l_schedule.push_back(list.at(0).toDouble(&ok));
        if(!ok) break;
        list = StringUtilities::processLine(m_driver_file.readLine());
    }


    if(!ok)  // checking if the numbers were read ok
    {
        QString message = QString("MASTERSCHEDULE is not in the right format. \nLast line: %1").arg(list.join(" ").toLatin1().constData());
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
    }

    // checking that the schedule is in ascending order
    for(int i = 1; i < l_schedule.size(); ++i)
    {
        if(l_schedule.at(i-1) >= l_schedule.at(i))
        {
            QString message = QString("MASTERSCHEDULE is not in ascending order.\n %1 >= %2").arg(l_schedule.at(i-1)).arg(l_schedule.at(i));
            emitException(ExceptionSeverity::ERROR, ExceptionType::INCONSISTENT, message);
        }
    }
    return l_schedule;
}


Reservoir* DriverReader::readReservoir()
{
    printProgress("Reading reservoir derfinition ...");

    Reservoir *res = new Reservoir();
    QStringList list;
    QString l_name = "";
    QString l_file = "";
    double l_endtime = -1.0;
    bool ok = true;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {
        if(list.at(0).startsWith("NAME")) l_name = list.at(1);                          // getting the reservoir name
        else if(list.at(0).startsWith("FILE")) l_file = list.at(1);                     // getting the file name
        else if(list.at(0).startsWith("MRST")) res->setMrstPath(list.at(1));            // setting the MRST path
        else if(list.at(0).startsWith("MATLAB")) res->setMatlabPath(list.at(1));        // setting the Matlab path
        else if(list.at(0).startsWith("SCRIPT")) res->setMrstScript(list.at(1));        // setting a custom MRST script
        else if(list.at(0).startsWith("KEEP_MAT_FILE")) res->setKeepMatFile(true);      // setting that the .mat file should not be deleted between runs
        else if(list.at(0).startsWith("TIME")) l_endtime = list.at(1).toDouble(&ok);    // getting the file name
        else if(list.at(0).startsWith("PHASES"))                                        // getting the phases present in the reservoir
        {
            if(list.length() != 4) // wrong number of arguments
            {
                QString message = QString("Error detected in input file.\nPHASES keyword is not in the right format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
            bool l_gas = false;
            bool l_oil = false;
            bool l_wat = false;
            if(list.at(1).toInt() == 1) l_gas = true;
            if(list.at(2).toInt() == 1) l_oil = true;
            if(list.at(3).toInt() == 1) l_wat = true;
            res->setPhases(l_gas, l_oil, l_wat);        // setting the phases to reservoir
        }

        else if(list.at(0).startsWith("DENS"))                         // getting the phase densities
        {
            if(list.length() != 4) // wrong number of arguments
            {
                QString message = QString("Error detected in input file.\nDENS keyword is not in the right format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
            double l_gas = list.at(1).toDouble();
            double l_oil = list.at(2).toDouble();
            double l_wat = list.at(3).toDouble();
            res->setDensities(l_gas, l_oil, l_wat);     // setting the phase densities to the reservoir
        }

        else
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("Error detected in input file.\nKeyword: %1 \nNot understood in current context.").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        list = StringUtilities::processLine(m_driver_file.readLine());
    }

    if(l_name.compare("") == 0)
    {
        QString message = QString("Error detected in input file.\nThe reservoir NAME was not defined.");
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
    }
    if(l_file.compare("") == 0)
    {
        QString message = QString("Error detected in input file.\nThe reservoir FILE was not defined.");
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
    }

    if(!ok || l_endtime < 0)
    {
        QString message = QString("Error detected in input file.\nThe reservoir TIME was not defined.");
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
    }
    // everything ok, setting to reservoir
    res->setName(l_name);
    res->setFile(l_file);
    res->setEndTime(l_endtime);
    return res;
}


Well* DriverReader::readWell()
{
    printProgress("Reading well definition ...");

    Well *w = 0;
    QStringList list;
    QString l_name = "";
    QString l_group = "";
    double l_bhp_limit = 0.0;
    WellControl::control_type l_bhp_inj = WellControl::QWAT;
    bool ok_bhp = false;
    Cost *well_cost = 0;
    shared_ptr<IntVariable> var_install;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {
        if(list.at(0).startsWith("NAME")) l_name = list.at(1);                                      // getting the reservoir name
        else if(list.at(0).startsWith("GROUP")) l_group = list.at(1);                               // getting the file name
        else if(list.at(0).startsWith("BHPLIMIT")) l_bhp_limit = list.at(1).toDouble(&ok_bhp);      // getting the BHP limit
        else if(list.at(0).startsWith("COST"))                                                      // the cost
        {
            bool ok_cost = true;
            double cost = list.at(1).toDouble(&ok_cost);

            if(!ok_cost)
            {
                QString message = QString("Error detected in input file.\nCould not convert COST to number.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
            well_cost = new Cost();
            well_cost->setConstant(cost);
        }
        else if(list.at(0).startsWith("INSTALLTIME"))                                           // getting the installation time
        {
            var_install = shared_ptr<IntVariable>(new IntVariable(w));
            bool ok = true;

            if(list.size() == 2) // not a variable, only starting value specified
            {
                int value = list.at(1).toInt(&ok);
                var_install->setValue(value);
                var_install->setMax(value);
                var_install->setMin(value);
            }
            else if(list.size() == 4)   // value, max, and min specified
            {
                int value, max, min = 0;
                bool ok1, ok2, ok3 = true;
                value = list.at(1).toInt(&ok1);
                max = list.at(2).toInt(&ok2);
                min = list.at(3).toInt(&ok3);
                var_install->setValue(value);
                var_install->setMax(max);
                var_install->setMin(min);

                if(!ok1 || !ok2 || !ok3) ok = false;
            }
            else
            {
                QString message = QString("Error detected in input file.\nINSTALLTIME for WELL not in correct format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }

            if(!ok)
            {
                QString message = QString("Error detected in input file.\nINSTALLTIME for WELL not in correct format.\nProblem detected when converting numbers.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }

        }

        else if(list.at(0).startsWith("BHPINJ"))                                                    // getting the default injection phase
        {
            if(list.at(1).startsWith("WATER")) l_bhp_inj = WellControl::QWAT;
            else if(list.at(1).startsWith("GAS")) l_bhp_inj = WellControl::QGAS;
            else if(list.at(1).startsWith("OIL")) l_bhp_inj = WellControl::QOIL;
        }
        else if(list.at(0).startsWith("TYPE"))                                                      // getting the type of well
        {
            if(list.at(1).startsWith("P"))         // producer
            {
                ProductionWell *prod_well = new ProductionWell();
                if(l_name.compare("") != 0) prod_well->setName(l_name);
                w = prod_well;
                w->setAutomaticType();
            }
            else if(list.at(1).startsWith("I"))    // injector
            {
                w = new InjectionWell();
                w->setAutomaticType();
                if(l_name.compare("") != 0) w->setName(l_name);
            }
            else                                                        // not recognized
            {
                QString message = QString("Error detected in input file.\nWell TYPE: %1\nis not a recognized well type.").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }


        else if(list.at(0).startsWith("START"))                             // A subsection (connections, schedule)
        {
            // first checking that the well has been initialized
            if(w == 0)
            {
                QString message = QString("Error detected in input file.\nThe well TYPE must be specified before any of the START sections.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }

            if(list.at(1).startsWith("SCHEDULE")) readWellSchedule(w);                  // reading SCHEDULE
            else if(list.at(1).startsWith("CON")) readWellConnections(w);               // reading CONNECTIONS
            else if(list.at(1).startsWith("VARCON")) readVariableWellConnections(w);    // reading VARCONNECTIONS
            else if(list.at(1).startsWith("WELLPATH")) readWellPath(w);                 // reading WELLPATH
            else if(list.at(1).startsWith("OUTLETPIPE"))                                // reading OUTLETPIPES
            {
                // checking if this is a production well
                ProductionWell *prod_well = dynamic_cast<ProductionWell*>(w);
                if(prod_well == 0)
                {
                    QString message = QString("Error detected in input file.\nOUTLETPIPES can not be defined for INJECTION wells.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                    emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
                }
                // getting the pipe connections
                readPipeConnections(prod_well);
            }
            else if(list.at(1).startsWith("GASLIFT"))                    // reading GASLIFT
            {
                // checking if this is a production well
                ProductionWell *prod_well = dynamic_cast<ProductionWell*>(w);
                if(prod_well == 0)
                {
                    QString message = QString("Error detected in input file.\nGASLIFT can not be defined for INECTION wells.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                    emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
                }
                // getting the pipe connections
                readGasLiftSchedule(prod_well);
            }
            else                                                            // unknown keyword
            {
                QString message = QString("Error detected in input file.\nKeyword: %1\nNot understood in current context.").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        else
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("Error detected in input file.\nKeyword: %1\nNot understood in current context.").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }


        list = StringUtilities::processLine(m_driver_file.readLine());

    }

    // checking remaining input

    if(l_name.compare("") == 0)
    {
        QString message = QString("Error detected in input file.\nThe well NAME was not defined.");
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
    }
    if(l_group.compare("") == 0)
    {
        QString message = QString("Error detected in input file.\nThe well GROUP was not defined.");
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
    }
    if(!ok_bhp)
    {
        QString message = QString("Error detected in input file.\nThe well BHPLIMIT was not defined.\nAssuming BHPLIMIT of 15 psia.");
        emitException(ExceptionSeverity::WARNING, ExceptionType::UNABLE_TO_PARSE, message);
        l_bhp_limit = 15.0;
    }
    if(l_name.contains("_"))
    {
        QString message = QString("Error detected in input file.\nThe well NAME can not contain underscore (_). \nWell name: %1").arg(l_name.toLatin1().constData());
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
    }

    // everything ok, setting to well
    w->setName(l_name);
    w->setGroup(l_group);
    w->setBhpLimit(l_bhp_limit);
    w->setBhpInj(l_bhp_inj);
    if(well_cost != 0) w->setCost(well_cost);
    if(var_install != 0)
    {
        var_install->setName("Installation time variable for well: " + l_name);
        w->setInstallTime(var_install);
    }
    return w;
}


bool DriverReader::readWellSchedule(Well *w)
{
    printProgress("       Reading well schedule...");

    QStringList list;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {
        if(list.size() == 5)        // correct number of elements (t_end, value, max, min, type)
        {
            // checking if the line is in the right format
            QVector<double> nums;
            bool ok = true;

            for(int i = 0; i < 4; i++)  // looping through t_end, value, max, min
            {
                nums.push_back(list.at(i).toDouble(&ok));
                if(!ok) break;
            }
            // got all the numbers ok
            if(ok)
            {
                shared_ptr<RealVariable> var(new RealVariable(w));
                var->setValue(nums.at(1));
                var->setMax(nums.at(2));
                var->setMin(nums.at(3));
                var->setName("Control variable for well: " + w->name() + ", until time = " + QString::number(nums.at(0)));

                WellControl *control = new WellControl();
                control->setEndTime(nums.at(0));
                control->setControlVar(var);

                // checking the "type"
                if(list.at(4).startsWith("OIL")) control->setType(WellControl::QOIL);
                else if(list.at(4).startsWith("GAS")) control->setType(WellControl::QGAS);
                else if(list.at(4).startsWith("WAT")) control->setType(WellControl::QWAT);
                else if(list.at(4).startsWith("BHP")) control->setType(WellControl::BHP);
                else
                {
                    QString message = QString("Error detected in input file.\nThe SCHEDULE type was not recognized.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                    emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
                }
                w->addControl(control);
            }
            else     // error when reading numbers in the schedule line
            {
                QString message = QString("Error detected in input file.\nThe SCHEDULE entry could not be read.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }

        else        // wrong number of arguments on line
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("Error detected in input file.\nThe SCHEDULE entry does not have the right format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        list = StringUtilities::processLine(m_driver_file.readLine());
    }
    return true;
}


bool DriverReader::readGasLiftSchedule(ProductionWell *w)
{
    printProgress("       Reading gas lift schedule.");

    QStringList list;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {
        if(list.size() == 4)        // correct number of elements (t_end, value, max, min)
        {
            // checking if the line is in the right format
            QVector<double> nums;
            bool ok = true;

            for(int i = 0; i < 4; i++)  // looping through t_end, value, max, min
            {
                nums.push_back(list.at(i).toDouble(&ok));
                if(!ok) break;
            }

            // got all the numbers ok
            if(ok)
            {
                shared_ptr<RealVariable> var(new RealVariable(w));
                var->setValue(nums.at(1));
                var->setMax(nums.at(2));
                var->setMin(nums.at(3));
                var->setName("Gas lift variable for well: " + w->name() + ", until time = " + QString::number(nums.at(0)));

                WellControl *control = new WellControl();
                control->setEndTime(nums.at(0));
                control->setType(WellControl::QGAS);
                control->setControlVar(var);
                w->addGasLiftControl(control);
            }

            else     // error when reading numbers in the schedule line
            {
                QString message = QString("Error detected in input file.\nThe GASLIFT entry could not be read.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }

        else        // wrong number of arguments on line
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("Error detected in input file.\nThe GASLIFT entry does not have the right format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        list = StringUtilities::processLine(m_driver_file.readLine());
    }
    return true;
}


bool DriverReader::readWellConnections(Well *w)
{
    printProgress("       Reading connections table.");

    QStringList list;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {
        if(list.size() == 2)        // correct number of elements (cell, wi)
        {
            // checking if the line is in the right format
            QVector<double> nums;
            bool ok = true;

            for(int i = 0; i < 2; i++)  // looping through cell, wi
            {
                nums.push_back(list.at(i).toDouble(&ok));
                if(!ok) break;
            }

            // got all the numbers ok
            if(ok)
            {
                WellConnection *con = new WellConnection();
                con->setCell(nums.at(0));
                con->setWellIndex(nums.at(1));
                w->addConnection(con);
            }
            else     // error when reading numbers in the connections line
            {
                QString message = QString("Error detected in input file.\nThe CONNECTIONS entry could not be read.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        else if(list.size() == 5) // correct number of elements (i, j, k1, k2, PI)
        {
            // checking the numbers
            QVector<double> nums;
            bool ok = true;

            for(int i = 0; i < 5; ++i)
            {
                nums.push_back(list.at(i).toDouble(&ok));
                if(!ok) break;
            }

            // got all the numbers ok
            if(ok)
            {
                WellConnection *con = new WellConnection();
                con->setI(nums.at(0));
                con->setJ(nums.at(1));
                con->setK1(nums.at(2));
                con->setK2(nums.at(3));
                con->setWellIndex(nums.at(4));

                w->addConnection(con);
            }
            else     // error when reading numbers in the connections line
            {
                QString message = QString("Error detected in input file.\nThe CONNECTIONS entry could not be read.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }

        else        // wrong number of arguments on line
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("Error detected in input file.\nThe CONNECTIONS entry does not have the right format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        list = StringUtilities::processLine(m_driver_file.readLine());
    }
    return true;
}


bool DriverReader::readVariableWellConnections(Well *w)
{
    printProgress("       Reading variable connections table.");

    QStringList list;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {
        if(list.size() == 9) // correct number of elements (i, i_max, i_min, j, j_max, j_min, k1, k2, PI)
        {
            // checking the numbers
            QVector<double> nums;
            bool ok = true;

            for(int i = 0; i < 9; ++i)
            {
                nums.push_back(list.at(i).toDouble(&ok));
                if(!ok) break;
            }

            // got all the numbers ok
            if(ok)
            {
                WellConnectionVariable *wcv = new WellConnectionVariable();

                shared_ptr<IntVariable> i_var(new IntVariable(w));
                i_var->setValue(nums.at(0));
                i_var->setMax(nums.at(1));
                i_var->setMin(nums.at(2));
                wcv->setIVariable(i_var);

                shared_ptr<IntVariable> j_var(new IntVariable(w));
                j_var->setValue(nums.at(3));
                j_var->setMax(nums.at(4));
                j_var->setMin(nums.at(5));
                wcv->setJVariable(j_var);

                wcv->setK1(nums.at(6));
                wcv->setK2(nums.at(7));
                wcv->setWellIndex(nums.at(8));

                w->addVariableConnection(wcv);

            }
            else     // error when reading numbers in the connections line
            {
                QString message = QString("Error detected in input file.\nThe VARCONNECTIONS entry could not be read.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        else        // wrong number of arguments on line
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("Error detected in input file.\nThe VARCONNECTIONS entry does not have the right format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        list = StringUtilities::processLine(m_driver_file.readLine());
    }
    return true;
}


bool DriverReader::readWellPath(Well *w)
{
    printProgress("       Reading well path.");

    bool ok = true;
    WellPath *wp = new WellPath();
    QStringList list;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END") && !list.at(1).startsWith("WELLPATH"))
    {
        if(list.at(0).startsWith("START") && list.at(1).startsWith("TOE")) readWellPathToe(wp, w);
        else if(list.at(0).startsWith("START") && list.at(1).startsWith("HEEL")) readWellPathHeel(wp, w);
        else if(list.at(0).startsWith("START") && list.at(1).startsWith("OPTIONS")) readWellPathOptions(wp, w);

        else
        {
            if(!StringUtilities::isEmpty(list))
            {

                QString message = QString("Error detected in input file.\nThe WELLPATH entry does not have the right format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        list = StringUtilities::processLine(m_driver_file.readLine());
    }


    if(!wp->initialize())
    {
        emitException(ExceptionSeverity::WARNING, ExceptionType::MODEL_VALIDATION, "WELLPATH did not initialize");
        ok = false;
    }

    if(ok)
    {
        w->setWellPath(wp);
    }

    else
    {
        QString message = QString("Error detected in input file.\nThe WELLPATH entry does not have the right format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
    }
}


bool DriverReader::readWellPathToe(WellPath *wp, Well *w)
{
    printProgress("       Reading toe.");
    QStringList list;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {
        if(list.size() == 4) // correct number of elements (type, value, max, min)
        {
            bool ok;

            // checking the type
            if(list.at(0).startsWith("I") || list.at(0).startsWith("J") || list.at(0).startsWith("K")) ok = true;
            else ok = false;

            // checking the numbers
            QVector<double> nums;

            for(int i = 1; i < 4; ++i)
            {
                nums.push_back(list.at(i).toDouble(&ok));
                if(!ok) break;
            }

            // all ok
            if(ok)
            {
                shared_ptr<IntVariable> v(new IntVariable(w));
                v->setValue(nums.at(0));
                v->setMax(nums.at(1));
                v->setMin(nums.at(2));

                if(list.at(0).startsWith("I"))
                {
                    wp->setToeI(v);
                }
                else if(list.at(0).startsWith("J"))
                {
                    wp->setToeJ(v);
                }
                else if(list.at(0).startsWith("K"))
                {
                    wp->setToeK(v);
                }

            }
            else     // error when reading
            {
                QString message = QString("Error detected in input file.\nThe WELLPATH TOE entry could not be read.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }

        else        // wrong number of arguments on line
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("Error detected in input file.\nThe WELLPATH TOE entry does not have the right format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        list = StringUtilities::processLine(m_driver_file.readLine());
    }
    return true;
}


bool DriverReader::readWellPathHeel(WellPath *wp, Well *w)
{
    printProgress("       Reading heel.");
    QStringList list;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {
        if(list.size() == 4) // correct number of elements (type, value, max, min)
        {
            bool ok;

            // checking the type
            if(list.at(0).startsWith("I") || list.at(0).startsWith("J") || list.at(0).startsWith("K")) ok = true;
            else ok = false;

            // checking the numbers
            QVector<double> nums;

            for(int i = 1; i < 4; ++i)
            {
                nums.push_back(list.at(i).toDouble(&ok));
                if(!ok) break;
            }
            // all ok
            if(ok)
            {
                shared_ptr<IntVariable> v(new IntVariable(w));
                v->setValue(nums.at(0));
                v->setMax(nums.at(1));
                v->setMin(nums.at(2));

                if(list.at(0).startsWith("I")) wp->setHeelI(v);
                else if(list.at(0).startsWith("J")) wp->setHeelJ(v);
                else if(list.at(0).startsWith("K")) wp->setHeelK(v);
            }
            else     // error when reading
            {
                QString message = QString("Error detected in input file.\nThe WELLPATH HEEL entry could not be read.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        else        // wrong number of arguments on line
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("Error detected in input file.\nThe WELLPATH HEEL entry does not have the right format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        list = StringUtilities::processLine(m_driver_file.readLine());
    }
    return true;
}


bool DriverReader::readWellPathOptions(WellPath *wp, Well *w)
{
    printProgress("       Reading wellpath options.");

    QStringList list;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {
        if(list.size() == 4) // variable (name, value, max, min)
        {
            bool ok;
            // checking the numbers
            QVector<double> nums;

            for(int i = 1; i < 4; ++i)
            {
                nums.push_back(list.at(i).toDouble(&ok));
                if(!ok) break;
            }
            // all ok
            if(ok)
            {
                shared_ptr<IntVariable> v(new IntVariable(w));
                v->setValue(nums.at(0));
                v->setMax(nums.at(1));
                v->setMin(nums.at(2));
                v->setName(list.at(0));
                wp->addOptionVariable(v);
            }
            else     // error when reading
            {
                QString message = QString("Error detected in input file.\nThe WELLPATH OPTION entry could not be read.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }

        else if(list.size() == 2) // constant (name, value)
        {
            bool ok= true;

            // checking the number
            double value = list.at(1).toDouble(&ok);

            // all ok
            if(ok)
            {
                wp->addOptionConstant(list.at(0), value);
            }
            else     // error when reading
            {
                QString message = QString("Error detected in input file.\nThe WELLPATH OPTION entry could not be read.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        else        // wrong number of arguments on line
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("Error detected in input file.\nThe WELLPATH OPTION entry does not have the right format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        list = StringUtilities::processLine(m_driver_file.readLine());
    }
    return true;
}


bool DriverReader::readPipeConnections(ProductionWell *w)
{
    printProgress("       Reading outlet pipes table.");

    QStringList list;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {
        if(list.size() == 2 && !StringUtilities::isEmpty(list))        // correct number of elements (pipe#, fraction)
        {
            // checking if the line is in the right format
            bool ok1 = true;
            bool ok2 = true;
            int pipe_num = list.at(0).toInt(&ok1);
            double frac = list.at(1).toDouble(&ok2);
            // got all the numbers ok
            if(ok1 && ok2)
            {
                PipeConnection *pipe_con = new PipeConnection();
                shared_ptr<BinaryVariable> var(new BinaryVariable(w));

                var->setValue(frac);
                var->setName("Routing variable for well: "+ w->name() + " to Pipe #" + QString::number(pipe_num));
                var->setIsVariable(false);
                pipe_con->setPipeNumber(pipe_num);
                pipe_con->setVariable(var);
                w->addPipeConnection(pipe_con);
            }
            else     // error when reading numbers in the connections line
            {
                QString message = QString("Error detected in input file.\nThe OUTLETPIPES could not be read.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        else if(list.size() == 3 && !StringUtilities::isEmpty(list)) // pipe#, fraction, BIN
        {
            // checking if the third argument is BIN
            if(list.at(2).startsWith("BIN"))
            {
                bool ok1 = true;
                bool ok2 = true;
                int pipe_num = list.at(0).toInt(&ok1);
                double frac = list.at(1).toDouble(&ok2);

                // got all the numbers ok
                if(ok1 && ok2)
                {
                    PipeConnection *pipe_con = new PipeConnection();
                    shared_ptr<BinaryVariable> var(new BinaryVariable(w));

                    var->setValue(frac);
                    var->setName("Routing variable for well: "+ w->name() + " to Pipe #" + QString::number(pipe_num));
                    var->setIsVariable(true);
                    pipe_con->setPipeNumber(pipe_num);
                    pipe_con->setVariable(var);
                    w->addPipeConnection(pipe_con);
                }
                else     // error when reading numbers in the connections line
                {
                    cout << endl << "### Error detected in input file! ###" << endl
                         << "The OUTLETPIPES entry could not be read..." << endl
                         << "Last line: " << list.join(" ").toLatin1().constData() << endl;

                    exit(1);
                }
            }
            else
            {
                QString message = QString("Error detected in input file.\nThe OUTLETPIPES is not in the right format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }

        else        // wrong number of arguments on line
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("Error detected in input file.\nThe OUTLETPIPES does not have the right format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        list = StringUtilities::processLine(m_driver_file.readLine());
    }

    // if the well has more than one connection, they should be considered variables
    if(w->numberOfConnections() > 1)
    {
        for(int i = 0; i < w->numberOfConnections(); ++i)
        {
            w->pipeConnection(i)->variable()->setIsVariable(true);
        }
    }

    // if the well only has one connection, and it is not a variable, the value should be 1.0
    if(w->numberOfConnections() == 1)
    {
        if(!w->pipeConnection(0)->variable()->isVariable()) w->pipeConnection(0)->variable()->setValue(1.0);
    }
    return true;
}


bool DriverReader::readPipeConnections(MidPipe *p)
{
    printProgress("       Reading outlet pipes table.");

    cout << "        outlet pipes table..." << endl;
    QStringList list;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {
        if(list.size() == 2 && !StringUtilities::isEmpty(list))        // correct number of elements (pipe#, fraction)
        {
            // checking if the line is in the right format
            bool ok1 = true;
            bool ok2 = true;
            int pipe_num = list.at(0).toInt(&ok1);
            double frac = list.at(1).toDouble(&ok2);

            // got all the numbers ok
            if(ok1  && ok2)
            {
                PipeConnection *pipe_con = new PipeConnection();
                shared_ptr<BinaryVariable> var(new BinaryVariable(p));

                var->setValue(frac);
                var->setName("Routing variable for Pipe #"+ QString::number(p->number()) + " to Pipe #" + QString::number(pipe_num));
                var->setIsVariable(false);
                pipe_con->setPipeNumber(pipe_num);
                pipe_con->setVariable(var);
                p->addOutletConnection(pipe_con);
            }
            else     // error when reading numbers in the connections line
            {
                QString message = QString("Error detected in input file.\nThe OUTLETPIPES could not be read.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        else if(list.size() == 3 && !StringUtilities::isEmpty(list)) // (pipe#, fraction, BIN)
        {
            // checking that the thrid item is BIN
            if(list.at(2).startsWith("BIN"))
            {
                bool ok1 = true;
                bool ok2 = true;
                int pipe_num = list.at(0).toInt(&ok1);
                double frac = list.at(1).toDouble(&ok2);

                // got all the numbers ok
                if(ok1  && ok2)
                {
                    PipeConnection *pipe_con = new PipeConnection();
                    shared_ptr<BinaryVariable> var(new BinaryVariable(p));

                    var->setValue(frac);
                    var->setName("Routing variable for Pipe #"+ QString::number(p->number()) + " to Pipe #" + QString::number(pipe_num));
                    var->setIsVariable(true);
                    pipe_con->setPipeNumber(pipe_num);
                    pipe_con->setVariable(var);
                    p->addOutletConnection(pipe_con);
                }
                else     // error when reading numbers in the connections line
                {
                    QString message = QString("Error detected in input file.\nThe OUTLETPIPES coukd not be read.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                    emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
                }
            }
            else
            {
                QString message = QString("Error detected in input file.\nThe OUTLETPIPES does not have the right format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }

        else        // wrong number of arguments on line
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("Error detected in input file.\nThe OUTLETPIPES does not have the right format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        list = StringUtilities::processLine(m_driver_file.readLine());
    }

    // if the pipe has more than one connection, they should be considered variables
    if(p->numberOfOutletConnections() > 1)
    {
        for(int i = 0; i < p->numberOfOutletConnections(); ++i)
        {
            p->outletConnection(i)->variable()->setIsVariable(true);
        }
    }

    // if the well only has one connection, and it is not a variable, the value should be 1.0
    if(p->numberOfOutletConnections() == 1)
    {
        if(!p->outletConnection(0)->variable()->isVariable()) p->outletConnection(0)->variable()->setValue(1.0);
    }
    return true;
}


Objective* DriverReader::readObjective()
{
    printProgress("Reading objective definition.");

    Objective *o = 0;
    double l_dcf = 0.0;
    double l_p_oil = 0.0;
    double l_p_gas = 0.0;
    double l_p_wat = 0.0;
    bool ok = true;
    QStringList list;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {
        if(list.at(0).startsWith("TYPE"))              // getting the type of objective
        {
            if(list.at(1).startsWith("NPV")) o = new NpvObjective();                // this is an NPV type objective
            else if(list.at(1).startsWith("CUMOIL")) o = new CumoilObjective();     // this is an CUMOIL type objective
            else if(list.at(1).startsWith("CUMGAS")) o = new CumgasObjective();     // this is an CUMGAS type objective
            else                                                                    // not recognized
            {
                cout << endl << "### Error detected in input file! ###" << endl
                                << "Objective TYPE: " << list.at(1).toLatin1().constData() << endl
                                << "Is not a recognized objective type." << endl << endl;
                exit(1);
            }
        }
        else if(list.at(0).startsWith("DCF")) l_dcf = list.at(1).toDouble(&ok);            // getting the discount factor
        else if(list.at(0).startsWith("OILPRICE")) l_p_oil = list.at(1).toDouble(&ok);     // getting the oil price
        else if(list.at(0).startsWith("GASPRICE")) l_p_gas = list.at(1).toDouble(&ok);     // getting the gas price
        else if(list.at(0).startsWith("WATERPRICE")) l_p_wat = list.at(1).toDouble(&ok);   // getting the water price

        else
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("Error detected in input file.\nKeyword: %1\n Not understood in current context.").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        if(!ok) break;
        list = StringUtilities::processLine(m_driver_file.readLine());
    }
    // checking remaining input

    if(o == 0 || !ok)
    {
        QString message = QString("Error detected in input file.\nThe OBJECTIVE definition is incomplete.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
    }

    // checking if the objective is NPV
    NpvObjective *npv = dynamic_cast<NpvObjective*>(o);
    if(npv != 0)
    {
        npv->setDcf(l_dcf);
        npv->setGasPrice(l_p_gas);
        npv->setOilPrice(l_p_oil);
        npv->setWaterPrice(l_p_wat);
    }
    // everything ok
    return o;
}


Pipe* DriverReader::readPipe()
{
    printProgress("Reading pipe definition.");
    cout << "Reading pipe definition..." << endl;

    Pipe *p = 0;
    int l_number = -1;
    QString l_file = "";
    bool l_mustroute = false;
    bool ok = true;
    QStringList list;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !(list.at(0).startsWith("END") && list.at(1).startsWith("PIPE")))
    {
        if(list.at(0).startsWith("NUMBER")) l_number = list.at(1).toInt(&ok);        // getting the id number of the pipe
        else if(list.at(0).startsWith("FILE")) l_file = list.at(1);                  // getting the file name
        else if(list.at(0).startsWith("MUSTROUTE")) l_mustroute = true;              // setting that the pipe must be routed upstream
        else if(list.at(0).startsWith("OUTLETPRESSURE"))                             // getting the outlet pressure, this is an end pipe
        {
            if(p != 0)  // outlet pipes have allready been defined
            {
                QString message = QString("Error detected in input file.\nThe PIPE definition is overdefined.\nDid you specify both OUTLETPIPES and OUTLETPRESSURE for the pipe?\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
            else
            {
                EndPipe *p_end = new EndPipe();

                double l_outletpressure = list.at(1).toDouble(&ok);
                if(!ok)
                {
                    QString message = QString("Error detected in input file.\nThe OUTLETPRESSURE definition is not in correct format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                    emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
                }
                p_end->setOutletPressure(l_outletpressure);

                // checking if there is a unit definition for the pressure
                if(list.size() > 2)
                {
                    if(list.at(2).startsWith("BAR"))
                    {
                        cout << "Setting OUTLETPRESSURE uniuts to BAR..." << endl;
                        p_end->setOutletUnit(Stream::METRIC);
                    }
                    else if(list.at(2).startsWith("PSI"))
                    {
                        cout << "Setting OUTLETPRESSURE uniuts to PSI..." << endl;
                        p_end->setOutletUnit(Stream::FIELD);
                    }
                }
                p = p_end;
            }
        }
        else if(list.at(0).startsWith("START") && list.at(1).startsWith("OUTLETPIPE"))  // getting outletpipes, this is a mid pipe
        {
            if(p != 0)  // outlet pipes have allready been defined
            {
                QString message = QString("Error detected in input file.\nThe PIPE definition is overdefined.\nDid you specify both OUTLETPIPES and OUTLETPRESSURE for the pipe?\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
            else
            {
                MidPipe *p_mid = new MidPipe();
                if(l_number > 0) p_mid->setNumber(l_number);
                readPipeConnections(p_mid);
                p = p_mid;
            }
        }
        else
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("Error detected in input file.\nKeyword: %1\nNot understood in current context.").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        if(!ok) break;
        list = StringUtilities::processLine(m_driver_file.readLine());
    }

    // checking remaining input
    if(!ok || p == 0)                                             // error with number reading
    {
        QString message = QString("Error detected in input file.\nThe PIPE definition is incomplete.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
    }
    // everything is ok, setting common parameters
    // setting comon parameters
    p->setFileName(m_path + "/" + l_file);
    p->setNumber(l_number);

    if(l_mustroute)
    {
        MidPipe *mp = dynamic_cast<MidPipe*>(p);

        if(mp != 0)
        {
            mp->setMustRoute();
            emitException(ExceptionSeverity::WARNING, ExceptionType::ASSUMPTION, "Setting that the pipe must be routed to an upstream element.");
            cout << "setting that the pipe must be routed to an upstream element..." << endl;
        }
        else
        {
            QString message = QString("Error detected in input file.\nThe PIPE definition is incorrect.\nMUSTROUTE keyword can only be used for pipes that are not top nodes.");
            emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
        }
    }
    return p;
}


Capacity* DriverReader::readCapacity()
{
    printProgress("Reading capacity definition.");

    Capacity *s = new Capacity();
    QVector<int> pipe_nums;
    bool consr_added = false;   // set true when a constraint is added to the separator
    QStringList list;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {
        if(list.at(0).startsWith("NAME")) s->setName(list.at(1));                      // getting the name of the separator
        else if(list.at(0).startsWith("PIPE"))                                      // getting the input pipe numbers
        {
            bool pipe_ok = true;
            for(int i = 1; i < list.size(); i++)
            {
                pipe_nums.push_back(list.at(i).toInt(&pipe_ok));
                if(!pipe_ok) break;
            }
            if(!pipe_ok)
            {
                QString message = QString("Error detected in input file.\nThe CAPACITY input  PIPES were not correcty defined.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        else if(list.at(0).startsWith("WATER"))                     // getting water constraint
        {
            if(list.size() == 2)    // ok format
            {
                bool ok_max = true;
                double max = list.at(1).toDouble(&ok_max);
                if(ok_max)    // got the number
                {
                    s->setMaxWater(max);
                    consr_added = true;
                }
                else                    // error getting the numbers
                {
                    QString message = QString("Error detected in input file.\nThe CAPACITY WATER constraint was not correcty defined.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                    emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
                }
            }
        }
        else if(list.at(0).startsWith("GAS"))                     // getting gas constraint
        {
            if(list.size() == 2)    // ok format
            {
                bool ok_max = true;
                double max = list.at(1).toDouble(&ok_max);
                if(ok_max)    // got the numbers ok
                {
                    s->setMaxGas(max);
                    consr_added = true;
                }
                else                    // error getting the numbers
                {
                    QString message = QString("Error detected in input file.\nThe CAPACITY GAS constraint was not correcty defined.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                    emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
                }
            }
        }
        else if(list.at(0).startsWith("OIL"))                     // getting oil constraint
        {
            if(list.size() == 2)    // ok format
            {
                bool ok_max = true;
                double max = list.at(1).toDouble(&ok_max);
                if(ok_max)    // got the numbers ok
                {
                    s->setMaxOil(max);
                    consr_added = true;
                }
                else                    // error getting the numbers
                {
                    QString message = QString("Error detected in input file.\nThe CAPACITY OIL constraint was not correcty defined.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                    emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
                }
            }
        }
        else if(list.at(0).startsWith("LIQ"))                     // getting liquid constraint
        {
            if(list.size() == 2)    // ok format
            {
                bool ok_max = true;
                double max = list.at(1).toDouble(&ok_max);
                if(ok_max)    // got the numbers ok
                {
                    s->setMaxLiquid(max);
                    consr_added = true;
                }
                else                    // error getting the numbers
                {
                    QString message = QString("Error detected in input file.\nThe CAPACITY LIQUID constraint was not correcty defined.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                    emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
                }
            }
        }
        else
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("Error detected in input file.\nKeyword: %1\nNot understood in current context.").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        list = StringUtilities::processLine(m_driver_file.readLine());
    }
    // checking remaining input
    if(!consr_added)    // no constraint
    {
        QString message = QString("Error detected in input file.\nThe CAPACITY definition is incomplete.\nNo constraint of any kind detected.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
    }
    if(pipe_nums.size() == 0)    // no pipes
    {
        QString message = QString("Error detected in input file.\nThe CAPACITY definition is incomplete.\nNo input PIPES defined.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
    }
    // everything ok
    // adding pipe numbers
    for(int i = 0; i < pipe_nums.size(); i++) s->addFeedPipeNumber(pipe_nums.at(i));

    return s;
}


Pipe* DriverReader::readSeparator()
{
    printProgress("Reading separator definition.");

    Separator *p_sep = new Separator();
    QStringList list;
    bool ok = true;
    int l_number = -1;
    int l_outlet_pipe = -1;
    double l_cost_constant = -1.0;
    double l_cost_fraction = -1.0;
    double l_cost_capacity = -1.0;
    double l_cost_frac_exp = 1.0;
    double l_cost_cap_exp = 1.0;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !(list.at(0).startsWith("END") && list.at(1).startsWith("SEPARATOR")))
    {
        if(list.at(0).startsWith("NUMBER")) l_number = list.at(1).toInt(&ok);                   // getting the id number of the separator
        else if(list.at(0).startsWith("OUTLETPIPE")) l_outlet_pipe = list.at(1).toInt(&ok);     // getting the outlet pipe number
        else if(list.at(0).startsWith("TYPE"))                                                  // getting the type
        {
            if(list.at(1).startsWith("WATER")) p_sep->setType(Separator::WATER);
            else if(list.at(1).startsWith("GAS")) p_sep->setType(Separator::GAS);
            else
            {
                QString message = QString("Error detected in input file.\nSEPARATOR type not recognized. The TYPE can be either WATER or GAS.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        else if(list.at(0).startsWith("COST"))                                                  // getting the cost
        {
            if(list.size() == 4)  // the right number of numbers
            {
                bool ok1, ok2, ok3 = true;
                l_cost_constant = list.at(1).toDouble(&ok1);
                l_cost_fraction = list.at(2).toDouble(&ok2);
                l_cost_capacity = list.at(3).toDouble(&ok3);

                if(!ok1 || !ok2 || !ok3) ok = false;
            }
            else if(list.size() == 7) // including exponents
            {
                if(list.at(4).startsWith("EXP"))
                {
                    bool ok1, ok2, ok3, ok4, ok5 = true;
                    l_cost_constant = list.at(1).toDouble(&ok1);
                    l_cost_fraction = list.at(2).toDouble(&ok2);
                    l_cost_capacity = list.at(3).toDouble(&ok3);
                    l_cost_frac_exp = list.at(5).toDouble(&ok4);
                    l_cost_cap_exp = list.at(6).toDouble(&ok5);

                    if(!ok1 || !ok2 || !ok3 || !ok4 || !ok5) ok = false;
                }
                else ok = false;
            }
            else
            {
                QString message = QString("Error detected in input file.\nCOST for SEPARATOR not in correct format.\nThe COST keyword must be followed by three numbers: constant term, fraction multiplier, capacity multiplier.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        else if(list.at(0).startsWith("INSTALLTIME"))                                           // getting the installation time
        {
            shared_ptr<IntVariable> var_install = shared_ptr<IntVariable>(new IntVariable(p_sep));

            if(list.size() == 2) // not a variable, only starting value specified
            {
                int value = list.at(1).toInt(&ok);
                var_install->setValue(value);
                var_install->setMax(value);
                var_install->setMin(value);
            }
            else if(list.size() == 4)   // value, max, and min specified
            {
                int value, max, min = 0;
                bool ok1, ok2, ok3 = true;

                value = list.at(1).toInt(&ok1);
                max = list.at(2).toInt(&ok2);
                min = list.at(3).toInt(&ok3);

                var_install->setValue(value);
                var_install->setMax(max);
                var_install->setMin(min);

                if(!ok1 || !ok2 || !ok3) ok = false;
            }
            else
            {
                cout << endl << "### Error detected in input file! ###" << endl
                     << "INSTALLTIME for SEPARATOR not in correct format..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;
                exit(1);
            }
            p_sep->setInstallTime(var_install);
        }
        else if(list.at(0).startsWith("REMOVE"))                                           // getting the remove fraction
        {
            shared_ptr<RealVariable> var_fraction = shared_ptr<RealVariable>(new RealVariable(p_sep));
            if(list.size() == 2) // not a variable, only starting value specified
            {
                double value = list.at(1).toDouble(&ok);
                var_fraction->setValue(value);
                var_fraction->setMax(value);
                var_fraction->setMin(value);
            }
            else if(list.size() == 4)   // value, max, and min specified
            {
                double value, max, min = 0;
                bool ok1, ok2, ok3 = true;

                value = list.at(1).toDouble(&ok1);
                max = list.at(2).toDouble(&ok2);
                min = list.at(3).toDouble(&ok3);

                var_fraction->setValue(value);
                var_fraction->setMax(max);
                var_fraction->setMin(min);
                if(!ok1 || !ok2 || !ok3) ok = false;
            }
            else
            {
                QString message = QString("Error detected in input file.\nREMOVE keyword for SEPARATOR not in correct format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
            p_sep->setRemoveFraction(var_fraction);
        } // REMOVE kwrd
        else if(list.at(0).startsWith("CAPACITY"))                                           // getting the water removal capacity
        {
            shared_ptr<RealVariable> var_capacity = shared_ptr<RealVariable>(new RealVariable(p_sep));
            if(list.size() == 2) // not a variable, only starting value specified
            {
                double value = list.at(1).toDouble(&ok);
                var_capacity->setValue(value);
                var_capacity->setMax(value);
                var_capacity->setMin(value);

            }
            else if(list.size() == 4)   // value, max, and min specified
            {
                double value, max, min = 0;
                bool ok1, ok2, ok3 = true;

                value = list.at(1).toDouble(&ok1);
                max = list.at(2).toDouble(&ok2);
                min = list.at(3).toDouble(&ok3);

                var_capacity->setValue(value);
                var_capacity->setMax(max);
                var_capacity->setMin(min);
                if(!ok1 || !ok2 || !ok3) ok = false;
            }
            else
            {
                QString message = QString("Error detected in input file.\nCAPACITY keyword for SEPARATOR not in correct format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
                cout << endl << "### Error detected in input file! ###" << endl
                     << "CAPACITY keyword for SEPARATOR not in correct format..." << endl
                     << "Last line: " << list.join(" ").toLatin1().constData() << endl << endl;
                exit(1);
            }
            p_sep->setRemoveCapacity(var_capacity);
        } // CAPACITY kwrd
        else
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("Error detected in input file.\nKeyword: %1\nNot recognized in current context.\n").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        if(!ok) break;
        list = StringUtilities::processLine(m_driver_file.readLine());
    }

    if(!ok)                                             // error with number reading
    {
        QString message = QString("Error detected in input file.\nPIPE definition is incomplete.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
    }

    // everything is ok, setting common parameters
    p_sep->setNumber(l_number);

    // the cost
    Cost *c = new Cost();
    c->setConstant(l_cost_constant);
    c->setFractionMultiplier(l_cost_fraction);
    c->setCapacityMultiplier(l_cost_capacity);
    c->setFraction(p_sep->removeFraction()->value());
    c->setCapacity(p_sep->removeCapacity()->value());
    c->setCapacityExponent(l_cost_cap_exp);
    c->setFractionExponent(l_cost_frac_exp);
    p_sep->setCost(c);

    // the outlet connection
    PipeConnection *p_con = new PipeConnection();
    p_con->setPipeNumber(l_outlet_pipe);

    shared_ptr<BinaryVariable> routing_var = shared_ptr<BinaryVariable>(new BinaryVariable(p_sep));
    routing_var->setValue(1.0);
    routing_var->setIsVariable(false);
    routing_var->setName("Dummy variable for separator outlet connection");
    p_con->setVariable(routing_var);
    p_sep->setOutletConnection(p_con);

    // setting the names of the removal variables
    p_sep->installTime()->setName("Installation time variable for separator #" + QString::number(l_number));
    p_sep->removeFraction()->setName("Water removal fraction for separator #" + QString::number(l_number));
    p_sep->removeCapacity()->setName("Water removal capacity for separator #" + QString::number(l_number));
    return p_sep;
}


Pipe* DriverReader::readPressureBooster()
{
    printProgress("Reading booster definition.");
    PressureBooster *p_boost = new PressureBooster();
    QStringList list;

    bool ok = true;
    int l_number = -1;
    int l_outlet_pipe = -1;
    double l_cost_constant = -1.0;
    double l_cost_fraction = -1.0;
    double l_cost_cap_exp = 1.0;
    double l_cost_frac_exp = 1.0;

    list = StringUtilities::processLine(m_driver_file.readLine());
    while(!m_driver_file.atEnd() && !(list.at(0).startsWith("END") && list.at(1).startsWith("BOOSTER")))
    {
        if(list.at(0).startsWith("NUMBER")) l_number = list.at(1).toInt(&ok);                   // getting the id number of the booster
        else if(list.at(0).startsWith("OUTLETPIPE")) l_outlet_pipe = list.at(1).toInt(&ok);     // getting the outlet pipe number
        else if(list.at(0).startsWith("COST"))                                                  // getting the cost
        {
            if(list.size() == 3)  // the right number of numbers
            {
                bool ok1, ok2 = true;
                l_cost_constant = list.at(1).toDouble(&ok1);
                l_cost_fraction = list.at(2).toDouble(&ok2);
                if(!ok1 || !ok2) ok = false;
            }
            else if(list.size() == 6) // including exponents
            {
                if(list.at(3).startsWith("EXP"))
                {
                    bool ok1, ok2, ok3, ok4 = true;
                    l_cost_constant = list.at(1).toDouble(&ok1);
                    l_cost_fraction = list.at(2).toDouble(&ok2);
                    l_cost_frac_exp = list.at(4).toDouble(&ok3);
                    l_cost_cap_exp = list.at(5).toDouble(&ok4);
                    if(!ok1 || !ok2 || !ok3 || !ok4) ok = false;
                }
                else ok = false;
            }
            else
            {
                QString message = QString("Error detected in input file.\nCOST for BOOSTER not in correct format.\nThe COST keyword must be followed by two numbers: constant term, multiplier.\nor it can include exponents: cost term, multiplier, EXP, pressure exponent, capacity exponent\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        else if(list.at(0).startsWith("INSTALLTIME"))                                           // getting the installation time
        {
            shared_ptr<IntVariable> var_install = shared_ptr<IntVariable>(new IntVariable(p_boost));
            if(list.size() == 2) // not a variable, only starting value specified
            {
                int value = list.at(1).toInt(&ok);
                var_install->setValue(value);
                var_install->setMax(value);
                var_install->setMin(value);
            }
            else if(list.size() == 4)   // value, max, and min specified
            {
                int value, max, min = 0;
                bool ok1, ok2, ok3 = true;

                value = list.at(1).toInt(&ok1);
                max = list.at(2).toInt(&ok2);
                min = list.at(3).toInt(&ok3);

                var_install->setValue(value);
                var_install->setMax(max);
                var_install->setMin(min);
                if(!ok1 || !ok2 || !ok3) ok = false;
            }
            else
            {
                QString message = QString("Error detected in input file.\nINSTALLTIME for BOOSTER not in correct format\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
            p_boost->setInstallTime(var_install);
        }
        else if(list.at(0).startsWith("PRESSUREBOOST"))                                           // getting the boost pressure
        {
            shared_ptr<RealVariable> var_pressure = shared_ptr<RealVariable>(new RealVariable(p_boost));

            if(list.size() == 2) // not a variable, only starting value specified
            {
                double value = list.at(1).toDouble(&ok);
                var_pressure->setValue(value);
                var_pressure->setMax(value);
                var_pressure->setMin(value);
            }
            else if(list.size() == 4)   // value, max, and min specified
            {
                double value, max, min = 0;
                bool ok1, ok2, ok3 = true;

                value = list.at(1).toDouble(&ok1);
                max = list.at(2).toDouble(&ok2);
                min = list.at(3).toDouble(&ok3);

                var_pressure->setValue(value);
                var_pressure->setMax(max);
                var_pressure->setMin(min);
                if(!ok1 || !ok2 || !ok3) ok = false;
            }
            else
            {
                QString message = QString("Error detected in input file.\nPRESSUREBOOST keyword for BOOSTER not in correct format.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
            p_boost->setPressureVariable(var_pressure);
        } // PRESSUREBOOST kwrd

        else if(list.at(0).startsWith("CAPACITY"))                                           // getting the capacity
        {
            shared_ptr<RealVariable> var_capacity = shared_ptr<RealVariable>(new RealVariable(p_boost));
            if(list.size() == 2) // not a variable, only starting value specified
            {
                double value = list.at(1).toDouble(&ok);
                var_capacity->setValue(value);
                var_capacity->setMax(value);
                var_capacity->setMin(value);
            }
            else if(list.size() == 4)   // value, max, and min specified
            {
                double value, max, min = 0;
                bool ok1, ok2, ok3 = true;
                value = list.at(1).toDouble(&ok1);
                max = list.at(2).toDouble(&ok2);
                min = list.at(3).toDouble(&ok3);
                var_capacity->setValue(value);
                var_capacity->setMax(max);
                var_capacity->setMin(min);
                if(!ok1 || !ok2 || !ok3) ok = false;
            }
            else
            {
                QString message = QString("Error detected in input file.\nCAPACITY keyword for BOOSTER not in correct format\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
            p_boost->setCapacityVariable(var_capacity);
        } // CAPACITY kwrd
        else
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("Error detected in input file.\nKeyword: %1\nNot understood in current context.").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        if(!ok) break;
        list = StringUtilities::processLine(m_driver_file.readLine());
    }
    if(!ok)                                             // error with number reading
    {
        QString message = QString("Error detected in input file.\nBOOSTER definition is incomplete.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
    }
    // everything is ok, setting common parameters
    p_boost->setNumber(l_number);

    // the cost
    Cost *c = new Cost();
    c->setConstant(l_cost_constant);
    c->setFractionMultiplier(l_cost_fraction);
    c->setCapacityMultiplier(0.0);
    c->setFractionExponent(l_cost_frac_exp);
    c->setCapacityExponent(l_cost_cap_exp);
    c->setFraction(p_boost->pressureVariable()->value());
    c->setCapacity(p_boost->capacityVariable()->value());
    c->setLinear(false);
    p_boost->setCost(c);

    // the outlet connection
    PipeConnection *p_con = new PipeConnection();
    p_con->setPipeNumber(l_outlet_pipe);

    shared_ptr<BinaryVariable> routing_var = shared_ptr<BinaryVariable>(new BinaryVariable(p_boost));
    routing_var->setValue(1.0);
    routing_var->setIsVariable(false);
    routing_var->setName("Dummy variable for booster outlet connection");
    p_con->setVariable(routing_var);
    p_boost->setOutletConnection(p_con);

    // setting the names of the removal variables
    p_boost->installTime()->setName("Installation time variable for booster #" + QString::number(l_number));
    p_boost->pressureVariable()->setName("Pressure variable for booster #" + QString::number(l_number));
    p_boost->capacityVariable()->setName("Capacity variable for booster #" + QString::number(l_number));
    return p_boost;
}


void DriverReader::readOptimizer(Model *m)
{
    printProgress("Reading optimizer definition.");

    QStringList list;
    double l_perturb = 0.0001;
    int l_max_iter = 1;
    int l_max_iter_cont = 100;
    int l_parallel_runs = 1;
    double l_term = 0.0;
    int l_term_start = 5;
    bool l_startingpoint_update = false;
    QList<int> l_eropt_steps;
    bool ok = true;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {
        if(list.at(0).startsWith("TYPE"))                           // getting the type
        {
            if(list.at(1).startsWith("RUNONCE"))
                m->getRuntimeSettings()->getOptimizerSettings()->setOptimizer(RUNONCE);
            else if(list.at(1).startsWith("LSH"))
                m->getRuntimeSettings()->getOptimizerSettings()->setOptimizer(LSH);
            else if(list.at(1).startsWith("EROPT"))
                m->getRuntimeSettings()->getOptimizerSettings()->setOptimizer(EROPT);
        }
        else if(list.at(0).startsWith("ITERATIONS")) l_max_iter = list.at(1).toInt(&ok);    // getting the max number if iterations
        else if(list.at(0).startsWith("CONT_ITER")) l_max_iter_cont = list.at(1).toInt(&ok); // getting the max number if iterations for the contienous solver
        else if(list.at(0).startsWith("PERTURB")) l_perturb = list.at(1).toDouble(&ok);     // getting the perturbation size
        else if(list.at(0).startsWith("STARTINGPOINT_UPDATE")) l_startingpoint_update = true;     // using the starting-point from the best sub-problem
        else if(list.at(0).startsWith("TERMINATION"))                                       // getting the termination options
        {
            l_term = list.at(1).toDouble(&ok);
            if(list.size() == 3)
                l_term_start = list.at(2).toInt(&ok);
        }
        else if(list.at(0).startsWith("STEPS"))                 // list of steps for EROPT only
        {
            bool ok_steps = true;

            for(int i = 1; i < list.size(); ++i)
            {
                int stp = list.at(i).toInt(&ok_steps);
                if(!ok_steps)
                {
                    QString message = QString("Error detected in input file.\nCould not read STEPS in OPTIMIZER definition.\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                    emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
                }
                l_eropt_steps.push_back(stp);
            }
        }

        else if(list.at(0).startsWith("PARALLEL"))                                           // getting the number of parallel runs
        {
            if(list.at(1).startsWith("IDEAL"))              // find the ideal number of parallel runs
            {
                l_parallel_runs = 2;
                emitException(ExceptionSeverity::WARNING, ExceptionType::ASSUMPTION, "Using IDEAL number of parallel runs.");
            }
            else
            {
                l_parallel_runs = list.at(1).toInt(&ok);   // user specified number
                emitException(ExceptionSeverity::WARNING, ExceptionType::ASSUMPTION, "Using user-specified number of parallel runs.");
            }
            // checking that the number of parallel runs is at least 1
            if(l_parallel_runs < 1) l_parallel_runs = 1;
        }
        else
        {
            if(!StringUtilities::isEmpty(list))
            {
                QString message = QString("Error detected in input file.\nKeyword: %1 \nNot understood in current context.").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        list = StringUtilities::processLine(m_driver_file.readLine());
    }
    if(!ok)
    {
        QString message = QString("Error detected in input file.\nThe optimizer settings were not defined properly.");
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
    }

    // everything ok, setting to optimizer
    m->getRuntimeSettings()->getOptimizerSettings()->setMaxIterations(l_max_iter);
    m->getRuntimeSettings()->getOptimizerSettings()->setMaxIterations(l_max_iter_cont);
    m->getRuntimeSettings()->setParallelRuns(l_parallel_runs);
    m->getRuntimeSettings()->getOptimizerSettings()->setPerturbationSize(l_perturb);
    m->getRuntimeSettings()->getOptimizerSettings()->setStartingpointUpdate(l_startingpoint_update);
    m->getRuntimeSettings()->getOptimizerSettings()->setTermination(l_term);
    m->getRuntimeSettings()->getOptimizerSettings()->setTerminationStart(l_term_start);

    // EROPT specific input
    if(l_eropt_steps.size() > 0)
    {
        m->getRuntimeSettings()->getOptimizerSettings()->setSteps(l_eropt_steps);
    }
}


void DriverReader::readUserDefinedConstraints(Model *m)
{
    printProgress("Reading user defined constraints.");

    QStringList list;
    list = StringUtilities::processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {
        if(!StringUtilities::isEmpty(list))
        {
            UserConstraint *uc = new UserConstraint(m);
            uc->setExpression(list.join(" "));
            m->addUserDefinedConstraint(uc);
        }
        list = StringUtilities::processLine(m_driver_file.readLine());
    }
}
