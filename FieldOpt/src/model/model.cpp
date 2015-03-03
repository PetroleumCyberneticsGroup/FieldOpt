#include "model.h"

Model::Model() :
    p_reservoir(0),
    p_obj(0),
    m_up_to_date(false)
{}

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

void Model::emitException(ExceptionSeverity severity, ExceptionType type, QString message)
{
    ModelHandler* mh = new ModelHandler;
    connect(this, SIGNAL(error(ExceptionSeverity, ExceptionType, QString)),
            mh, SLOT(handleException(ExceptionSeverity, ExceptionType, QString)));
    emit error(severity, type, message);
    disconnect(this, SIGNAL(error(ExceptionSeverity, ExceptionType, QString)),
               mh, SLOT(handleException(ExceptionSeverity, ExceptionType, QString)));
}

