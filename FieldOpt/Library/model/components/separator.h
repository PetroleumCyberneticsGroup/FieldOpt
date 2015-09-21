#ifndef SEPARATOR_H
#define SEPARATOR_H

#include <tr1/memory>
#include "model/components/pipe.h"
#include "model/cost.h"
#include "model/pipeconnection.h"
#include "variables/intvariable.h"
#include "variables/realvariable.h"

using std::tr1::shared_ptr;

class Cost;
class IntVariable;
class RealVariable;
class PipeConnection;

/*!
 * \brief A separator that can remove phases.
 *
 * The separator may be fed by any number of pipes, and feeds one upstream pipe.
 * The total rates going through the separator is calculated from the feed pipes.
 * An installation time is set for the separator.
 * From the installation time and on, any of the phases flowing through the separator may be removed.
 */
class Separator : public Pipe
{
public:
    enum Type {WATER, GAS};

private:
    Type m_type;
    Cost *p_cost;                               //!< The cost associated with installing the separator
    shared_ptr<IntVariable> p_install_time;     //!< The installation time of the separator
    shared_ptr<RealVariable> p_remove_fraction; //!< The fraction of water that should be removed from the separator
    shared_ptr<RealVariable> p_remove_capacity; //!< The maximum removal capacity of water
    PipeConnection *p_outlet_connection;        //!< The outlet pipe that this separator feeds.
    QVector<double> m_remaining_capacity;       //!< remaining capacity for each time step

public:
    Separator();                    //!< Default constructor. Sets the Type to WATER, and p_cost and p_outlet_connection to 0.
    Separator(const Separator &s);  //!< Copy constructor.
    virtual ~Separator();           //!< Default destructor. Deletes the cost and outlet connection objects.

    virtual Pipe* clone() {return new Separator(*this);}       //!< Get a copy of this separator.
    virtual void initialize(const QVector<double> &schedule);  //!< Initializes the object by settings the schedule and populating the m_remaining_capacity vector.
    virtual void emptyStreams();                               //!< Calls the emptyStreams() function in the Pipe class and resets the m_remaining_capacity vector.
    void reduceRemainingCapacity(int i, double q);             //!< Reduces the remaining capacity by q for time step i.
    virtual void calculateInletPressure();                     //!< Calculate the inlet pressure of the separator.
    virtual QString description() const;                       //!< Generate a description for the driver file.

    void setType(Type t) {m_type = t;}                                           //!< Set
    void setOutletConnection(PipeConnection *c) {p_outlet_connection = c;}       //!< Set
    void setCost(Cost *c) {p_cost = c;}                                          //!< Set
    void setInstallTime(shared_ptr<IntVariable> t) {p_install_time = t;}         //!< Set
    void setRemoveFraction(shared_ptr<RealVariable> f) {p_remove_fraction = f;}  //!< Set
    void setRemoveCapacity(shared_ptr<RealVariable> c) {p_remove_capacity = c;}  //!< Set

    Type type() const {return m_type;}                                     //!< Get
    PipeConnection* outletConnection() {return p_outlet_connection;}       //!< Get
    Cost* cost() const {return p_cost;}                                    //!< Get
    shared_ptr<IntVariable> installTime() {return p_install_time;}         //!< Get
    shared_ptr<RealVariable> removeFraction() {return p_remove_fraction;}  //!< Get
    shared_ptr<RealVariable> removeCapacity() {return p_remove_capacity;}  //!< Get
    double remainingCapacity(int i) {return m_remaining_capacity.at(i);}   //!< Get
};

#endif // SEPARATOR_H
