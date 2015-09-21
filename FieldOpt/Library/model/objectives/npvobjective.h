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

#ifndef NPVOBJECTIVE_H
#define NPVOBJECTIVE_H

#include "objective.h"

class NpvObjective : public Objective
{
private:
    double m_dcf;           //!< Discount factor \todo This is marked as todo in the ResOpt code.
    double m_price_oil;     //!< Oil price       \todo This is marked as todo in the ResOpt code.
    double m_price_gas;     //!< Gas price       \todo This is marked as todo in the ResOpt code.
    double m_price_water;   //!< Water price     \todo This is marked as todo in the ResOpt code.

public:
    NpvObjective();  //!< Default constructor. Initializes all values to 0.0.
    NpvObjective(const NpvObjective &o);  //!< Copy constructor.

    virtual Objective* clone() {return new NpvObjective(*this);}        //!< Get a copy of this objective.
    virtual QString description() const;                                //!< Get a description for the driver file.
    virtual void calculateValue(QVector<Stream*> s, QVector<Cost*> c);  //!< Get the NPV based on the input streams, the discount factor, and the product prices

    void setDcf(double d) {m_dcf = d;}                 //!< Set the discount factor.
    void setOilPrice(double p) {m_price_oil = p;}      //!< Set the oil price.
    void setGasPrice(double p) {m_price_gas = p;}      //!< Set the gas price.
    void setWaterPrice(double p) {m_price_water = p;}  //!< Set the water price.

    double dcf() const {return m_dcf;}                 //!< Get the discount factor
    double oilPrice() const {return m_price_oil;}      //!< Get the oil price
    double gasPrice() const {return m_price_gas;}      //!< Get the gas price
    double waterPrice() const {return m_price_water;}  //!< Get the water price
};

#endif // NPVOBJECTIVE_H
