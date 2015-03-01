#ifndef OBJECTIVE_H
#define OBJECTIVE_H

#include <QObject>
#include "exceptionhandler/objectivehandler.h"

/*!
 * \brief Abstract base class for objective functions.
 */
class Objective : public QObject
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
    double m_value; //!< \todo This is marked as todo in the ResOpt code.

public:
        Objective() : m_value(0){}  //!< Default constructor.

        virtual Objective* clone() = 0;  //!< Get a copy of this objective.

        /**
         * @brief Calculates the objective value based on a vector of streams.
         * @details The function calculates and stores the objective value. To retreive the calculated value, use value(). This
         *          function must be implemented in each sub class of Objective to correctly calculate the objective value.
         *
         * @param s
         */
        virtual void calculateValue(QVector<Stream*> s, QVector<Cost*> c) = 0;
        virtual QString description() const = 0; //!< Get a description of the objective.

        void setValue(double v) {m_value = v;} //!< Set the objective value

        double value() const {return m_value;}  //!< Get the objective value.

protected:
    /*!
     * \brief Convenience class used by the component subclasses to emit exceptions.
     * \param severity The severity of the event.
     * \param type The type of exception.
     * \param message The message to be printed.
     */
    void emitException(ExceptionSeverity severity, ExceptionType type, QString message);
};

#endif // OBJECTIVE_H
