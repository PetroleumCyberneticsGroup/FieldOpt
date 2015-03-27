#include "model.h"




RuntimeSettings *Model::getRuntimeSettings() const
{
    return runtimeSettings;
}

void Model::setRuntimeSettings(RuntimeSettings *value)
{
    runtimeSettings = value;
}
bool Model::updateCapacityConstraints()
{
    bool ok = true;
    for(int i = 0; i < numberOfCapacities(); i++)
    {
        capacity(i)->updateConstraints();
    }
    return ok;
}

bool Model::updateWellConstaints()
{
    bool ok = true;
    for(int i = 0; i < numberOfWells(); i++)
    {
        // checking if this is a production well
        ProductionWell *prod_well = dynamic_cast<ProductionWell*>(well(i));

        if(prod_well != 0)
        {
            prod_well->updateBhpConstraint();
            prod_well->updatePipeConnectionConstraint();
        }
    }
    return ok;
}

bool Model::updatePipeConstraints()
{
    bool ok = true;
    for(int i = 0; i < numberOfPipes(); ++i)
    {
        // checking if this is a mid pipe
        MidPipe *p_mid = dynamic_cast<MidPipe*>(pipe(i));
        if(p_mid != 0)
        {
            p_mid->updateOutletConnectionConstraint();
        }
    }
    return ok;
}

bool Model::updateBoosterConstraints()
{
    bool ok = true;
    for(int i = 0; i < numberOfPipes(); ++i)
    {
        // checking if this is a booster
        PressureBooster *p_boost = dynamic_cast<PressureBooster*>(pipe(i));
        if(p_boost != 0)
        {
            p_boost->updateCapacityConstraints();
        }
    }
    return ok;
}

bool Model::updateUserDefinedConstraints()
{
    bool ok = true;
    for(int i = 0; i < numberOfUserDefinedConstraints(); ++i)
    {
        if(!userDefinedConstraint(i)->update()) ok = false;
    }
    return ok;
}

QVector<Cost *> Model::sortCosts(QVector<Cost *> c)
{
    QVector<Cost*> result;

    // adding the first element to the results
    if(c.size() > 0) result.push_back(c.at(0));

    // looping through the unsorted costs
    for(int i = 1; i < c.size(); ++i)
    {
        Cost *current_unsorted = c.at(i);

        // finding the correct place in the results:
        bool inserted = false;
        for(int j = 0; j < result.size(); ++j)
        {
            // checking if the current_unsorted < result
            if(current_unsorted->time() <= result.at(j)->time())
            {
                result.insert(j, current_unsorted);
                inserted = true;
                break;
            }
        }
        if(!inserted) result.push_back(current_unsorted);
    }
    return result;
}

Model::Model() :
    p_reservoir(0),
    p_obj(0),
    m_up_to_date(false)
{
    runtimeSettings = new RuntimeSettings();
}

Model::Model(const Model &m)
{
    m_up_to_date = false;
    m_driver_path = m.m_driver_path;
    p_reservoir = new Reservoir(*m.reservoir());
    for(int i = 0; i < m.numberOfWells(); i++)
        m_wells.push_back(m.well(i)->clone());
    for(int i = 0; i < m.numberOfPipes(); i++)
        m_pipes.push_back(m.pipe(i)->clone());
    for(int i = 0; i < m.numberOfCapacities(); i++)
        m_capacities.push_back(new Capacity(*m.m_capacities.at(i)));
    p_obj = m.p_obj->clone();
    m_master_schedule = m.m_master_schedule;
    for(int i = 0; i < m.m_user_constraints.size(); ++i)
        m_user_constraints.push_back(new UserConstraint(*m.m_user_constraints.at(i), this));
}

Model::~Model()
{
    if(p_reservoir != 0)
        delete p_reservoir;
    for(int i = 0; i < m_wells.size(); i++)
        delete m_wells.at(i);
    for(int i = 0; i < m_pipes.size(); i++)
        delete m_pipes.at(i);
    for(int i = 0; i < m_capacities.size(); i++)
        delete m_capacities.at(i);
    for(int i = 0; i < m_user_constraints.size(); ++i)
        delete m_user_constraints.at(i);
}

