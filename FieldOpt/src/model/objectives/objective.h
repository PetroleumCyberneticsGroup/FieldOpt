#ifndef OBJECTIVE_H
#define OBJECTIVE_H

/*!
 * \brief Abstract base class for objective functions.
 */
class Objective
{
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
};

#endif // OBJECTIVE_H
