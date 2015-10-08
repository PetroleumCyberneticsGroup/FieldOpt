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

#ifndef COUPLEDMODEL_H
#define COUPLEDMODEL_H

#include "model.h"
#include "Library/variables/realvariable.h"
#include "Library/variables/intvariable.h"
#include "Library/variables/binaryvariable.h"
#include "Library/constraints/constraint.h"

#include "coupledmodel.h"
#include "components/midpipe.h"
#include "pipeconnection.h"
#include "Library/variables/binaryvariable.h"
#include "Library/variables/realvariable.h"
#include "Library/variables/intvariable.h"
#include "stream.h"
#include "components/productionwell.h"
#include "components/separator.h"
#include "components/pressurebooster.h"
#include "Library/constraints/capacity.h"
#include "Library/constraints/userconstraint.h"
#include "wellconnectionvariable.h"
#include "wellpath.h"

/*!
 * \brief A model where the mass balance is always feasible.
 *
 * In a coupled model, the output streams from an upstream part of the project is transfered as input
 * streams to the downstream parts of the project.
 * This ensures that the mass balance is always honored.
 */
class CoupledModel : public Model
{
private:

    QVector<shared_ptr<BinaryVariable> > m_vars_binary;     //!< Vector containing all binary variables
    QVector<shared_ptr<RealVariable> > m_vars_real;         //!< Vector containing all real variables
    QVector<shared_ptr<IntVariable> > m_vars_integer;       //!< Vector containing all integer variables
    QVector<shared_ptr<Constraint> > m_cons;                //!< Vector containing all the constraints

    void addStreamsUpstream(ProductionWell *w);                                      //!< Adds the streams flowing from this well to the upstream connected pipes
    void addStreamsUpstream(MidPipe *p, Well *from_well, double flow_frac);          //!< Adds the streams flowing from this pipe to the upstream connected pipes (or separators)
    void addStreamsUpstream(Separator *s, Well *from_well, double flow_frac);        //!< Adds the streams flowing from this separator to the upstream connected pipe
    void addStreamsUpstream(PressureBooster *b, Well *from_well, double flow_frac);  //!< Adds the streams flowing from this booster to the upstream connected pipe

public:
    CoupledModel();
    CoupledModel(const CoupledModel &m) : Model(m){}
    virtual ~CoupledModel();

    virtual Model* clone() const {return new CoupledModel(*this);}
    virtual QString description() const {return QString("COUPLED MODEL\n\n");}
    virtual void initialize();  //!< Initialize the model and set up constraints.
    virtual void process();     //!< Process the model after the reservoir simulation is run.
    virtual void updateStreams();  //!< Update the rates flowing through every element in the model.
    virtual bool updateConstraints();  //!< Update the rates flowing through every element in the model.

    virtual QVector<shared_ptr<BinaryVariable> >& binaryVariables(bool force_refresh = false);  //!< Collect all binary variables.
    virtual QVector<shared_ptr<RealVariable> >& realVariables(bool force_refresh = false);      //!< Collect all real variables.
    virtual QVector<shared_ptr<IntVariable> >& integerVariables(bool force_refresh = false);    //!< Collect all integer variables.
    virtual QVector<shared_ptr<Constraint> >& constraints(bool force_refresh = false);          //!< Collect all constraints.
    virtual QVector<shared_ptr<RealVariable> > realVariables(Component *c);                     //!< Collect all real variables for a component.

    virtual int numberOfRealVariables() const {return m_vars_real.size();}
    virtual int numberOfBinaryVariables() const {return m_vars_binary.size();}
    virtual int numberOfIntegerVariables() const {return m_vars_integer.size();}
    virtual int numberOfConstraints() const {return m_cons.size();}

    virtual double realVariableValue(int i) const {return m_vars_real.at(i)->value();}
    virtual double binaryVariableValue(int i) const {return m_vars_binary.at(i)->value();}
    virtual double integerVariableValue(int i) const {return m_vars_integer.at(i)->value();}
    virtual double constraintValue(int i) const {return m_cons.at(i)->value();}
};

#endif // COUPLEDMODEL_H