bool Model::validate()
{
    bool ok = true;

    // first checking that the reservoir is defined
    if(p_reservoir == 0)
    {
        emitException(ExceptionSeverity::WARNING, ExceptionType::MODEL_VALIDATION, QString("No reservoir defined."));
        ok = false;
    }

    // checking that at least one well is defined
    if(numberOfWells() == 0)
    {
        emitException(ExceptionSeverity::WARNING, ExceptionType::MODEL_VALIDATION, QString("No well defined."));
        ok = false;
    }

    // checking that at least one pipe is defined
    if(numberOfPipes() == 0)
    {
        emitException(ExceptionSeverity::WARNING, ExceptionType::MODEL_VALIDATION, QString("No pipe defined."));
        ok = false;
    }

    // checking that the master schedule at least containts one entry
    if(numberOfMasterScheduleTimes() < 1)
    {
        emitException(ExceptionSeverity::WARNING, ExceptionType::MODEL_VALIDATION, QString("Master schedule does not contain any entries."));
        ok = false;
    }

    // checking that the master schedule corresponds to all well schedules
    for(int i = 0; i < numberOfWells(); ++i)
    {
        // first checking that the schedules have the same size
        if(well(i)->numberOfControls() != numberOfMasterScheduleTimes())
        {
            QString message = QString("Well %1\nDoes not have the same number of schedule entries as the master schedule.").arg(well(i)->name().toLatin1().constData());
            emitException(ExceptionSeverity::WARNING, ExceptionType::MODEL_VALIDATION, message);
            ok = false;
            break;
        }
        else
        {
            // checking that each time entry in the well is the same as in the master schedule
            for(int j = 0; j < numberOfMasterScheduleTimes(); ++j)
            {
                if(well(i)->control(j)->endTime() != masterScheduleTime(j))
                {
                    QString message = QString("Well %1\nSchedule entry:%2\nIs not found in the master schedule.").arg(well(i)->name().toLatin1().constData()).arg(well(i)->control(j)->endTime());
                    emitException(ExceptionSeverity::WARNING, ExceptionType::MODEL_VALIDATION, message);
                    ok = false;
                    break;
                }
            } // master schedule entries
        }

    } // well

    // checking that the master schedule corresponds to the gas lift schedule for the production wells that have gas lift
    for(int i = 0; i < numberOfWells(); ++i)
    {
        // checking if this is a production well
        ProductionWell *prod_well = dynamic_cast<ProductionWell*>(well(i));

        if(prod_well != 0)
        {
            // checking if this production well has gas lift
            if(prod_well->hasGasLift())
            {
                // first checking that the schedules have the same size
                if(prod_well->numberOfGasLiftControls() != numberOfMasterScheduleTimes())
                {
                    QString message = QString("Well %1\nDoes not have the same number of gaslift entries as the master schedule.").arg(well(i)->name().toLatin1().constData());
                    emitException(ExceptionSeverity::WARNING, ExceptionType::MODEL_VALIDATION, message);
                    ok = false;
                    break;
                }
                else
                {
                    // checking that each time entry in the well is the same as in the master schedule
                    for(int j = 0; j < numberOfMasterScheduleTimes(); ++j)
                    {
                        if(prod_well->gasLiftControl(j)->endTime() != masterScheduleTime(j))
                        {
                            QString message = QString("Well %1\nGaslift entry: %2\n Is not found in the master schedule.").arg(well(i)->name().toLatin1().constData()).arg(prod_well->gasLiftControl(j)->endTime());
                            emitException(ExceptionSeverity::WARNING, ExceptionType::MODEL_VALIDATION, message);
                            ok = false;
                            break;
                        }
                    } // master schedule entries
                }
            } // has gas lift
        } // production well
    }   // well

    return ok;
}

