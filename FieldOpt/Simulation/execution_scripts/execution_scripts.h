/******************************************************************************
 *
 *
 *
 * Created: 24.11.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
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
 *****************************************************************************/

#ifndef EXECUTION_SCRIPTS
#define EXECUTION_SCRIPTS

#include <QString>
#include <QMap>
#include "Utilities/filehandling.hpp"

namespace Simulation { namespace ExecutionScripts {

    /*!
     * \brief The DefaultScripts enum lists the available scripts.
     */
    enum Script {
        csh_eclrun,
        bash_ecl,
        bash_adgprs,
        bash_flow
    };

    static QMap<Script, QString> DefaultScripts {
        {Script::csh_eclrun, QString("execution_scripts/csh_eclrun.sh")},
        {Script::bash_ecl, QString("execution_scripts/bash_ecl.sh")},
        {Script::bash_adgprs, QString("execution_scripts/bash_adgprs.sh")},
        {Script::bash_flow, QString("execution_scripts/bash_flow.sh")}
    };

    static Script GetScript(QString name) {
        if (QString::compare(name, "csh_eclrun") == 0)
            return Script::csh_eclrun;
        if (QString::compare(name, "bash_ecl") == 0)
            return Script::bash_ecl;
        else if (QString::compare(name, "bash_adgprs") == 0)
            return Script::bash_adgprs;
        else if (QString::compare(name, "bash_flow") == 0)
            return Script::bash_flow;
        else throw std::runtime_error("Script " + name.toStdString() + " not recognized.");
    }

    static QString GetScriptPath(QString name) {
        Script scr = GetScript(name);
        QString path = DefaultScripts[scr];
        return path;
    }
}}

#endif // EXECUTION_SCRIPTS
