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
#ifndef CASE_H
#define CASE_H

#include <QHash>
#include <QUuid>
#include <Utilities/math.hpp>
#include <Eigen/Core>
#include <QtCore/QDateTime>
#include <Model/properties/variable_property_container.h>
#include "Runner/loggable.hpp"
#include "optimization_exceptions.h"

namespace Optimization {

class CaseHandler;
class CaseTransferObject;

/*!
 * \brief The Case class represents a specific case for the optimizer, i.e. a specific set of variable values
 * and the value of the objective function after evaluation.
 */
class Case : public Loggable
{
 public:
  friend class CaseHandler;
  friend class CaseTransferObject;

  Case();
  Case(const QHash<QUuid, bool> &binary_variables,
       const QHash<QUuid, int> &integer_variables,
       const QHash<QUuid, double> &real_variables);
  Case(const Case &c) = delete;
  Case(const Case *c);

  /*!
   * @brief The CaseState struct holds information about the current
   * status of the Case object, such as whether or not it has been
   * evaluated and whether or not it has been modified by a constriant.
   */
  struct CaseState {
    enum EvalStatus : int {
      E_FAILED=-2, E_TIMEOUT=-1,
      E_PENDING=0,
      E_CURRENT=1, E_DONE=2,
      E_BOOKKEEPED=3
    };
    enum ConsStatus : int {
      C_PROJ_FAILED=-2, C_INFEASIBLE=-1,
      C_PENDING=0,
      C_FEASIBLE=1, C_PROJECTED=2, C_PENALIZED=3,
    };
    enum QueueStatus : int {
      Q_DISCARDED=-1,
      Q_QUEUED=0,
      Q_DEQUEUED=1
    };
    enum ErrorMessage : int {
      ERR_SIM=-4, ERR_WIC=-3, ERR_CONS=-2, ERR_UNKNOWN=-1,
      ERR_OK=0
    };
    CaseState() {
      eval = E_PENDING;
      cons = C_PENDING;
      queue = Q_QUEUED;
      err_msg = ERR_OK;
    }
    EvalStatus eval;
    ConsStatus cons;
    QueueStatus queue;
    ErrorMessage err_msg;
  };

  CaseState state; //!< The state of the Case, directly modifiable.

  /*!
   * \brief Equals Checks whether this case is equal to another case within some tolerance.
   * \param other Case to compare with.
   * \param tolerance The allowed deviation between two cases.
   * \return True if the cases are equal within the tolerance, otherwise false.
   */
  bool Equals(const Case *other, double tolerance=0.0) const;

  QUuid id() const { return id_; }
  string id_stdstr() { return id_.toString().toStdString(); } //!< Get an std string representation of the case uuid.

  /*!
   * @brief Get a string representation of this case, suitable for console printing.
   * @param varcont Pointer to the variable container. This is needed to get variable names.
   * @return An std string describing the case.
   */
  string StringRepresentation(Model::Properties::VariablePropertyContainer *varcont);

  QHash<QUuid, bool> binary_variables() const { return binary_variables_; }
  QHash<QUuid, int> integer_variables() const { return integer_variables_; }
  QHash<QUuid, double> real_variables() const { return real_variables_; }
  void set_binary_variables(const QHash<QUuid, bool> &binary_variables) { binary_variables_ = binary_variables; }
  void set_integer_variables(const QHash<QUuid, int> &integer_variables) { integer_variables_ = integer_variables; }
  void set_real_variables(const QHash<QUuid, double> &real_variables) { real_variables_ = real_variables; }

  double objective_function_value() const; //!< Get the objective function value. Throws an exception if the value has not been defined.
  void set_objective_function_value(double objective_function_value);
  void set_iteration(int iteration);
  void set_variables_name(Model::Properties::VariablePropertyContainer *varcont);

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

  void SetRealVarVelocity(Eigen::VectorXd vec);

  /*!
   * @brief Get a vector containing the variable UUIDs in the same order they appear
   * in in the vector from GetRealVarVector.
   */
  QList<QUuid> GetRealVarIdVector() { return real_id_index_map_; }

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