bool Model::resolvePipeRouting()
{
    bool ok = true;


    for(int k = 0; k < m_pipes.size(); ++k)  // first cleaning up the current feeds connected to the pipes
        m_pipes.at(k)->cleanFeedConnections();

    // connecting the wells
    for(int i = 0; i < m_wells.size(); ++i)     // looping through each well
    {
        ProductionWell *prod_well = dynamic_cast<ProductionWell*>(well(i));  // checking if this is a production well

        if(prod_well != 0)     // skipping injection wells
        {
            // looping through the pipe connections for the well
            for(int k = 0; k < prod_well->numberOfPipeConnections(); ++k)
            {
                bool connection_ok;
                int pipe_num = prod_well->pipeConnection(k)->pipeNumber();

                for(int j = 0; j < m_pipes.size(); j++)     // looping through the pipes to find the correct one
                {
                    if(m_pipes.at(j)->number() == pipe_num)     // found the correct pipe
                    {
                        m_pipes.at(j)->addFeedWell(prod_well);              // adding the well as a feed to the pipe
                        prod_well->pipeConnection(k)->setPipe(m_pipes.at(j));   // setting the pipe as outlet pipe for the pipe connection

                        connection_ok = true;
                        break;
                    }
                } // pipe for pipe connection k

                // checking if the well - pipe connection was ok
                if(!connection_ok)
                {
                    QString message = QString("Well to Pipe connection could not be established.\nPipe: %1\nWell: %2").arg(pipe_num).arg(prod_well->name().toLatin1().constData());
                    emitException(ExceptionSeverity::ERROR, ExceptionType::MODEL_VALIDATION, message);
                }

            } // pipe connections for well i

            // checking that the well is at least connected to one pipe
            if(prod_well->numberOfPipeConnections() == 0)
            {
                QString message = QString("Well %1 is not connected to any pipe.").arg(prod_well->name().toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::MODEL_VALIDATION, message);
            }
        }
    }

    // connecting the pipes
    for(int i = 0; i < m_pipes.size(); i++)     // looping through the pipes
    {
        // only the MidPipe, Separator, and PressureBooster types should be checked
        MidPipe *p_mid = dynamic_cast<MidPipe*>(m_pipes.at(i));
        Separator *p_sep = dynamic_cast<Separator*>(m_pipes.at(i));
        PressureBooster *p_boost = dynamic_cast<PressureBooster*>(m_pipes.at(i));

        if(p_mid != 0)  // this is a MidPipe
        {
            // looping through the outlet connections for the pipe
            for(int k = 0; k < p_mid->numberOfOutletConnections(); k++)
            {
                bool pipe_ok = false;

                int pipe_num = p_mid->outletConnection(k)->pipeNumber();

                // checking if it is connected to it self
                if(pipe_num == p_mid->number())
                {
                    QString message = QString("Pipe %1 is connected to itself.").arg(pipe_num);
                    emitException(ExceptionSeverity::ERROR, ExceptionType::MODEL_VALIDATION, message);
                }

                // looping through the pipes to find the correct one
                for(int j = 0; j < numberOfPipes(); j++)
                {
                    if(pipe_num == pipe(j)->number())
                    {
                        p_mid->outletConnection(k)->setPipe(pipe(j));
                        pipe(j)->addFeedPipe(p_mid);
                        pipe_ok = true;
                        break;
                    }
                }

                // checking if the pipe - pipe connection was ok
                if(!pipe_ok)
                {
                    QString message = QString("Pipe to Pipe connection could not be established.\nUpstream Pipe: %1\n Downstream Pipe: %2").arg(pipe_num).arg(pipe(i)->number());
                    emitException(ExceptionSeverity::ERROR, ExceptionType::MODEL_VALIDATION, message);
                }
            } // connection k

            // checking that the pipe has at least one outlet connection
            if(p_mid->numberOfOutletConnections() == 0)
            {
                QString message = QString("Pipe %1 is not connected to any upstream pipe.").arg(p_mid->number());
                emitException(ExceptionSeverity::ERROR, ExceptionType::MODEL_VALIDATION, message);
            }

        } // midpipe
        else if(p_sep != 0) // this is a Separator
        {
            bool pipe_ok = false;
            int pipe_num = p_sep->outletConnection()->pipeNumber();

            // checking if it is connected to it self
            if(pipe_num == p_sep->number())
            {
                QString message = QString("Separator %1 is connected to itself.").arg(pipe_num);
                emitException(ExceptionSeverity::ERROR, ExceptionType::MODEL_VALIDATION, message);
            }

            // looping through the pipes to find the correct one
            for(int j = 0; j < numberOfPipes(); j++)
            {
                if(pipe_num == pipe(j)->number())
                {
                    p_sep->outletConnection()->setPipe(pipe(j));
                    pipe(j)->addFeedPipe(p_sep);
                    pipe_ok = true;
                    break;
                }

            }

            // checking if the pipe - pipe connection was ok
            if(!pipe_ok)
            {
                QString message = QString("Separator to Pipe connection could not be established.\nSeparator: %1\n Downstream Pipe: %2").arg(pipe_num).arg(pipe(i)->number());
                emitException(ExceptionSeverity::ERROR, ExceptionType::MODEL_VALIDATION, message);
            }
        } // separator
        else if(p_boost != 0) // this is a PressureBooster
        {
            bool pipe_ok = false;
            int pipe_num = p_boost->outletConnection()->pipeNumber();

            // checking if it is connected to it self
            if(pipe_num == p_boost->number())
            {
                QString message = QString("Booster %1 is connected to itself.").arg(pipe_num);
                emitException(ExceptionSeverity::ERROR, ExceptionType::MODEL_VALIDATION, message);
            }

            // looping through the pipes to find the correct one
            for(int j = 0; j < numberOfPipes(); ++j)
            {
                if(pipe_num == pipe(j)->number())
                {
                    p_boost->outletConnection()->setPipe(pipe(j));
                    pipe(j)->addFeedPipe(p_boost);

                    pipe_ok = true;
                    break;
                }

            }

            // checking if the pipe - pipe connection was ok
            if(!pipe_ok)
            {
                QString message = QString("Booster to Pipe connection could not be established.\nBooster: %1\n Downstream Pipe: %2").arg(pipe_num).arg(pipe(i)->number());
                emitException(ExceptionSeverity::ERROR, ExceptionType::MODEL_VALIDATION, message);
            }
        } // booster

    }   // pipe i


    return ok;
}

