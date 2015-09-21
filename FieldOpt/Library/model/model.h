#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QString>
#include <QVector>
#include <tr1/memory>
#include "components/component.h"
#include "components/well.h"
#include "components/productionwell.h"
#include "components/injectionwell.h"
#include "components/pipe.h"
#include "components/pressurebooster.h"
#include "components/endpipe.h"
#include "components/midpipe.h"
#include "reservoir.h"
#include "objectives/objective.h"
#include "constraints/capacity.h"
#include "variables/intvariable.h"
#include "variables/realvariable.h"
#include "variables/binaryvariable.h"
#include "constraints/constraint.h"
#include "constraints/userconstraint.h"
#include "cost.h"
#include "exceptionhandler/modelhandler.h"
#include "settings/runtimesettings.h"

using std::tr1::shared_ptr;

class Well;
class Reservoir;
class Objective;
class Pipe;
class Capacity;
class IntVariable;
class RealVariable;
class BinaryVariable;
class Constraint;
class Component;
class UserConstraint;
class Cost;
class RuntimeSettings;


/*!
 * \brief Abstract base class for models.
 *
 * The Model stores all the sub-parts of the problem, e.g. reseroiv, wells, pipes, separators, etc.
 */
class Model : public QObject
{
    Q_OBJECT

signals:
    /*!
     * \brief Error signal.
     *
     * Emitted when an error occurs.
     *
     * \param severity The severity of the event. May be WARNING or ERROR.
     * \param type The type of exception.
     * \param message The message to be printed in addition to the severity and the type.
     */
    void error(ExceptionSeverity severity, ExceptionType type, const QString message);

private:
    Reservoir *p_reservoir;                       //!< Reservoir defined for the model. \todo This is marked as todo in the ResOpt code.
    QVector<Well*> m_wells;                       //!< Wells in the model. \todo This is marked as todo in the ResOpt code.
    QVector<Pipe*> m_pipes;                       //!< Pipes in the model.
    QVector<Capacity*> m_capacities;              //!< Capacities for the pipes in the model.
    Objective *p_obj;                             //!< The objective function for optimization.
    QVector<double> m_master_schedule;            //!< The master schedule for this model.
    QVector<UserConstraint*> m_user_constraints;  //!< User defined constraints for this model.
    bool m_up_to_date;                            //!< True if the model has been evaluated
    QString m_driver_path;                        //!< The path to the driver file. \todo This really should not be here.
    RuntimeSettings* runtimeSettings;             //!< Settings used by a runner. \todo This really should not be here.

    /*!
     * \brief Update the capacity constraints for all the Capacities in the model.
     * \return bool
     */
    bool updateCapacityConstraints();

    /*!
     * \brief Update the BHP and connection constraints on the Wells.
     *
     * This function calls in turn Capacity::updateConstraints() on all the capacities.
     * \return bool
     */
    bool updateWellConstaints();

    /*!
     * \brief Update the pipe connnection constraints
     *
     * \return bool
     */
    bool updatePipeConstraints();  //!< Update all Pipe constraints.
    bool updateBoosterConstraints(); //!< Update constraints on all the capacities.
    bool updateUserDefinedConstraints();  //!< Updates the user defined constraints.

    QVector<Cost*> sortCosts(QVector<Cost*> c); //!< \todo Sort the costs wrt. time.
public:
    Model();                //!< Default constructor. Sets reservoir and objective to 0, m_up_to_date to false.
    Model(const Model &m);  //!< Copy constructor.
    virtual ~Model();       //!< Default destructor. Deletes connected pipes, wells, constraints and capacities.

    virtual QString description() const = 0;  //!< Create a description for the driver file.

    /*!
     * \brief Calcuate the entire upstream part of the model
     *
     * This function updates the streams in the pipe network, calculates pressures, updates constraint values and objective.
     */
    virtual void process() = 0;

    virtual void updateStreams() = 0;  //!< Update the streams in the pipe network.
    virtual Model* clone() const = 0;  //!< Get a clone of this model.
    virtual QVector<shared_ptr<BinaryVariable> >& binaryVariables(bool force_refresh = false) = 0;  //!< Get a vector containing all binary variables defined within the model. These are all the routing variables for the model.
    virtual QVector<shared_ptr<RealVariable> >& realVariables(bool force_refresh = false) = 0;      //!< Get a vector containing all the real variables defined within the model. These are all the well control variables.
    virtual QVector<shared_ptr<IntVariable> >& integerVariables(bool force_refresh = false) = 0;    //!< Get a vector containing all the integer variables. These are the install time variables for the separators.
    virtual QVector<shared_ptr<Constraint> >& constraints(bool force_refresh = false) = 0;          //!< Get a vector containing all the constraints defined within the model. This includes well BHP constraints and capasity constraints.
    virtual QVector<shared_ptr<RealVariable> > realVariables(Component *c) = 0;                     //!< Get a vector of the real variables associated with the Component c.

    /*!
     * \brief Initialize the model.
     *
     * This function sets up the constraints associated with production wells and separators, and connects the pipe network
     * to the wells and separators. The function must be called before the model is used for anything.
     */
    virtual void initialize() = 0;

    /*!
     * \brief Make sure that the Model is set up propperly.
     *
     * This function should be called before the Model is used for anything. It checks that all the sub parts of the Model
     * are defined, that the master schedule corresponds to the schedule of all the wells, etc.
     */
    bool validate();

