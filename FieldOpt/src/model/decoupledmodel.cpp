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

#include "decoupledmodel.h"

DecoupledModel::DecoupledModel(const DecoupledModel &m)
    : Model(m)
{
    initializeVarsAndCons();
}

DecoupledModel::~DecoupledModel()
{
    for(int i = 0; i < m_mb_cons.size(); ++i)
        delete m_mb_cons.at(i);
    for(int i = 0; i < m_rate_vars.size(); ++i)
        delete m_rate_vars.at(i);
}


void DecoupledModel::initialize()
{
    // initializing all wells, setting up constraints for production wells
    for(int i = 0; i < numberOfWells(); ++i)
    {
        well(i)->initialize();

        // casting the well to production well, setting up constraints if cast is ok
        ProductionWell *prod_well = dynamic_cast<ProductionWell*>(well(i));
        if(prod_well != 0) prod_well->setupConstraints();
    }

    for(int i = 0; i < numberOfPipes(); ++i)
        pipe(i)->initialize(masterSchedule());

    for(int i = 0; i < numberOfCapacities(); ++i)
        capacity(i)->setupConstraints(masterSchedule());


    initializeVarsAndCons();  // setting up the rate input variables

    for(int i = 0; i < numberOfUserDefinedConstraints(); ++i)
        userDefinedConstraint(i)->initialize();
}


void DecoupledModel::initializeVarsAndCons()
{
    for(int i = 0; i < numberOfPipes(); ++i)
    {
        Pipe *p = pipe(i);
        for(int j = 0; j < p->numberOfStreams(); ++j)
        {
            InputRateVariable *irv = new InputRateVariable();
            irv->setPipe(p);
            irv->setStream(p->stream(j));

            shared_ptr<RealVariable> var_oil = shared_ptr<RealVariable>(new RealVariable(p));
            var_oil->setMax(1e6);
            var_oil->setMin(0);
            var_oil->setValue(100);
            var_oil->setName("Input oil rate variable for Pipe #" + QString::number(p->number()) + ", time = " + QString::number(p->stream(j)->time()));

            irv->setOilVariable(var_oil);

            shared_ptr<RealVariable> var_gas = shared_ptr<RealVariable>(new RealVariable(p));
            var_gas->setMax(1e7);
            var_gas->setMin(0);
            var_gas->setValue(1000);
            var_gas->setName("Input gas rate variable for Pipe #" + QString::number(p->number()) + ", time = " + QString::number(p->stream(j)->time()));

            irv->setGasVariable(var_gas);

            shared_ptr<RealVariable> var_water = shared_ptr<RealVariable>(new RealVariable(p));
            var_water->setMax(1e5);
            var_water->setMin(0);
            var_water->setValue(10);
            var_water->setName("Input water rate variable for Pipe #" + QString::number(p->number()) + ", time = " + QString::number(p->stream(j)->time()));

            irv->setWaterVariable(var_water);

            // creating the constraints asociates with the input rate variable
            MaterialBalanceConstraint *mbc = new MaterialBalanceConstraint();
            mbc->setInputRateVariable(irv);
            m_mb_cons.push_back(mbc);

            // adding the input rate variable to the vector
            m_rate_vars.push_back(irv);
        }
    }
}


void DecoupledModel::process()
{
    updateStreams();
    calculatePipePressures();
    updateConstraints();
    updateObjectiveValue();
    setUpToDate(true);
}


bool DecoupledModel::updateConstraints()
{
    bool ok = true;
    ok = updateCommonConstraints();
    updateMaterialBalanceStreams();

    for(int i = 0; i < m_mb_cons.size(); ++i)
        m_mb_cons.at(i)->updateConstraints();

    return ok;
}


void DecoupledModel::updateStreams()
{
    for(int i = 0; i < m_rate_vars.size(); ++i)
        m_rate_vars.at(i)->updateStream();
}


MaterialBalanceConstraint* DecoupledModel::find(Stream *s)
{
    MaterialBalanceConstraint *mbc = 0;

    for(int i = 0; i < m_mb_cons.size(); ++i)
    {
        if(s == m_mb_cons.at(i)->inputRateVariable()->stream())
        {
            mbc = m_mb_cons.at(i);
            break;
        }
    }

    if(mbc == 0)
        s->printToCout();

    return mbc;
}


