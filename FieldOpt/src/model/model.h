#ifndef MODEL_H
#define MODEL_H

#include "components/well.h"
#include "reservoir.h"
#include "objectives/objective.h"
#include "components/pipe.h"
#include "constraints/capacity.h"
#include "variables/intvariable.h"
#include "variables/realvariable.h"
#include "variables/binaryvariable.h"
#include "constraints/constraint.h"
#include "components/component.h"
#include "constraints/constraint.h"
#include "cost.h"

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


/*!
 * \brief Abstract base class for models.
 *
 * The Model stores all the sub-parts of the problem, e.g. reseroiv, wells, pipes, separators, etc.
 */
class Model
{
private:
    Reservoir *p_reservoir; //!< Reservoir defined for the model. \todo This is marked as todo in the ResOpt code.
    QVector<Well*> m_wells; //!< Wells in the model. \todo This is marked as todo in the ResOpt code.
    QVector<Pipe*> m_pipes;  //!< Pipes in the model.
    QVector<Capacity*> m_capacities;  //!< Capacities for the pipes in the model.
    Objective *p_obj;  //!< The objective function for optimization.
    QVector<double> m_master_schedule;  //!< The master schedule for this model.
    QVector<UserConstraint*> m_user_constraints;  //!< User defined constraints for this model.
    bool m_up_to_date;      //!< True if the model has been evaluated
    QString m_driver_path;  //!< The path to the driver file.

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

    QVector<Cost*> sortCosts(QVector<Cost*> c);
public:
    Model();
    ~Model();
};

#endif // MODEL_H
