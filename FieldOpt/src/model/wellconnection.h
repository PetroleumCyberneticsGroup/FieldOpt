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

#ifndef WELLCONNECTION_H
#define WELLCONNECTION_H

/*!
 * \brief Class representing a well connection.
 */
class WellConnection
{
private:
    int m_cell; //!< Cell number in global indexing scheme. \todo This has a todo tag in the ResOpt code.
    int m_i;    //!< i-coordinate of the perforations.
    int m_j;    //!< j-coordinate of the perforations.
    int m_k1;   //!< k-coordinate of the upper perforation range.
    int m_k2;   //!< k-coordinate of the lower perforation range.
    double m_wi;   //!< Well Index (WI) for the connection(s). \todo This has a todo tag in the ResOpt code.

public:
    WellConnection(); //!< Default constructor. Initializes all properties to -1.

    void setCell(int i) { m_cell = i; }          //!< Set cell number.
    void setI(int i) { m_i = i; }                //!< Set i-coordinate.
    void setJ(int j) { m_j = j; }                //!< Set j-coordinate.
    void setK1(int k1) { m_k1 = k1; }            //!< Set k1-coordinate.
    void setK2(int k2) { m_k2 = k2; }            //!< Set k2-coordinate.
    void setWellIndex(double wi) { m_wi = wi; }  //!< Set connection well index.

    int cell() const {return m_cell;}        //!< Get cell-number.
    int i() const {return m_i;}              //!< Get i-coordinate.
    int j() const {return m_j;}              //!< Get j-coordinate.
    int k1() const {return m_k1;}            //!< Get k1-coordinate.
    int k2() const {return m_k2;}            //!< Get k2-coordinate.
    double wellIndex() const {return m_wi;}  //!< Get connection well index.
};

#endif // WELLCONNECTION_H