bool Model::resolveCapacityConnections()
{
    bool ok = true;

    for(int i = 0; i < m_capacities.size(); i++)        // looping through all separators
    {
        Capacity *s = m_capacities.at(i);

        for(int j = 0; j < s->numberOfFeedPipeNumbers(); j++)   // looping through all the pipes specified in the driver file
        {
            int pipe_num = s->feedPipeNumber(j);
            bool pipe_ok = false;

            for(int k = 0; k < m_pipes.size(); k++)     // looping through pipes to find the correct one
            {
                if(m_pipes.at(k)->number() == pipe_num) // this is the correct pipe
                {
                    s->addFeedPipe(m_pipes.at(k));

                    pipe_ok = true;
                    break;
                }
            }

            // checking if the pipe number was found
            if(!pipe_ok)
            {
                QString message = QString("Capacity to Pipe connection could not be established.\nPipe: %1\nCapacity: %2").arg(pipe_num).arg(s->name().toLatin1().constData());
                emitException(ExceptionSeverity::ERROR, ExceptionType::MODEL_VALIDATION, message);
            }

        }
    }

    return ok;
}

bool Model::calculatePipePressures()
{
    bool ok = true;

    // finding the endnodes
    QVector<EndPipe*> end_pipes;
    for(int i = 0; i < numberOfPipes(); i++)
    {
        EndPipe *p = dynamic_cast<EndPipe*>(pipe(i));   // trying to cast as EndPipe
        if(p != 0) end_pipes.push_back(p);              // adding if cast was ok
    }

    // if no endpipes were found, return error
    if(end_pipes.size() == 0)
    {
        QString message = QString("No end nodes found in the pipe system. \nAt least one pipe must have an OUTLETPRESSURE defined.");
        emitException(ExceptionSeverity::WARNING, ExceptionType::MODEL_VALIDATION, message);
    }
    else    // found end pipes, getting on with the calculations...
    {
        // looping through the end nodes
        for(int i = 0; i < end_pipes.size(); i++)
        {
            // calculating the inlet pressures for the end pipes, and the branch of pipes connected
            end_pipes.at(i)->calculateBranchInletPressures();
        }
    }
    return ok;
}

