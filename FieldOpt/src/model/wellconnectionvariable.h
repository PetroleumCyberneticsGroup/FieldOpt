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

#ifndef WELLCONNECTIONVARIABLE_H
#define WELLCONNECTIONVARIABLE_H

#include <tr1/memory>
#include "model/wellconnection.h"
#include "variables/intvariable.h"

using std::tr1::shared_ptr;
class IntVariable;

class WellConnectionVariable
{
private:
    WellConnection *p_well_con;       //!< Well connection holding coordinates.
    shared_ptr<IntVariable> p_var_i;  //!< Variable holding the i-coordinate.
    shared_ptr<IntVariable> p_var_j;  //!< Variable holding the j-coordinate.


public:
    WellConnectionVariable();                                   //!< Default constructor. Instantiates p_well_con.
    WellConnectionVariable(const WellConnectionVariable &wcv);  //!< Copy constructor.
    ~WellConnectionVariable();                                  //!< Deletes p_well_con.

    void setIVariable(shared_ptr<IntVariable> i) {p_var_i = i;}   //!< Set i-variable.
    void setJVariable(shared_ptr<IntVariable> j) {p_var_j = j;}   //!< Set j-variable.
    void setK1(int k1) {p_well_con->setK1(k1);}                   //!< Set k1-coordinate.
    void setK2(int k2) {p_well_con->setK2(k2);}                   //!< Set k2-coordinate.
    void setWellIndex(double wi) {p_well_con->setWellIndex(wi);}  //!< Set connection well index.

    WellConnection* wellConnection();                      //!< Update well connection i,j coordinates according to the variables and return it.
    shared_ptr<IntVariable> iVariable() {return p_var_i;}  //!< Get i-variable.
    shared_ptr<IntVariable> jVariable() {return p_var_j;}  //!< Get j-variable.
};

#endif // WELLCONNECTIONVARIABLE_H
