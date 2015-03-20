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

#include "modelreader.h"

ModelReader::ModelReader(const QString &driver)
    : m_driver_file_name(driver)
{}


Model* ModelReader::readDriverFile(Runner *r)
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
    list = processLine(m_driver_file.readLine());

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
            if(!isEmpty(list))
            {
                QString message = QString("The first keyword must specify the model type.\n Possible types are COUPLED, DECOUPLEDM ADJOINTS_COUPLED.\n Last line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        list = processLine(m_driver_file.readLine());
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
            else if(list.at(1).startsWith("OPTIMIZER")) readOptimizer(r);                                       // optimizer
            else if(list.at(1).startsWith("MASTERSCHEDULE")) p_model->setMasterSchedule(readMasterSchedule());  // master schedule
            else if(list.at(1).startsWith("SEPARATOR")) p_model->addPipe(readSeparator());                      // separator
            else if(list.at(1).startsWith("BOOSTER")) p_model->addPipe(readPressureBooster());                  // booster
            else if(list.at(1).startsWith("CONSTRAINTS")) readUserDefinedConstraints(p_model);                  // user defined constraints
        }
        else if(list.at(0).startsWith("DEBUG"))
        {
            r->setDebugFileName(m_path + "/" + list.at(1));                                    // setting the debug file
        }
        else if(list.at(0).startsWith("SIMULATOR"))     // reading the type of reservoir simulator to use
        {
            if(list.at(1).startsWith("GPRS")) r->setReservoirSimulator(new GprsSimulator());
            else if(list.at(1).startsWith("VLP")) r->setReservoirSimulator(new VlpSimulator());
            else if(list.at(1).startsWith("MRST_BATCH")) r->setReservoirSimulator(new MrstBatchSimulator());
            else
            {
                QString message = QString("Type of SIMULATOR not understood.\nPossible types are: GPRS, MRST_BATCH, VLP\nLast line: %1").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }
        else
        {
            if(!isEmpty(list))
            {
                QString message = QString("Keyword: %1\nNot understood in current context.").arg(list.join(" ").toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, message);
            }
        }

        if(!ok) break;
        list = processLine(m_driver_file.readLine());
    }
    if(ok)
        printProgress(QString("Done reading input. All OK."));
    else
        emitException(ExceptionSeverity::ERROR, ExceptionType::UNABLE_TO_PARSE, QString("Malformed input."));
    // setting the driver file path to the model
    p_model->setDriverPath(m_path);
    return p_model;
}


QVector<double> ModelReader::readMasterSchedule()
{
    printProgress(QString("Reading master schedule."));
    QStringList list;
    QVector<double> l_schedule;
    bool ok = true;
    list = processLine(m_driver_file.readLine());

    while(!m_driver_file.atEnd() && !list.at(0).startsWith("END"))
    {
        if(!isEmpty(list))
            l_schedule.push_back(list.at(0).toDouble(&ok));
        if(!ok) break;
        list = processLine(m_driver_file.readLine());
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
            emitException(ExceptionSeverity::ERROR, ExceptionType::INCONSISTENT);
        }
    }
    return l_schedule;
}
