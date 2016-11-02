#ifndef CASE_H
#define CASE_H

#include <QHash>
#include <QUuid>
#include <Eigen/Core>
#include "optimization_exceptions.h"

namespace Optimization {

    class CaseHandler;
    class CaseTransferObject;

/*!
 * \brief The Case class represents a specific case for the optimizer, i.e. a specific set of variable values
 * and the value of the objective function after evaluation.
 */
    class Case
    {
    public:
        friend class CaseHandler;
        friend class CaseTransferObject;

        Case();
        Case(const QHash<QUuid, bool> &binary_variables, const QHash<QUuid, int> &integer_variables, const QHash<QUuid, double> &real_variables);
        Case(const Case &c) = delete;
        Case(const Case *c);

        /*!
         * \brief Equals Checks whether this case is equal to another case within some tolerance.
         * \param other Case to compare with.
         * \param tolerance The allowed deviation between two cases.
         * \return True if the cases are equal within the tolerance, otherwise false.
         */
        bool Equals(const Case *other, double tolerance=0.0) const;

        QUuid id() const { return id_; }
        std::string id_stdstr() { return id_.toString().toStdString(); } //!< Get an std string representation of the case uuid.

        QHash<QUuid, bool> binary_variables() const { return binary_variables_; }
        QHash<QUuid, int> integer_variables() const { return integer_variables_; }
        QHash<QUuid, double> real_variables() const { return real_variables_; }
        void set_binary_variables(const QHash<QUuid, bool> &binary_variables) { binary_variables_ = binary_variables; }
        void set_integer_variables(const QHash<QUuid, int> &integer_variables) { integer_variables_ = integer_variables; }
        void set_real_variables(const QHash<QUuid, double> &real_variables) { real_variables_ = real_variables; }

        double objective_function_value() const; //!< Get the objective function value. Throws an exception if the value has not been defined.
        void set_objective_function_value(double objective_function_value) { objective_function_value_ = objective_function_value; }

        void set_integer_variable_value(const QUuid id, const int val); //!< Set the value of an integer variable in the case.
        void set_binary_variable_value(const QUuid id, const bool val); //!< Set the value of a boolean variable in the case.
        void set_real_variable_value(const QUuid id, const double val); //!< Set the value of a real variable in the case.

        enum SIGN { PLUS, MINUS, PLUSMINUS};

        /*!
         * \brief Perturb Creates variations of this Case where the value of one variable has been changed.
         *
         * If PLUS or MINUS is selected as the sign, _one_ case is returned. If PLUSMINUS is selected, _two_
         * cases are returned.
         *
         * Note that this method only handles integer and real variables.
         * \param variabe_id The UUID of the variable to be perturbed.
         * \param sign The sign/direction of the perturbation.
         * \param magnitude The magnitude of the perturbaton.
         * \return One or two cases where one variable has been perturbed.
         */
        QList<Case *> Perturb(QUuid variabe_id, SIGN sign, double magnitude);

        QString StringRepresentation();

        /*!
         * Get the real variables of this case as a Vector.
         *
         * @note This function will not work with Case objects created from CaseTransferObject.
         * This implies that, when running in parallel, it will only work on the main process.
         *
         * This creates an ordering of the variables so that for future
         * use the i'th index in the vector will always correspond to the
         * same variable.
         * @return Values of the real variables in a vector
         */
        Eigen::VectorXd GetRealVarVector();

        /*!
         * Sets the real variable values of this case from a given vector.
         *
         * @note This function will not work with Case objects created from CaseTransferObject.
         * This implies that, when running in parallel, it will only work on the main process.
         *
         * The order of the variables as they appear in vector this case is preserved
         * given that they were taken from this same case from the function GetRealVector()
         * @param vec
         */
        void SetRealVarValues(Eigen::VectorXd vec);

        /*!
         * Get the integer variables of this case as a Vector.
         *
         * @note This function will not work with Case objects created from CaseTransferObject.
         * This implies that, when running in parallel, it will only work on the main process.
         *
         * This creates an ordering of the variables so that for future
         * use the i'th index in the vector will always correspond to the
         * same variable.
         * @return Values of the integer variables in a vector
         */
        Eigen::VectorXi GetIntegerVarVector();

        /*!
         * Sets the integer variable values of this case from a given vector.
         *
         * @note This function will not work with Case objects created from CaseTransferObject.
         * This implies that, when running in parallel, it will only work on the main process.
         *
         * The order of the variables as they appear in vector this case is preserved
         * given that they were taken from this same case from the function GetIntegerVarVector()
         * @param vec
         */
        void SetIntegerVarValues(Eigen::VectorXi vec);



    private:
        QUuid id_; //!< Unique ID for the case.

        double objective_function_value_;
        QHash<QUuid, bool> binary_variables_;
        QHash<QUuid, int> integer_variables_;
        QHash<QUuid, double> real_variables_;

        QList<QUuid> real_id_index_map_;
        QList<QUuid> integer_id_index_map_;

    };

}

#endif // CASE_H