  /*!
   * @brief Set the origin info of this Case/trial point, i.e. which point it was generated
   * from, in which direction it was perturbed, and with what magnitude. This method is
   * needed by some optimization algorithms.
   * @param parent The Case/trial point this was generated from.
   * @param direction_index The direction index of the perturbation.
   * @param step_length The magnitude of the perturbation.
   */
  void set_origin_data(Case* parent, int direction_index, double step_length);

  Case* origin_case() const { return parent_; }
  int origin_direction_index() const { return direction_index_; }
  double origin_step_length() const { return step_length_; }

  void SetSimTime(const int sec) { sim_time_sec_ = sec; }
  int GetSimTime() const { return sim_time_sec_; }

  // Logger interface
  LogTarget GetLogTarget() override;
  map<string, string> GetState() override;
  QUuid GetId() override;
  map<string, vector<double>> GetValues() override;
  // End Logger interface

  /*!
   * @brief Set the time spent computing the well blocs for this case.
   * @param secs The number of seconds it took.
   */
  void SetWICTime(const int secs) { wic_time_sec_ = secs; }

  /*!
   * @brief Get the number of seconds spent computing the well blocks for this case.
   */
  int GetWICTime() const { return wic_time_sec_; }

  // Multiple realizations-support
  void SetEnsembleRealization(const QString &alias) { ensemble_realization_ = alias; }
  QString GetEnsembleRealization() const { return ensemble_realization_; }
  void SetRealizationOfv(const QString &alias, const double &ofv);
  bool HasRealizationOfv(const QString &alias);
  double GetRealizationOfv(const QString &alias);
  double GetEnsembleAverageOfv() const;
  /*!
   * Gets Ensemble Expected Objective Function Value (OFV). This includes the average
   * and standard deviation for all the OFVs in the ensemble that was previously run.
   */

  QPair<double, double> GetEnsembleExpectedOfv() const;
  QHash<QString, double> GetRealizationOFVMap() const { return ensemble_ofvs_; }

  QHash<QUuid, string> variables_name() const { return variables_name_; }

  void create_mpso_id_r_CO2(int mpso_nr_of_swarms, const QList<double> &r_CO2_list);

  void set_mpso_id_r_CO2(const QHash<QUuid, double> &mpso_id_r_CO2);

  QHash<QUuid, double> mpso_id_r_CO2() const { return mpso_id_r_CO2_; }

  void set_mpso_id_name(const QHash<QUuid, double> &mpso_id_r_CO2);

  QHash<QUuid, QString> mpso_id_name() const { return mpso_id_name_; }

  void set_mpso_id_ofv(const QHash<QUuid, double> &mpso_id_ofv);

  QHash<QUuid, double> mpso_id_ofv() const { return mpso_id_ofv_; }

  void set_ObjFn_id(const QUuid &ObjFn_id);

 private:
  QUuid id_; //!< Unique ID for the case.
  int sim_time_sec_;
  int wic_time_sec_; //!< The number of seconds spent computing the well index for this case.

  double objective_function_value_;
  int iteration_;
  QHash<QUuid, string> variables_name_;
  QHash<QUuid, bool> binary_variables_;
  QHash<QUuid, int> integer_variables_;
  QHash<QUuid, double> real_variables_;
  QHash<QUuid, double> real_variables_velocity_;

  QList<QUuid> real_id_index_map_;
  QList<QUuid> integer_id_index_map_;

  Case* parent_; //!< The parent of this trial point. Needed by the APPS algorithm.
  int direction_index_; //!< The direction index used to generate this trial point.
  double step_length_; //!< The step length used to generate this trial point.

  // Multiple realizations-support
  QString ensemble_realization_; //!< The realization to evaluate next. Used by workers when in parallel mode.
  QHash<QString, double> ensemble_ofvs_; //!< Map of objective function values from realization alias - value.

  QHash<QUuid, double> mpso_id_r_CO2_; //!< Map of r_CO2 value from objective function id.
  QHash<QUuid, QString> mpso_id_name_; //!< Map of objective function name from objective function id.
  QHash<QUuid, double> mpso_id_ofv_; //!< Map of objective function value from objective function id.
  QUuid ObjFn_id_; //!< This ID indicates that the case belongs to a swarm that optimizes an objective function uniquely marked by the ID.
};

}

#endif // CASE_H
