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
 * \brief Pressure drop calculations afrer Beggs and Brill, 1973. (SPE-4007-PA)
 *
 * Brief descriptions of the equations used follow.
 *
 * __Superficial Gas Velocity (vsg)__
 *
 * The superficialGasVelocity function uses the expression
 * \f[
 *     v_g = \frac{q_g}{\pi r^2}
 * \f]
 * Where \f$ q_g \f$ is the gas rate at pipe conditions, calculated from
 * \f[
 *     q_g = \frac{q_{g,surface}}{\frac{1}{B_g}}
 * \f]
 * and
 * \f[
 *     \frac{1}{B_g} = \frac{PT_{sc}}{P_{sc}Tz}
 * \f]
 *
 * __Superficial Liquid Velocity (vsl)__
 *
 * The superficialLiquidVelocity function uses the expression
 * \f[
 *     v_l = \frac{q_l}{\pi r^2}
 * \f]
 * where
 * \f[
 *     q_l = q_o + q_w
 * \f]
 *
 * __Liquid Density__
 *
 * The liquidDensity function uses the expression
 * \f[
 *     \rho_l = \frac{\rho_0 q_o + \rho_w q_w}{q_o+q_w}
 * \f]
 *
 * __Gas z-factor__
 * \todo Document gasZFactor
 * Calculatied using the Hall and Yarborough (HY) method with residuals presented in SPE-172373-MS
 *
 * __Gas Density__
 * \f[
 *     \rho_g = \frac{pM_g}{TzR}
 * \f]
 *
 * __Surface Tension__
 * Calculated using the graphical correlation of Ramey (SPE-4429-MS), ref. Whitson (1990-11-20, Minimum Lift Calculations for Gas Wells)
 * According to Whitson, rhe following relation may be used to estimate the gas-liquid surface tension:
 *
 * \f[
 *     \sigma_{wg} = 15 + 0.91 \Delta \rho_{wg}
 * \f]
 *
 * or
 *
 * \f[
 *     (\sigma_{wg} \Delta \rho_{wg})^{0.25} = [(15+0.91 \Delta \rho_{wg}) \Delta \rho{wg}]^{0.25}.
 * \f]
 *
 * The first relation is used in the code.
 *
 * The units for \f$ \rho \f$ are \f$ lb\ft^3 \f$ and the units for \f$ \sigma \f$ are \f$ dynes/cm \f$.
 *
 *
 * __Gas Viscosity__
 * \todo Document gasViscosity
 *
 * __Liquid Viscosity__
 *
 * The liquid viscosity is calculated as
 * \f[
 *     \mu_l = \frac{q_o\mu_o + q_w\mu_w}{q_o + q_w}
 * \f]
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
    double gasZFactor(double yg, double t, double p);                  //!< Calculate the gas compressibility (z) factor.
    double gasDensity(double t, double p, double z);                   //!< Calculate the gas density at pipe conditions.
    double surfaceTension(double gas_density, double liquid_density);  //!< Calculate the gas-liquid surface tension.
    double gasViscosity(double p, double z);                           //!< Calculate the gas viscosity.
    double liquidViscosity(Stream *s);                                 //!< Calculate the liquid viscosity.

public:
    BeggsBrillCalculator();
    virtual ~BeggsBrillCalculator(){};

    virtual PressureDropCalculator* clone() const {return new BeggsBrillCalculator(*this);} //!< Return a copy of this calculator.

    /*!
     * \brief Calculate the pressure drop in the pipe for a given stream and outlet pressure.
     * \param s The Stream (rates) going through the Pipe segment.
     * \param p The outlet pressure of the pipe.
     * \param unit The units to b e used in the calculations.
     * \return The calculated pressure drop in the pipe.
     */
    virtual double pressureDrop(Stream *s, double p_outlet, Stream::units unit);

    void setDiameter(double d) {m_diameter = d;}          //!< Set function for m_diameter
    void setLength(double l) {m_length = l;}              //!< Set function for m_length
    void setAngle(double a) {m_angle = a;}                //!< Set function for m_angle
    void setTemperature(double t) {m_temperature = t;}    //!< Set function for m_temperature
    void setGasSpecificGravity(double s) {m_sg_gas = s;}  //!< Set function for m_sg_gas
    void setOilDensity(double d) {m_den_oil = d;}         //!< Set function for m_den_oil
    void setWaterDensity(double d) {m_den_wat = d;}       //!< Set function for m_den_wat
    void setOilViscosity(double v) {m_vis_oil = v;}       //!< Set function for m_vis_oil
    void setWaterViscosity(double v) {m_vis_wat = v;}     //!< Set function for m_vis_wat

    // get functions

    double diameter() {return m_diameter;}          //!< Get function for m_diameter
    double length() {return m_length;}              //!< Get function for m_length
    double temperature() {return m_temperature;}    //!< Get function for m_temperature
    double gasSpecificGravity() {return m_sg_gas;}  //!< Get function for m_sg_gas
    double oilDensity() {return m_den_oil;}         //!< Get function for m_den_oil
    double waterDensity() {return m_den_wat;}       //!< Get function for m_den_wat
    double oilViscosity() {return m_vis_oil;}       //!< Get function for m_vis_oil
    double waterViscosity() {return m_vis_wat;}     //!< Get function for m_vis_wat

    /*!
     * \brief Get the angle of the pipe in either radians or degrees.
     * \param rad True if the angle is to be returned in radians.
     * \return The angle of the pipe in radians or degrees.
     */
    double angle(bool rad = false);
};

#endif // BEGGSBRILLCALCULATOR_H
