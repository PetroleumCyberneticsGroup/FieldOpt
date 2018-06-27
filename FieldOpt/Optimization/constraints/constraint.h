/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it
 and/or modify it under the terms of the GNU General
 Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at
 your option) any later version.

 FieldOpt is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#ifndef CONSTRAINT_H
#define CONSTRAINT_H

// ---------------------------------------------------------
#include "Optimization/normalizer.h"
#include "Optimization/case.h"
#include "Settings/optimizer.h"
#include "Model/properties/variable_property_container.h"
#include <Utilities/debug.hpp>

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// =========================================================
/*!
 * \brief The Constraint class is the abstract parent
 * class to all other constraint classes. One constraint
 * object should be created for each defined constraint.
 */
class Constraint
{
 public:
  Constraint();

  // -------------------------------------------------------
  /*!
   * \brief CaseSatisfiesConstraint checks whether a case
   * satisfies the constraints for all applicable variables.
   *
   * \param c The case to be checked.
   * \return True if constraint is satisfied; otherwise false.
   */
  virtual bool CaseSatisfiesConstraint(Case *c) = 0;

  // -------------------------------------------------------
  /*!
   * \brief SnapCaseToConstraints Snaps all variable values
   * in Case to closest value that satisfies the constraint.
   *
   * \param c The case that should have
   * it's variable values snapped.
   */
  virtual void SnapCaseToConstraints(Case *c) = 0;

  // -------------------------------------------------------
  void EnableLogging(QString output_directory_path);

  // -------------------------------------------------------
  /*!
   * @brief
   */
  void set_verbosity_vector(const std::vector<int> verb_vector)
  { verb_vector_ = verb_vector; }

  // -------------------------------------------------------
  /*!
   * @brief
   */
  std::vector<int> verb_vector() const { return verb_vector_; }

  // -------------------------------------------------------
  /*!
   * @brief Indicates whether the constaint is a bound
   * constraint.
   *
   * This should be overridden and return true for all
   * constraints that are bound constraints _and_ are
   * able to return min and max bound vectors.
   *
   * @return True if the constraint is a bound
   * constraint; otherwise false.
   */
  virtual bool IsBoundConstraint() const { return false; }

  // -------------------------------------------------------
  /*!
   * @brief Get a vector defining the
   * lower bounds for the variables.
   *
   * Note that variables that are not specified in a
   * constraint will be left as 0. So, if multiple
   * constraints are used, e.g. if we have multiple
   * wells, the vectors from all constraints should
   * be summed to get the correct vector.
   *
   * @param id_vector Vector containing the UUIDs
   * of the variables in the correct order.
   */
  virtual Eigen::VectorXd
  GetLowerBounds(QList<QUuid> id_vector) const;

  // -------------------------------------------------------
  /*!
   * @brief Get a vector defining the
   * upper bounds for the variables.
   *
   * Note that variables that are not specified in a
   * constraint will be left as 0. So, if multiple
   * constraints are used, e.g. if we have multiple
   * wells, the vectors from all constraints should
   * be summed to get the correct vector.
   *
   * @param id_vector Vector containing the UUIDs
   * of the variables in the correct order.
   */
  virtual Eigen::VectorXd
  GetUpperBounds(QList<QUuid> id_vector) const;

  // -------------------------------------------------------
  /*!
   * @brief Get the name of the constraint.
   * All constraints should override this.
   * @return Name of the constraint.
   */
  virtual string name() { return "NONAME"; }

  // -------------------------------------------------------
  /*!
   * @brief Initialize normalizer, setting the parameters.
   *
   * This default implementation should be overridden
   * by subclasses. This sets the parameters to
   *  - x_0 = 0.0
   *  - k = 1.0
   *  - L = 1.0
   *
   * @param cases A list of cases to be used for
   * calculating the normalization parameters.
   */
  virtual void InitializeNormalizer(QList<Case *> cases);

  // -------------------------------------------------------
  /*!
   * @brief Get the penalty term for a case.
   *
   * This default implementation should be overridden by
   * subclasses. This default implementation returns 0.0;
   *
   * @param c Case to compute the violation for.
   *
   * @return The penalty term for a case (0.0
   * if it does not violate the constraint).
   */
  virtual double Penalty(Case *c);

  // -------------------------------------------------------
  /*!
   * @brief Get the normalized penalty value for the case.
   *
   * @param c Case to get the penalty for.
   *
   * @return Normalized penalty.
   */
  virtual long double PenaltyNormalized(Case *c);

  // -------------------------------------------------------
  long double GetPenaltyWeight() { return penalty_weight_; }

 protected:

  // -------------------------------------------------------
  bool logging_enabled_;

  // -------------------------------------------------------
  // Normalizer for constraint violation value;
  // to be used with penalty functions.
  Normalizer normalizer_;

  // -------------------------------------------------------
  // The weight to be used when considering the
  // constraint in a penalty function. (default: 0.0)
  long double penalty_weight_;

  // -------------------------------------------------------
  Settings::Optimizer *settings_;

 private:
  // -------------------------------------------------------
  // Path to the constraint log path to be written.
  QString constraint_log_path_;

  // -------------------------------------------------------
  std::vector<int> verb_vector_ = std::vector<int>(11,0);

};

}
}

#endif // CONSTRAINT_H