void Model::readPipeFiles()
{
    for(int i = 0; i < numberOfPipes(); i++)
    {
        // this should not be done for separators or boosters

        Separator *sep = dynamic_cast<Separator*>(pipe(i));
        PressureBooster *boost = dynamic_cast<PressureBooster*>(pipe(i));

        if(sep == 0 && boost == 0) pipe(i)->readInputFile();
    }
}

void Model::updateObjectiveValue()
{
    QVector<Stream*> field_rates;

    // finding the end pipes
    QVector<EndPipe*> p_end_pipes;
    for(int i = 0; i < numberOfPipes(); ++i)
    {
        EndPipe *p = dynamic_cast<EndPipe*>(pipe(i));
        if(p != 0) p_end_pipes.push_back(p);
    }

    // adding together the streams from all the end pipes
    for(int i = 0; i < masterSchedule().size(); ++i)
    {
        Stream *s = new Stream();

        // looping through the end pipes
        for(int j = 0; j < p_end_pipes.size(); ++j)
        {
            *s += *p_end_pipes.at(j)->stream(i);
        }
        field_rates.push_back(s);
    }
    // then collecting all the costs
    QVector<Cost*> costs;

    // collecting the cost of the separators:
    for(int i = 0; i < numberOfPipes(); ++i)
    {
        Separator *p_sep = dynamic_cast<Separator*>(pipe(i));
        if(p_sep != 0)
        {
            // updating the remove fraction and capacity in the cost according to the variable values in the separator
            p_sep->cost()->setFraction(p_sep->removeFraction()->value());
            p_sep->cost()->setCapacity(p_sep->removeCapacity()->value());

            // updating the time of the cost according to the variable
            int time_cost = p_sep->installTime()->value();

            if(time_cost <= 0) p_sep->cost()->setTime(0.0);
            else if(time_cost >= m_master_schedule.size()) p_sep->cost()->setTime(m_master_schedule.at(m_master_schedule.size()-1) + 1);
            else p_sep->cost()->setTime(m_master_schedule.at(time_cost-1));

            // adding the cost to the vector
            costs.push_back(p_sep->cost());
        }
    }

    // collecting the cost of the boosters:
    for(int i = 0; i < numberOfPipes(); ++i)
    {
        PressureBooster *p_boost = dynamic_cast<PressureBooster*>(pipe(i));
        if(p_boost != 0)
        {
            // updating the pressure and capacity in the cost according to the variable values in the booster
            p_boost->cost()->setFraction(p_boost->pressureVariable()->value());
            p_boost->cost()->setCapacity(p_boost->capacityVariable()->value());

            // updating the time of the cost according to the variable
            int time_cost = p_boost->installTime()->value();

            if(time_cost <= 0) p_boost->cost()->setTime(0.0);
            else if(time_cost >= m_master_schedule.size()) p_boost->cost()->setTime(m_master_schedule.at(m_master_schedule.size()-1) + 1);
            else p_boost->cost()->setTime(m_master_schedule.at(time_cost-1));

            // adding the cost to the vector
            costs.push_back(p_boost->cost());
        }
    }

    // collecting the installation cost of the wells:
    for(int i = 0; i < numberOfWells(); ++i)
    {
        Well *w = well(i);
        if(w->hasCost())
        {
            if(w->hasInstallTime())
            {
                // updating the time of the cost according to the variable
                int time_cost = w->installTime()->value();

                if(time_cost <= 0) w->cost()->setTime(0.0);
                else if(time_cost >= m_master_schedule.size()) w->cost()->setTime(m_master_schedule.at(m_master_schedule.size()-1) + 1);
                else w->cost()->setTime(m_master_schedule.at(time_cost));
            }
            else
            {
                w->cost()->setTime(0.0);
            }
            // adding the cost to the vector
            costs.push_back(w->cost());
        }
    }
    QVector<Cost*> costs_sorted = sortCosts(costs);
    objective()->calculateValue(field_rates, costs_sorted);
    for(int i = 0; i < field_rates.size(); i++) delete field_rates.at(i);
}