void DecoupledModel::updateMaterialBalanceStreams()
{
    // emptying the streams in the mbcs
    for(int i = 0; i < m_mb_cons.size(); ++i) m_mb_cons.at(i)->emptyStream();

    // updating the streams in the mbc, starting from the production wells, and working its way up the system
    for(int i = 0; i < numberOfWells(); ++i)
    {
        // trying to cast to production well
        ProductionWell *prod_well = dynamic_cast<ProductionWell*>(well(i));

        if(prod_well != 0)  // this is a production well
        {
            // adding the streams from this well to the upstream pipes connected to it
            addToMaterialBalanceStreamsUpstream(prod_well);

            // looping through the outlet connections of the well, doing the same
            for(int j = 0; j < prod_well->numberOfPipeConnections(); ++j)
            {
                // checking if it is a midpipe or separator
                MidPipe *p_mid = dynamic_cast<MidPipe*>(prod_well->pipeConnection(j)->pipe());
                Separator *p_sep = dynamic_cast<Separator*>(prod_well->pipeConnection(j)->pipe());
                if(p_mid != 0)
                    addToMaterialBalanceStreamsUpstream(p_mid, prod_well, prod_well->pipeConnection(i)->variable()->value());
                else if(p_sep != 0)
                    addToMaterialBalanceStreamsUpstream(p_sep, prod_well, prod_well->pipeConnection(i)->variable()->value());
            } // pipe connection
        } // production well
    } // well
}


void DecoupledModel::addToMaterialBalanceStreamsUpstream(ProductionWell *w)
{
    // looping through the pipes connected to the well
    for(int i = 0; i < w->numberOfPipeConnections(); ++i)
    {
        Pipe *p = w->pipeConnection(i)->pipe();     // pointer to the pipe

        // finding the flow fraction from this well to the pipe
        double frac = w->pipeConnection(i)->variable()->value();

        // calculating the rate from this well to the pipe vs. time
        for(int j = 0; j < w->numberOfStreams(); ++j)
        {
            Stream s = *w->stream(j) * frac;

            // finding the material balance constraint that corresponds to this pipe and time
            MaterialBalanceConstraint *mbc = find(p->stream(j));

            // adding the rate contribution from this well to what is allready in the mbc
            mbc->setStream(s + mbc->stream());
        }
    }
}


void DecoupledModel::addToMaterialBalanceStreamsUpstream(MidPipe *p, Well *from_well, double flow_frac)
{
    // looping through the pipes connected to the pipe
    for(int i = 0; i < p->numberOfOutletConnections(); ++i)
    {
        Pipe *upstream = p->outletConnection(i)->pipe();    // pointer to the upstream pipe

        // finding the flow fraction from this pipe to the upstream pipe
        double frac = p->outletConnection(i)->variable()->value();
        double total_frac = frac * flow_frac;

        // looping through the streams, adding the rate from this pipe
        for(int j = 0; j < p->numberOfStreams(); ++j)
        {
            Stream s = *from_well->stream(j) * total_frac;

            // finding the material balance constraint that corresponds to this pipe and time
            MaterialBalanceConstraint *mbc = find(upstream->stream(j));

            // adding the rate contribution from this well to what is allready in the mbc
            mbc->setStream(s + mbc->stream());
        }
        // then checking if the outlet pipe connection is a midpipe or separator
        MidPipe *p_mid = dynamic_cast<MidPipe*>(upstream);
        Separator *p_sep = dynamic_cast<Separator*>(upstream);

        if(p_mid != 0)
            addToMaterialBalanceStreamsUpstream(p_mid, from_well, total_frac);
        else if(p_sep != 0)
            addToMaterialBalanceStreamsUpstream(p_sep, from_well, total_frac);
    }
}


