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

#ifndef PIPEREADER_H
#define PIPEREADER_H

#include <QStringList>
#include <QFile>
#include "Library/pressuredropcalculators/pressuredropcalculator.h"
#include "Library/pressuredropcalculators/beggsbrillcalculator.h"
#include "Library/utilities/stringutilities.h"
#include "reader.h"

class PressureDropCalculator;
class BeggsBrillCalculator;
class StringUtilities;


/*!
 * \brief The PipeReader class reads a pipe-description file and returns a PressureCalculator object.
 *
 * \todo Support DpTableCalculator
 */
class PipeReader : Reader
{
private:
    BeggsBrillCalculator* readBeggsBrill(QFile &file); //!< Read the input file and generate a BeggsBrillCalculator.

public:
    PipeReader(){}  //!< Default constructor. Does nothing. \todo Take file name as parameter in constructor and remove it from readFile()

    /*!
     * \brief Reads a pipe-description file and returns a PressureDropCalculator.
     *
     * Searches the file for a keyword determining the type of pressure drop calculator to be used, then
     * calls the appropriate function to generate it. If the keyword is not found, an error is emmited.
     *
     * \todo Support DpTableCalculator. Currently prints error and exits if anything other than BB73 is defined as calculator.
     * \param file_name The name of the file to be read.
     * \return
     */
    PressureDropCalculator* readFile(const QString &file_name);
};

#endif // PIPEREADER_H