bool Model::updateCommonConstraints()
{
    bool ok = true;
    if(!updateCapacityConstraints()) ok = false;
    if(!updateWellConstaints()) ok = false;
    if(!updatePipeConstraints()) ok = false;
    if(!updateBoosterConstraints()) ok = false;
    if(!updateUserDefinedConstraints()) ok = false;
    return ok;
}

Well *Model::wellById(int comp_id)
{
    for(int i = 0; i < numberOfWells(); ++i)
    {
        if(well(i)->id() == comp_id) return well(i);
    }
    return 0;
}

Well *Model::wellByName(const QString &name)
{
    for(int i = 0; i < numberOfWells(); ++i)
    {
        if(well(i)->name().compare(name) == 0) return well(i);
    }
    return 0;
}

Model &Model::operator=(const Model &rhs)
{
    bool ok = this->numberOfWells() == rhs.numberOfWells() &&
            this->numberOfPipes() == rhs.numberOfPipes() &&
            this->numberOfRealVariables() == rhs.numberOfRealVariables() &&
            this->numberOfBinaryVariables() == rhs.numberOfBinaryVariables() &&
            this->numberOfIntegerVariables() == rhs.numberOfIntegerVariables() &&
            this->numberOfConstraints() == rhs.numberOfConstraints();

    if(this != &rhs && ok)
    {
        // updating wells
        for(int i = 0; i < numberOfWells(); ++i)
        {
            // looping through the streams
            for(int j = 0; j < well(i)->numberOfStreams(); ++j)
            {
                *well(i)->stream(j) = *rhs.well(i)->stream(j);
                //well(i)->stream(j)->setPressure(rhs.well(i)->stream(j)->pressure(true));
            }
        }

        // updating pipes
        for(int i = 0; i < numberOfPipes(); ++i)
        {
            // looping through the streams
            for(int j = 0; j < pipe(i)->numberOfStreams(); ++j)
            {
                *pipe(i)->stream(j) = *rhs.pipe(i)->stream(j);
                //pipe(i)->stream(j)->setPressure(rhs.pipe(i)->stream(j)->pressure(true));
            }
        }
        // updating real variable values
        for(int i = 0; i < numberOfRealVariables(); ++i) realVariables().at(i)->setValue(rhs.realVariableValue(i));

        // updating binary variable values
        for(int i = 0; i < numberOfBinaryVariables(); ++i) binaryVariables().at(i)->setValue(rhs.binaryVariableValue(i));

        // updating integer variable values
        for(int i = 0; i < numberOfIntegerVariables(); ++i) integerVariables().at(i)->setValue(rhs.integerVariableValue(i));

        // updating constraint values
        for(int i = 0; i < numberOfConstraints(); ++i) constraints().at(i)->setValue(rhs.constraintValue(i));

    }
    return *this;
}

void Model::emitException(ExceptionSeverity severity, ExceptionType type, QString message)
{
    ModelHandler* mh = new ModelHandler;
    connect(this, SIGNAL(error(ExceptionSeverity, ExceptionType, QString)),
            mh, SLOT(handleException(ExceptionSeverity, ExceptionType, QString)));
    emit error(severity, type, message);
    disconnect(this, SIGNAL(error(ExceptionSeverity, ExceptionType, QString)),
               mh, SLOT(handleException(ExceptionSeverity, ExceptionType, QString)));
}

