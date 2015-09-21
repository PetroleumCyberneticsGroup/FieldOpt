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

#include "coupledmodel.h"

CoupledModel::CoupledModel()
{

}

CoupledModel::~CoupledModel()
{

}

void CoupledModel::initialize()
{

    // initializing all wells, setting up constraints for production wells
    for(int i = 0; i < numberOfWells(); ++i)
    {
        // initializing the well
        well(i)->initialize();

        // casting the well to production well, setting up constraints if cast is ok
        ProductionWell *prod_well = dynamic_cast<ProductionWell*>(well(i));
        if(prod_well != 0)
            prod_well->setupConstraints();
    }

    // initializing the pipes
    for(int i = 0; i < numberOfPipes(); ++i)
    {
        pipe(i)->initialize(masterSchedule());

        // checking if this is a Booster
        PressureBooster *p_boost = dynamic_cast<PressureBooster*>(pipe(i));
        if(p_boost != 0)
            p_boost->setupCapacityConstraints(masterSchedule());   // if so, setting up the capacity constraints
    }

    // setting up the constraints for the capacities
    for(int i = 0; i < numberOfCapacities(); ++i)
        capacity(i)->setupConstraints(masterSchedule());

    // initializing the user defined constraints
    for(int i = 0; i < numberOfUserDefinedConstraints(); ++i)
        userDefinedConstraint(i)->initialize();
}

void CoupledModel::process()
{
    updateStreams();
    calculatePipePressures();
    updateConstraints();
    updateObjectiveValue();
    setUpToDate(true);
}

void CoupledModel::updateStreams()
{
    // first need to empty all the streams in all pipes
    for(int i = 0; i < numberOfPipes(); ++i) pipe(i)->emptyStreams();

    // starting with the production wells, feeding the rates to the connected pipes
    for(int i = 0; i < numberOfWells(); ++i)
    {
        // trying to cast to production well
        ProductionWell *prod_well = dynamic_cast<ProductionWell*>(well(i));

        if(prod_well != 0)  // this is a production well
        {
            // adding the streams from this well to the upstream pipes connected to it
            addStreamsUpstream(prod_well);

            // looping through the outlet connections of the well, doing the same
            for(int j = 0; j < prod_well->numberOfPipeConnections(); ++j)
            {
                // checking if it is a midpipe, separator or booster
                MidPipe *p_mid = dynamic_cast<MidPipe*>(prod_well->pipeConnection(j)->pipe());
                Separator *p_sep = dynamic_cast<Separator*>(prod_well->pipeConnection(j)->pipe());
                PressureBooster *p_boost = dynamic_cast<PressureBooster*>(prod_well->pipeConnection(j)->pipe());

                if(p_mid != 0) addStreamsUpstream(p_mid, prod_well, prod_well->pipeConnection(j)->variable()->value());
                else if(p_sep != 0) addStreamsUpstream(p_sep, prod_well, prod_well->pipeConnection(j)->variable()->value());
                else if(p_boost != 0) addStreamsUpstream(p_boost, prod_well, prod_well->pipeConnection(j)->variable()->value());
            } // pipe connection
        } // production well
    } // well
}

bool CoupledModel::updateConstraints()
{
    // the coupled model does not have any custom constraints
    bool ok = updateCommonConstraints();  // updates the constraints that are common for all model types
    return ok;
}

void CoupledModel::addStreamsUpstream(ProductionWell *w)
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

            // adding the rate contribution from this well to what is allready going through the pipe
            p->addToStream(j, s);
        }
    }
}

void CoupledModel::addStreamsUpstream(MidPipe *p, Well *from_well, double flow_frac)
{
    // looping through the pipes connected to the pipe
    for(int i = 0; i < p->numberOfOutletConnections(); ++i)
    {
        Pipe *upstream = p->outletConnection(i)->pipe();    // pointer to the upstream pipe

        // finding the flow fraction from this pipe to the upstream pipe
        double frac = p->outletConnection(i)->variable()->value();
        double total_frac = frac*flow_frac;

        // looping through the streams, adding the rate from this pipe
        for(int j = 0; j < p->numberOfStreams(); ++j)
        {
            Stream s = *from_well->stream(j) * total_frac;

            // adding the contribution to the upstream pipe
            upstream->addToStream(j, s);
        }

        // then checking if the outlet pipe connection is a midpipe or separator
        MidPipe *p_mid = dynamic_cast<MidPipe*>(upstream);
        Separator *p_sep = dynamic_cast<Separator*>(upstream);
        PressureBooster *p_boost = dynamic_cast<PressureBooster*>(upstream);

        if(p_mid != 0) addStreamsUpstream(p_mid, from_well, total_frac);
        else if(p_sep != 0) addStreamsUpstream(p_sep, from_well, total_frac);
        else if(p_boost != 0) addStreamsUpstream(p_boost, from_well, total_frac);

    }
}

