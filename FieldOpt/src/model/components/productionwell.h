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

#ifndef PRODUCTIONWELL_H
#define PRODUCTIONWELL_H

#include <tr1/memory>
#include "model/components/well.h"
#include "variables/intvariable.h"
#include "variables/binaryvariable.h"
#include "constraints/constraint.h"
#include "model/pipeconnection.h"
#include "model/components/pipe.h"

using std::tr1::shared_ptr;

class IntVariable;
class BinaryVariable;
class PipeConnection;
class Constraint;
class Pipe;

class ProductionWell : public Well
{
private:
    QVector<shared_ptr<Constraint> > m_bhp_constraints;  //!< Vector of bhp constraint<s
    QVector<PipeConnection*> m_pipe_connections;         //!< Vector of all pipes that could be connected to the well
    shared_ptr<Constraint> p_connection_constraint;      //!< Constraint that makes sure that the sum of flow to pipes = 1
    QVector<WellControl*> m_gaslift_schedule;            //!< Gas lift schedule of the well

public:
    ProductionWell(){}                        //!< Default constructor.
    ProductionWell(const ProductionWell &w);  //!< Copy constructor
    virtual ~ProductionWell();                //!<

    virtual Well* clone() const {return new ProductionWell(*this);}

    void setName(const QString &n) {Well::setName(n);}  //!< Overloads the setName() function in the Well class.
    void setupConstraints();                //!< Set up BHP and routing constraints.
    /*!
     * \brief Check and update BHP constraints.
     *
     * 1. Check that all the connected pipes have the same number of streams as the well.
     *
     * 2. Check that the number of streams is equal to the number of bhp constraints.
     *
     * Print error and exit(1) if either 1. or 2. fails.
     *
     * 3. Update all constraints. The constraints are calculated as \f$ c = (p_{wf} - p_{pipe}) / p_{wf} \f$.
     * when the pipe pressure is higher than the bhp, the constraint is violated, and \f$ c < 0 \f$.
     *
     * 3.1. Check if the well is installed. The constraint value is set to 0.5 if the well is not installed.
     *
     * 3.2. Calculate the weighted average pressure and use it to calculate the constraint value.
     *
     * 3.3. Print a warning if a constraint is violated.
     */
    void updateBhpConstraint();
    void updatePipeConnectionConstraint();  //!< Update the value of the pipeConnectionConstraint if it is not equal to 0.
    /*!
     * \brief Find the fraction of the rates from this Well that flows through Pipe p.
     * \param p Pipe to be checked.
     * \param ok false if the Pipe is not connected to this well
     * \return The fraction of the rates from this Well that flows through Pipe p.
     */
    double flowFraction(Pipe *p, bool *ok = 0);

    virtual void setAutomaticType() {setType(Well::PROD);}  //!< Set type to production well (PROD)
    virtual QString description() const;                    //!< Generate a description of this well for the driver file.

    void addPipeConnection(PipeConnection *c) {m_pipe_connections.push_back(c);}  //!< Add a PipeConnection to this Well.
    void addGasLiftControl(WellControl *c) {m_gaslift_schedule.push_back(c);}     //!< Add a WellControl to the gas lift schedule.

    shared_ptr<Constraint> bhpConstraint(int i) {return m_bhp_constraints.at(i);}        //!< Get the BHP constraints.
    int numberOfBhpConstraints() const {return m_bhp_constraints.size();}                //!< Get the number of BHP contraints.
    shared_ptr<Constraint> pipeConnectionConstraint() {return p_connection_constraint;}  //!< Get the pipe connection constraint.
    int numberOfPipeConnections() const {return m_pipe_connections.size();}              //!< Get the number of pipe connections.
    PipeConnection* pipeConnection(int i) {return m_pipe_connections.at(i);}             //!< Get pipe connection i.
    int numberOfGasLiftControls() const {return m_gaslift_schedule.size();}              //!< Get the number of entries in the gas lift schedule.
    WellControl* gasLiftControl(int i) {return m_gaslift_schedule.at(i);}                //!< Get entry i in the gas lift schedule.
    bool hasGasLift() const {return (m_gaslift_schedule.size() == numberOfControls());}  //!< Check whether this well uses gas lift.

};

#endif // PRODUCTIONWELL_H