void DecoupledModel::addToMaterialBalanceStreamsUpstream(Separator *s, Well *from_well, double flow_frac)
{
    // pointer to the upstream connected pipe
    Pipe *upstream = s->outletConnection()->pipe();

    // looping through the streams, adding the contribution from the separator
    for(int i = 0; i < s->numberOfStreams(); ++i)
    {
        Stream str = *from_well->stream(i) * flow_frac;

        // checking if the separator is installed
        if(i >= s->installTime()->value())
        {
            // checking if this is a water or gas separator
            if(s->type() == Separator::WATER)
            {
                // how much water should be removed
                double qw_remove = str.waterRate(true) * s->removeFraction()->value();
                if(qw_remove > s->removeCapacity()->value()) qw_remove = s->removeCapacity()->value();

                // subtracting the removed water
                str.setWaterRate(str.waterRate(true) - qw_remove);
            }
            else if(s->type() == Separator::GAS)
            {
                // how much gas should be removed
                double qg_remove = str.gasRate(true) * s->removeFraction()->value();
                if(qg_remove > s->removeCapacity()->value())
                    qg_remove = s->removeCapacity()->value();

                // subtracting the removed gas
                str.setGasRate(str.gasRate(true) - qg_remove);
            }
        }

        // finding the material balance constraint that corresponds to this pipe and time
        MaterialBalanceConstraint *mbc = find(upstream->stream(i));

        // adding the rate contribution from this well to what is allready in the mbc
        mbc->setStream(str + mbc->stream());
    }

    // then checking if the upstream pipe is a midpipe or separator
    MidPipe *p_mid = dynamic_cast<MidPipe*>(upstream);
    Separator *p_sep = dynamic_cast<Separator*>(upstream);

    if(p_mid != 0)
        addToMaterialBalanceStreamsUpstream(p_mid, from_well, flow_frac);
    else if(p_sep != 0)
        addToMaterialBalanceStreamsUpstream(p_sep, from_well, flow_frac);

}


QVector<shared_ptr<BinaryVariable> >& DecoupledModel::binaryVariables(bool force_refresh)
{
    if(m_vars_binary.size() == 0 || force_refresh)
    {
        if(force_refresh)
            m_vars_binary.resize(0);

        // finding well routnig variables
        for(int i = 0; i < numberOfWells(); i++)
        {
            // checking if this is a production well
            ProductionWell* prod_well = dynamic_cast<ProductionWell*>(well(i));

            if(prod_well != 0)
            {
                // looping through the pipe connections
                for(int j = 0; j < prod_well->numberOfPipeConnections(); j++)
                {
                    if(prod_well->pipeConnection(j)->variable()->isVariable())
                        m_vars_binary.push_back(prod_well->pipeConnection(j)->variable());
                }
            }
        }

        // finding pipe routing variables
        for(int j = 0; j < numberOfPipes(); ++j)
        {
            MidPipe *p_mid = dynamic_cast<MidPipe*>(pipe(j));     // end pipes do not have routing

            if(p_mid != 0)
            {
                // looping through the outlet connections
                for(int j = 0; j < p_mid->numberOfOutletConnections(); j++)
                {
                    if(p_mid->outletConnection(j)->variable()->isVariable())
                        m_vars_binary.push_back(p_mid->outletConnection(j)->variable());
                }
            }
        }
    }
    return m_vars_binary;
}


QVector<shared_ptr<RealVariable> >& DecoupledModel::realVariables(bool force_refresh)
{
    if(m_vars_real.size() == 0 || force_refresh)
    {
        if(force_refresh)
            m_vars_real.resize(0);

        // getting the control variables for the wells
        for(int i = 0; i < numberOfWells(); ++i)     // looping through all the wells
        {
            Well *w = well(i);

            for(int j = 0; j < w->numberOfControls(); j++)  // looping through each element in the wells schedule
            {
                // checking if this shcedule entry is a variable
                if(w->control(j)->controlVar()->isVariable()) m_vars_real.push_back(w->control(j)->controlVar());
            }

            // checking if this is a production well, and if it has gas lift controls
            ProductionWell *prod_well = dynamic_cast<ProductionWell*>(w);
            if(prod_well != 0)
            {
                for(int j = 0; j < prod_well->numberOfGasLiftControls(); ++j)
                {
                    if(prod_well->gasLiftControl(j)->controlVar()->isVariable())
                        m_vars_real.push_back(prod_well->gasLiftControl(j)->controlVar());
                }
            }
        }

        // getting the remove fraction and capacity variables for the separators
        for(int i = 0; i < numberOfPipes(); ++i)    // looping through the pipes, finding the separators
        {
            Separator *s = dynamic_cast<Separator*>(pipe(i));

            if(s != 0)  // this is a separator
            {
                if(s->removeFraction()->isVariable())
                    m_vars_real.push_back(s->removeFraction());
                if(s->removeCapacity()->isVariable())
                    m_vars_real.push_back(s->removeCapacity());
            }
        }

        // getting the input rate variables
        for(int i = 0; i < m_rate_vars.size(); ++i)
        {
            m_vars_real.push_back(m_rate_vars.at(i)->oilVariable());
            m_vars_real.push_back(m_rate_vars.at(i)->gasVariable());
            m_vars_real.push_back(m_rate_vars.at(i)->waterVariable());
        }
    }
    return m_vars_real;
}


