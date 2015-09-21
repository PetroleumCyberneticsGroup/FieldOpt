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

#ifndef COST_H
#define COST_H

/*!
 * \brief Class for costs associated with installing equipment etc.
 *
 * A Cost has a price (value in USD), and an installation time.
 * A vector of costs can be supplied to the Objective the costs are only used for the NpvOpjective when the net present value is calculated.
 * Currently only separators have associated costs. \todo Expand to more than separators.
 */
class Cost
{
private:
    double m_time;               //!< Holds the current time.
    double m_const;              //!< \todo Document this
    double m_mult_frac;          //!< Holds the fraction multiplier.
    double m_mult_cap;           //!< Holds the capacity multiplier.
    double m_fraction;           //!< Holds the fraction value.
    double m_capacity;           //!< Holds the capacity value.
    double m_fraction_exponent;  //!< Holds the fraction exponent.
    double m_capacity_exponent;  //!< Holds the capacity exponent.
    bool m_linear;               //!< Is the cost linear or non-linear?

public:
    Cost();

    void setTime(double t) {m_time = t;}                           //!< Set function for time.
    void setConstant(double v) {m_const = v;}                      //!< Set function for m_const
    void setFractionMultiplier(double v) {m_mult_frac = v;}        //!< Set function for m_mult_frac
    void setCapacityMultiplier(double v) {m_mult_cap = v;}         //!< Set function for m_mult_cap
    void setFractionExponent(double e) {m_fraction_exponent = e;}  //!< Set function for m_fraction_exponent
    void setCapacityExponent(double e) {m_capacity_exponent = e;}  //!< Set function for m_capacity_exponent
    void setFraction(double f) {m_fraction = f;}                   //!< Set function for m_fraction
    void setCapacity(double c) {m_capacity = c;}                   //!< Set function for m_capacity
    void setLinear(bool b) {m_linear = b;}                         //!< Set function for m_linear

    double time() const {return m_time;}               //!< Returns the current time.
    double value() const;                              //!< Returns the current value of the cost.
    double constantCost() const {return m_const;}      //!< Returns the constant cost.
    double fractionCost() const {return m_mult_frac;}  //!< Returns the fraction multiplier.
    double capacityCost() const {return m_mult_cap;}   //!< Retruns the capacity multiplier.

    bool operator<(const Cost &rhs) const;  //!< Overloaded < operator. Checks m_time.
    bool operator>(const Cost &rhs) const;  //!< Overloaded > operator. Checks m_time.
};

#endif // COST_H