    virtual bool updateConstraints() = 0;  //!< Update the value of all constraints in the model.

    virtual int numberOfRealVariables() const = 0;         //!< Get the number of real variables in the model
    virtual int numberOfBinaryVariables() const = 0;       //!< Get the number of binary variables in the model
    virtual int numberOfIntegerVariables() const = 0;      //!< Get the number of integer variables in the model
    virtual int numberOfConstraints() const = 0;           //!< Get the number of constraints in the model.
    virtual double realVariableValue(int i) const = 0;     //!< Get the value of real variable i.
    virtual double binaryVariableValue(int i) const = 0;   //!< Get the value of binary variable i.
    virtual double integerVariableValue(int i) const = 0;  //!< Get the value of ingeger variable i.
    virtual double constraintValue(int i) const = 0;       //!< Get the value of constraint i.

    /*!
     * \brief Checks the current routing of wells and pipes, and connects them correctly.
     *
     * Wells and Pipes may have an OUTLETPIPE defined. This is either a fixed number, or a routing variable.
     * This function tries to resolve the current value of the OUTLETPIPE to a NUMBER assigned to a Pipe.
     *
     * \return true if successful, false if not resolved
     */
    bool resolvePipeRouting();

    /*!
     * \brief Translates the input PIPE numbers given in the driver file to pointers to Pipes.
     *
     * This functions should only be called once, before the optimization starts. The input pipes to a given Capacity are fixed.
     */
    bool resolveCapacityConnections();

    bool calculatePipePressures();  //!< Calculate the pressure drops in all the pipes.

    /*!
     * \brief Reads the pipe pressure drop definition files for all pipes in the model.
     *
     * These are the files speficied with the FILE keyword in the main driver file.
     */
    void readPipeFiles();

    void updateObjectiveValue();     //!< Update the value of the Objective from the current Streams of all the wells in the model.
    bool updateCommonConstraints();  //!< Update the value of the constraints that are common for all model types.

    void setMasterSchedule(const QVector<double> &schedule) {m_master_schedule = schedule;}  //!< Set the master schedule.
    void setReservoir(Reservoir *r) {p_reservoir = r;}                                       //!< Set the Reservoir.
    void setObjective(Objective *o) {p_obj = o;}                                             //!< Set the Objective
    void setUpToDate(bool b) {m_up_to_date = b;}                                             //!< Set whether or not the model is up to date.
    void setDriverPath(const QString &path) {m_driver_path = path;}                          //!< Set the driver file path.

    void addWell(Well *w) {m_wells.push_back(w);}                                          //!< Add a Well to the model.
    void addPipe(Pipe *p) {m_pipes.push_back(p);}                                          //!< Add a Pipe to the model.
    void addCapacity(Capacity *s) {m_capacities.push_back(s);}                             //!< Add a Capacity to the model.
    void addUserDefinedConstraint(UserConstraint *uc) {m_user_constraints.push_back(uc);}  //!< Add a user defined constraint to the model.

    const QString& driverPath() {return m_driver_path;}                              //!< Get the driver file path.
    int numberOfMasterScheduleTimes() const {return m_master_schedule.size();}       //!< Get number of entries in the master schedule.
    double masterScheduleTime(int i) const {return m_master_schedule.at(i);}         //!< Get time for entry i in the master schedule.
    QVector<double>& masterSchedule() {return m_master_schedule;}                    //!< Get the master schedule.
    Reservoir* reservoir() const {return p_reservoir;}                               //!< Get the Reservoir.
    Well* well(int i) const {return m_wells.at(i);}                                  //!< Get well i.
    int numberOfWells() const {return m_wells.size();}                               //!< Get the total number of wells defined in the model.
    Well* wellById(int comp_id);                                                     //!< Get the well with the specified id. Returns 0 if not found.
    Well* wellByName(const QString &name);                                           //!< Get the well with the specified name. Returns 0 if not found.
    Pipe* pipe(int i) const {return m_pipes.at(i);}                                  //!< Get Pipe i.
    int numberOfPipes() const {return m_pipes.size();}                               //!< Get the total number of pipes defined in the model.
    Capacity* capacity(int i) {return m_capacities.at(i);}                           //!< Get Capacity i.
    int numberOfCapacities() const {return m_capacities.size();}                     //!< Get the total number of capacities defined in the model.
    UserConstraint* userDefinedConstraint(int i) {return m_user_constraints.at(i);}  //! Get user constraint i.
    int numberOfUserDefinedConstraints() {return m_user_constraints.size();}         //!< Get the total number of user defined constraints in the model.
    Objective* objective() {return p_obj;}                                           //!< Get the the Objective function.
    bool isUpToDate() const {return m_up_to_date;}                                   //!< Get whether or not the model is up to date.

    Model& operator=(const Model &rhs);  //!< Assignment operator. Sets two models equal to each other (they will have pointers to the same wells, streams, etc.)

    RuntimeSettings *getRuntimeSettings() const;
    void setRuntimeSettings(RuntimeSettings *value);

protected:
    /*!
     * \brief Convenience class used by the component subclasses to emit exceptions.
     * \param severity The severity of the event.
     * \param type The type of exception.
     * \param message The message to be printed.
     */
    void emitException(ExceptionSeverity severity, ExceptionType type, QString message);
};

#endif // MODEL_H
