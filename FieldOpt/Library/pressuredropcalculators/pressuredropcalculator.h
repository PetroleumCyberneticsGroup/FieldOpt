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

#ifndef PRESSUREDROPCALCULATOR_H
#define PRESSUREDROPCALCULATOR_H

#include "model/stream.h"

/*!
 * \brief Abstract base class for pressure drop calculators.
 */
class PressureDropCalculator
{
public:
    PressureDropCalculator() {}
    virtual ~PressureDropCalculator() {}
    virtual PressureDropCalculator* clone() const = 0;  //!< Get a copy of the calculator.

    /*!
     * \brief Calculate the pressure drop in the pipe for a given Stream and outlet pressure.
     * \param s The rates going through the pipe segment.
     * \param p_outlet The outlet pressure of the pipe.
     * \param unit The units to be used in calculations.
     * \return The caluclated pressure drop through the pipe.
     */
    virtual double pressureDrop(Stream* s, double p_outlet, Stream::units unit) = 0;
};

#endif // PRESSUREDROPCALCULATOR_H