QVector<shared_ptr<IntVariable> >& DecoupledModel::integerVariables(bool force_refresh)
{

    if(m_vars_integer.size() == 0 || force_refresh)
    {
        if(force_refresh)
            m_vars_integer.resize(0);

        // collecting the install time variables for the separators
        for(int i = 0; i < numberOfPipes(); ++i)     // looping through all the pipes
        {
            // checking if this is a separator
            Separator *s = dynamic_cast<Separator*>(pipe(i));

            if(s != 0)
            {
                if(s->installTime()->isVariable())
                    m_vars_integer.push_back(s->installTime());  // adding install time if it is a variable
            }

        }

        // collecting the install time variables for the wells
        for(int i = 0 ; i < numberOfWells(); ++i)
        {
            // checking if the well has an install time variable
            if(well(i)->hasInstallTime())
            {
                if(well(i)->installTime()->isVariable())
                    m_vars_integer.push_back(well(i)->installTime());  // adding install time if it is a variable
            }
        }
    }
    return m_vars_integer;
}


QVector<shared_ptr<Constraint> >& DecoupledModel::constraints(bool force_refresh)
{
    if(m_cons.size() == 0 || force_refresh)
    {
        if(force_refresh) m_cons.resize(0);
        // getting the well bhp constraints
        for(int i = 0; i < numberOfWells(); ++i)
        {
            // checking if this is a production well
            ProductionWell* prod_well = dynamic_cast<ProductionWell*>(well(i));
            if(prod_well != 0)
            {
                for(int i = 0; i < prod_well->numberOfBhpConstraints(); ++i) m_cons.push_back(prod_well->bhpConstraint(i));
            }
        }

        // getting the well pipe connection constraints
        for(int i = 0; i < numberOfWells(); ++i)
        {
            // checking if this is a production well
            ProductionWell* prod_well = dynamic_cast<ProductionWell*>(well(i));
            if(prod_well != 0)
            {
                if(prod_well->pipeConnectionConstraint() != 0) m_cons.push_back(prod_well->pipeConnectionConstraint());
            }
        }

        // getting the mid pipe connection constraints
        for(int i = 0; i < numberOfPipes(); ++i)
        {
            // checking if this is a mid pipe
            MidPipe *p_mid = dynamic_cast<MidPipe*>(pipe(i));
            if(p_mid != 0) m_cons.push_back(p_mid->outletConnectionConstraint());
        }

        // getting the separator capacity constraints
        for(int i = 0; i < numberOfCapacities(); ++i)
        {
            Capacity *sep = capacity(i);
            m_cons += sep->gasConstraints();
            m_cons += sep->oilConstraints();
            m_cons += sep->waterConstraints();
            m_cons += sep->liquidConstraints();
        }

        // getting the material balance constraints
        for(int i = 0; i < m_mb_cons.size(); ++i)
        {
            MaterialBalanceConstraint *mbc = m_mb_cons.at(i);
            m_cons.push_back(mbc->oilConstraint());
            m_cons.push_back(mbc->gasConstraint());
            m_cons.push_back(mbc->waterConstraint());
        }

        // getting the user defined constraints
        for(int i = 0; i < numberOfUserDefinedConstraints(); ++i)
        {
            m_cons.push_back(userDefinedConstraint(i)->constraint());
        }
    }
    return m_cons;
}


QVector<shared_ptr<RealVariable> > DecoupledModel::realVariables(Component *c)
{
    QVector<shared_ptr<RealVariable> > comp_vars;

    // looping through all the real variables
    for(int i = 0; i < realVariables().size(); ++i)
    {
        if(realVariables().at(i)->parent()->id() == c->id())
        {
            comp_vars.push_back(realVariables().at(i));
        }
    }
    return comp_vars;
}
