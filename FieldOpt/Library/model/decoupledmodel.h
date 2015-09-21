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

#ifndef DECOUPLEDMODEL_H
#define DECOUPLEDMODEL_H

#include "model.h"
#include "stream.h"
#include "pipeconnection.h"

#include "variables/binaryvariable.h"
#include "variables/realvariable.h"
#include "variables/intvariable.h"
#include "inputratevariable.h"

#include "constraints/constraint.h"
#include "constraints/userconstraint.h"
#include "constraints/capacity.h"
#include "constraints/materialbalanceconstraint.h"

#include "components/productionwell.h"
#include "components/midpipe.h"
#include "components/separator.h"

class InputRateVariable;
class MaterialBalanceConstraint;
class ProductionWell;
class MidPipe;
class Separator;
class Stream;

/*!
 * \brief Model where input rates to all parts of the system are treated as variables.
 *
 * In this type of Model there is no automatic link between the output of an upstream part and the input to a downstream part.
 * The input rates to every pipe segment in the Model is included as a Variable.
 * Constraints are included to honour the mass balance in the system (\f[c = q_{in} - q_{out} = 0\f]).
 *
 * \todo Document the public functions.
 *
 * \todo Seems to only support production wells?
 */
class DecoupledModel : public Model
{
private:
    QVector<shared_ptr<BinaryVariable> > m_vars_binary;  //!< vector containing all binary variables
    QVector<shared_ptr<RealVariable> > m_vars_real;      //!< vector containing all real variables
    QVector<shared_ptr<IntVariable> > m_vars_integer;    //!< vector containing all integer variables
    QVector<shared_ptr<Constraint> > m_cons;             //!< vector containing all the constraints
    QVector<InputRateVariable*> m_rate_vars;             //!< all the varaibles for rate input to the different parts of the model
    QVector<MaterialBalanceConstraint*> m_mb_cons;       //!< constraints associated with the input rate variables for mass balance feasibility

    void initializeVarsAndCons();
    void addToMaterialBalanceStreamsUpstream(ProductionWell *w);                                //!< Add the streams flowing from this well to the streams in the asociated material balance constraints.
    void addToMaterialBalanceStreamsUpstream(MidPipe *p, Well *from_well, double flow_frac);    //!< Add the streams flowing from this pipe to the streams in the asociated material balance constraints.
    void addToMaterialBalanceStreamsUpstream(Separator *s, Well *from_well, double flow_frac);  //!< Add the streams flowing from this separator to the streams in the asociated material balance constraints.
    void updateMaterialBalanceStreams();                                                        //!< Update all the streams in the material balance constraints
    MaterialBalanceConstraint* find(Stream *s);                                                 //!< Findthe material balance constraint that is connected to this Stream.

public:
    DecoupledModel() {}                       //!< Default constructor.
    DecoupledModel(const DecoupledModel &m);  //!< Copy constructor.
    virtual ~DecoupledModel();                //!< Default destructor.

    virtual Model* clone() const {return new DecoupledModel(*this);}              //!< Get a clone of this Model.
    virtual QString description() const {return QString("DECOUPLED MODEL\n\n");}  //!< Get a description of this Model for the driver file.
    virtual void initialize();                                                    //!< Initialize the model, setting uo constraints.
    virtual void process();                                                       //!< Process the model after the reservoir simulator is run.
    virtual void updateStreams();                                                 //!< Update the reates flowing through every element in the model.
    virtual bool updateConstraints();                                             //!< Update the values of the constraints.

    virtual QVector<shared_ptr<BinaryVariable> >& binaryVariables(bool force_refresh = false);
    virtual QVector<shared_ptr<RealVariable> >& realVariables(bool force_refresh = false);
    virtual QVector<shared_ptr<IntVariable> >& integerVariables(bool force_refresh = false);
    virtual QVector<shared_ptr<Constraint> >& constraints(bool force_refresh = false);          //!< \todo The part of this function that is common between Coupled and Decoupled model should be put back into the Model class
    virtual QVector<shared_ptr<RealVariable> > realVariables(Component *c);

    virtual int numberOfRealVariables() const {return m_vars_real.size();}
    virtual int numberOfBinaryVariables() const {return m_vars_binary.size();}
    virtual int numberOfIntegerVariables() const {return m_vars_integer.size();}
    virtual int numberOfConstraints() const {return m_cons.size();}

    virtual double realVariableValue(int i) const {return m_vars_real.at(i)->value();}
    virtual double binaryVariableValue(int i) const {return m_vars_binary.at(i)->value();}
    virtual double integerVariableValue(int i) const {return m_vars_integer.at(i)->value();}
    virtual double constraintValue(int i) const {return m_cons.at(i)->value();}

};

#endif // DECOUPLEDMODEL_H