void CoupledModel::addStreamsUpstream(Separator *s, Well *from_well, double flow_frac)
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
                double qw_remove = str.waterRate() * s->removeFraction()->value();

                if(qw_remove > s->remainingCapacity(i)) qw_remove = s->remainingCapacity(i);

                // subtracting the removed water
                str.setWaterRate(str.waterRate() - qw_remove);

                // subrtacting from the remaining capacity
                s->reduceRemainingCapacity(i, qw_remove);
            }
            else if(s->type() == Separator::GAS)
            {
                // how much gas should be removed
                double qg_remove = str.gasRate() * s->removeFraction()->value();
                if(qg_remove > s->remainingCapacity(i)) qg_remove = s->remainingCapacity(i);

                // subtracting the removed gas
                str.setGasRate(str.gasRate() - qg_remove);

                // subrtacting from the remaining capacity
                s->reduceRemainingCapacity(i, qg_remove);
            }
        }
        // adding the stream to the upstream pipe
        upstream->addToStream(i, str);
    }

    // then checking if the upstream pipe is a midpipe or separator
    MidPipe *p_mid = dynamic_cast<MidPipe*>(upstream);
    Separator *p_sep = dynamic_cast<Separator*>(upstream);
    PressureBooster *p_boost = dynamic_cast<PressureBooster*>(upstream);

    if(p_mid != 0) addStreamsUpstream(p_mid, from_well, flow_frac);
    else if(p_sep != 0) addStreamsUpstream(p_sep, from_well, flow_frac);
    else if(p_boost != 0) addStreamsUpstream(p_boost, from_well, flow_frac);

}

//-----------------------------------------------------------------------------------------------
// adds the rates from the booster to all upstream connections
//-----------------------------------------------------------------------------------------------
void CoupledModel::addStreamsUpstream(PressureBooster *b, Well *from_well, double flow_frac)
{
    // pointer to the upstream connected pipe
    Pipe *upstream = b->outletConnection()->pipe();

    // looping through the streams, adding the contribution from the separator
    for(int i = 0; i < b->numberOfStreams(); ++i)
    {
        Stream str = *from_well->stream(i) * flow_frac;

        // adding the stream to the upstream pipe
        upstream->addToStream(i, str);
    }

    // then checking if the upstream pipe is a midpipe, separator, or booster
    MidPipe *p_mid = dynamic_cast<MidPipe*>(upstream);
    Separator *p_sep = dynamic_cast<Separator*>(upstream);
    PressureBooster *p_boost = dynamic_cast<PressureBooster*>(upstream);

    if(p_mid != 0) addStreamsUpstream(p_mid, from_well, flow_frac);
    else if(p_sep != 0) addStreamsUpstream(p_sep, from_well, flow_frac);
    else if(p_boost != 0) addStreamsUpstream(p_boost, from_well, flow_frac);

}

QVector<shared_ptr<BinaryVariable> >& CoupledModel::binaryVariables(bool force_refresh)
{
    if(m_vars_binary.size() == 0 || force_refresh)
    {
        if(force_refresh) m_vars_binary.resize(0);

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
                    if(prod_well->pipeConnection(j)->variable()->isVariable()) m_vars_binary.push_back(prod_well->pipeConnection(j)->variable());
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
                    if(p_mid->outletConnection(j)->variable()->isVariable()) m_vars_binary.push_back(p_mid->outletConnection(j)->variable());
                }
            }
        }
    }
    return m_vars_binary;
}

