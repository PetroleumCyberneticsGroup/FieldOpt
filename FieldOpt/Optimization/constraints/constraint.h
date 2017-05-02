/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/
#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "Optimization/case.h"
#include "Settings/optimizer.h"
#include "Model/properties/variable_property_container.h"

namespace Optimization {
namespace Constraints {

/*!
 * \brief The Constraint class is the abstract parent class to all other constraint classes. One Constraint
 * object should be created for each defined constraint.
 */
class Constraint
{
 public:
  Constraint();

  /*!
   * \brief CaseSatisfiesConstraint checks whether a case satisfies the constraints for all
   * applicable variables.
   * \param c The case to be checked.
   * \return True if the constraint is satisfied; otherwise false.
   */
  virtual bool CaseSatisfiesConstraint(Case *c) = 0;

  /*!
   * \brief SnapCaseToConstraints Snaps all variable values in the case to the closest value
   * that satisfies the constraint.
   * \param c The case that should have it's variable values snapped.
   */
  virtual void SnapCaseToConstraints(Case *c) = 0;

  void EnableLogging(QString output_directory_path);
  void SetVerbosityLevel(int level);

  /*!
   * @brief Indicates whether the constaint is a bound constraint.
   *
   * This should be overridden and return true for all constraints
   * that are bound constraints _and_ are able to return min and
   * max bound vectors.
   * @return True if the constraint is a bound constraint; otherwise false.
   */
  virtual bool IsBoundConstraint() const { return false; }

  /*!
   * @brief Get a vector defining the lower bounds for the variables.
   *
   * Note that variables that are not specified in a constraint will be
   * left as 0. So, if multiple constraints are used, e.g. if we have
   * multiple wells, the vectors from all constraints should be summed
   * to get the correct vector.
   *
   * @param id_vector Vector containing the UUIDs of the variables
   * in the correct order.
   */
  virtual Eigen::VectorXd GetLowerBounds(QList<QUuid> id_vector) const;

  /*!
   * @brief Get a vector defining the upper bounds for the variables.
   *
   * Note that variables that are not specified in a constraint will be
   * left as 0. So, if multiple constraints are used, e.g. if we have
   * multiple wells, the vectors from all constraints should be summed
   * to get the correct vector.
   *
   * @param id_vector Vector containing the UUIDs of the variables
   * in the correct order.
   */
  virtual Eigen::VectorXd GetUpperBounds(QList<QUuid> id_vector) const;

  /*!
   * @brief Get the name of the constraint. All constraints should override this.
   * @return Name of the constraint.
   */
  virtual string name() { return "NONAME"; }

 protected:
  bool logging_enabled_;
  int verbosity_level_;

 private:
  QString constraint_log_path_; //!< Path to the constraint log path to be written.
};

}
}

#endif // CONSTRAINT_H
