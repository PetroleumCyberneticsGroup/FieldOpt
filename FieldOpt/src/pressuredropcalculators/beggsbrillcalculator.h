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

#ifndef BEGGSBRILLCALCULATOR_H
#define BEGGSBRILLCALCULATOR_H

#include <pressuredropcalculators/pressuredropcalculator.h>

/*!
 * \brief Pressure drop calculations afrer Beggs and Brill, 1973.
 *
 * Brief descriptions of the equations used follow.
 *
 * __Superficial gas velocity (vsg)__
 *
 * The superficialGasVelocity function uses the expression
 * \f[
 *     v_g = \frac{q_g}{\pi \times r^2}
 * ]\f
 * Where \f$( q_g )$\f is the gas rate at pipe conditions, calculated from
 * \f[
 *     q_g = \frac{q_{g,surface}}{B_g}
 * ]\f
 *
 */
class BeggsBrillCalculator : public PressureDropCalculator
{
public:
    enum flow_regime {SEGREGATED, TRANSITION, INTERMITTENT, DISTRIBUTED, UNDEFINED};

private:
    double m_sg_gas;       //!< Gas specific gravity
    double m_den_oil;      //!< Oil density
    double m_den_wat;      //!< Water density

    double m_vis_oil;      //!< Oil viscosity
    double m_vis_wat;      //!< Water viscosity

    double m_diameter;     //!< Inner pipe diameter
    double m_length;       //!< Total pipe length
    double m_angle;        //!< Inclination of pipe
    double m_temperature;  //!< Average temperature in pipe

    double superficialGasVelocity(Stream *s, double p, double z);      //!< Calculate the superficial gas velocity (vsg).
    double superficialLiquidVelocity(Stream *s);                       //!< Calculate the superficial liquid velocity (vsl).
    double liquidDensity(Stream *s);                                   //!< Calculate the liquid density for oil and water.
    double gasZFactor(double yg, double t, double p);                  //!< Calculate the gas z-factor.
    double gasDensity(double t, double p, double z);                   //!< Calculate the gas density at pipe conditions.
    double surfaceTension(double gas_density, double liquid_density);  //!< Calculate the gas-liquid surface tension.
    double gasViscosity(double p, double z);                           //!< Calculate the gas viscosity.
    double liquidViscosity(Stream *s);                                 //!< Calculate the liquid viscosity.

public:
    BeggsBrillCalculator();
    virtual ~BeggsBrillCalculator();
};

#endif // BEGGSBRILLCALCULATOR_H