QVector<shared_ptr<RealVariable> >& CoupledModel::realVariables(bool force_refresh)
{
    if(m_vars_real.size() == 0 || force_refresh)
    {
        if(force_refresh) m_vars_real.resize(0);
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
                    if(prod_well->gasLiftControl(j)->controlVar()->isVariable()) m_vars_real.push_back(prod_well->gasLiftControl(j)->controlVar());
                }
            }
        }
        for(int i = 0; i < numberOfPipes(); ++i)    // looping through the pipes, finding the separators and boosters
        {
            // checking if this is a Separator
            Separator *s = dynamic_cast<Separator*>(pipe(i));
            if(s != 0)
            {
                if(s->removeFraction()->isVariable()) m_vars_real.push_back(s->removeFraction());
                if(s->removeCapacity()->isVariable()) m_vars_real.push_back(s->removeCapacity());
            }

            // checking if this is a Booster
            PressureBooster *b = dynamic_cast<PressureBooster*>(pipe(i));
            if(b != 0)
            {
                if(b->pressureVariable()->isVariable()) m_vars_real.push_back(b->pressureVariable());
                if(b->capacityVariable()->isVariable()) m_vars_real.push_back(b->capacityVariable());
            }
        }
    }
    return m_vars_real;
}

QVector<shared_ptr<IntVariable> >& CoupledModel::integerVariables(bool force_refresh)
{
    if(m_vars_integer.size() == 0 || force_refresh)
    {
        if(force_refresh) m_vars_integer.resize(0);

        // collecting the install time variables for the separators and boosters
        for(int i = 0; i < numberOfPipes(); ++i)     // looping through all the pipes
        {
            // checking if this is a separator
            Separator *s = dynamic_cast<Separator*>(pipe(i));
            if(s != 0)
            {
                if(s->installTime()->isVariable()) m_vars_integer.push_back(s->installTime());  // adding install time if it is a variable
            }

            // checking if this is a Booster
            PressureBooster *b = dynamic_cast<PressureBooster*>(pipe(i));
            if(b != 0)
            {
                if(b->installTime()->isVariable()) m_vars_integer.push_back(b->installTime());
            }
        }

        // collecting the install time variables and connection variables for the wells
        for(int i = 0 ; i < numberOfWells(); ++i)
        {
            // checking if the well has an install time variable
            if(well(i)->hasInstallTime())
            {
                if(well(i)->installTime()->isVariable()) m_vars_integer.push_back(well(i)->installTime());  // adding install time if it is a variable
            }

            // checking if the well has connection variables
            if(well(i)->hasVariableConnections())
            {
                for(int j = 0; j < well(i)->numberOfVariableConnections(); ++j)
                {
                    if(well(i)->variableConnection(j)->iVariable()->isVariable()) m_vars_integer.push_back(well(i)->variableConnection(j)->iVariable());
                    if(well(i)->variableConnection(j)->jVariable()->isVariable()) m_vars_integer.push_back(well(i)->variableConnection(j)->jVariable());
                }
            }

            // checking if the well has a well path
            if(well(i)->hasWellPath())
            {
                m_vars_integer += well(i)->wellPath()->variables();
            }
        }
    }
    return m_vars_integer;
}

QVector<shared_ptr<Constraint> >& CoupledModel::constraints(bool force_refresh)
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

        // getting the pipe constraints (midpipe connection and booster capacity)
        for(int i = 0; i < numberOfPipes(); i++)
        {
            // checking if this is a mid pipe
            MidPipe *p_mid = dynamic_cast<MidPipe*>(pipe(i));

            if(p_mid != 0) m_cons.push_back(p_mid->outletConnectionConstraint());

            // checking if this is a booster
            PressureBooster* p_boost = dynamic_cast<PressureBooster*>(pipe(i));

            if(p_boost != 0)
            {
                m_cons += p_boost->capacityConstraints();
            }
        }

        // getting the capacity constraints
        for(int i = 0; i < numberOfCapacities(); ++i)
        {
            Capacity *sep = capacity(i);
            m_cons += sep->gasConstraints();
            m_cons += sep->oilConstraints();
            m_cons += sep->waterConstraints();
            m_cons += sep->liquidConstraints();
        }

        // getting the user defined constraints
        for(int i = 0; i < numberOfUserDefinedConstraints(); ++i)
        {
            m_cons.push_back(userDefinedConstraint(i)->constraint());
        }
    }
    return m_cons;
}

QVector<shared_ptr<RealVariable> > CoupledModel::realVariables(Component *c)
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
