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
#ifndef CASE_H
#define CASE_H

// ---------------------------------------------------------
// Qt/Eigen
#include <QHash>
#include <QUuid>
#include <QtCore/QDateTime>
#include <Eigen/Core>

// ---------------------------------------------------------
#include <Model/properties/variable_property_container.h>
#include "Runner/loggable.hpp"
#include "optimization_exceptions.h"

// ---------------------------------------------------------
namespace Optimization {

// ---------------------------------------------------------
class CaseHandler;
class CaseTransferObject;

// =========================================================
/*!
 * \brief The Case class represents a specific case for the
 * optimizer, i.e. a specific set of variable values and the
 * value of the objective function after evaluation.
 */
class Case : public Loggable
{
  // -------------------------------------------------------
 public:
  friend class CaseHandler;
  friend class CaseTransferObject;

  // -------------------------------------------------------
  Case();
  Case(const Model::Properties::VariablePropertyContainer* v);
  Case(const QHash<QUuid, bool> &binary_variables,
       const QHash<QUuid, int> &integer_variables,
       const QHash<QUuid, double> &real_variables);
  Case(const Case &c) = delete;
  Case(const Case *c);

  // -------------------------------------------------------
  /*!
   * @brief The CaseState struct holds information about the
   * current status of the Case object, such as whether or
   * not it has been evaluated and whether or not it has been
   * modified by a constriant.
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
      ERR_SIM=-4, ERR_WIC=-3,
      ERR_CONS=-2, ERR_UNKNOWN=-1,
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

  // -------------------------------------------------------
  CaseState state; //!< State of Case, directly modifiable.

  /*!
   * \brief Equals Checks whether this case is
   * equal to another case within some tolerance.
   *
   * \param other Case to compare with.
   *
   * \param tolerance The allowed deviation
   * between two cases.
   *
   * \return True if cases are equal within
   * given tolerance, otherwise false.
   */
  bool Equals(const Case *other,
              double tolerance=0.0) const;

  QUuid id() const { return id_; }

  // -------------------------------------------------------
  //!< Get an std string representation of the case uuid.
  string id_stdstr() { return id_.toString().toStdString(); }

  // -------------------------------------------------------
  /*!
   * @brief Get a string representation of this
   * case, suitable for console printing.
   *
   * @param varcont Pointer to the variable container.
   * This is needed to get variable names.
   *
   * @return An std string describing the case.
   */
  string StringRepresentation(
      Model::Properties::VariablePropertyContainer *varcont);

  // -------------------------------------------------------
  // Differentiate vars by intrinsic type
  QHash<QUuid, bool> binary_variables() const
  { return binary_variables_; }

  QHash<QUuid, int> integer_variables() const
  { return integer_variables_; }

  QHash<QUuid, double> real_variables() const
  { return real_variables_; }

  QHash<QUuid, string> real_vars_names() const
  { return real_vars_names_; }

  // Differentiate vars by extrinsic type
  QHash<QUuid, double> real_wspline_vars() const
  { return real_wspline_vars_; }

  QHash<QUuid, string> real_wspline_names() const
  { return real_wspline_names_; }

  // -------------------------------------------------------
  void set_binary_variables(
      const QHash<QUuid, bool> &binary_variables)
  { binary_variables_ = binary_variables; }

  void set_integer_variables(
      const QHash<QUuid, int> &integer_variables)
  { integer_variables_ = integer_variables; }

  void set_real_variables(
      const QHash<QUuid, double> &real_variables)
  { real_variables_ = real_variables; }

  void set_real_wspline_vars(
      const QHash<QUuid, double> &real_wspline_vars)
  { real_wspline_vars_ = real_wspline_vars; }

  // -------------------------------------------------------
  /*!
   * \brief Get objective function value. Throws an
   * exception if the value has not been defined.
   */
  double objective_function_value() const;

  void set_objective_function_value(double objective_function_value) {
    objective_function_value_ = objective_function_value;
  }

  //!< Set the value of an integer variable in the case.
  void set_integer_variable_value(const QUuid id,
                                  const int val);

  //!< Set the value of a boolean variable in the case.
  void set_binary_variable_value(const QUuid id,
                                 const bool val);

  //!< Set the value of a real variable in the case.
  void set_real_variable_value(const QUuid id,
                               const double val);

  void set_real_variable_value_nfbck(const QUuid id,
                                     const double val);

  enum SIGN { PLUS, MINUS, PLUSMINUS};

  // -------------------------------------------------------
  /*!
   * \brief Perturb Creates variations of this Case where the value
   * of one variable has been changed.
   *
   * If PLUS or MINUS is selected as the sign, _one_ case is returned.
   * If PLUSMINUS is selected, _two_ cases are returned.
   *
   * Note that this method only handles integer and real variables.
   *
   * \param variable_id The UUID of the variable to be perturbed.
   * \param sign The sign/direction of the perturbation.
   * \param magnitude The magnitude of the perturbaton.
   * \return One or two cases where one variable has been perturbed.
   */
  QList<Case *> Perturb(QUuid variable_id,
                        SIGN sign,
                        double magnitude);

  // -------------------------------------------------------
  /*!
   * Get the real well placement variables of this case as
   * a Vector.
   *
   * @return Values of the real well placement variables.
   */
  Eigen::VectorXd GetRealWSplineVarVector();

  // -------------------------------------------------------
  /*!
   * @brief Get a vector containing the variable UUIDs
   * in the same order they appear in the vector from
   * GetRealWellPlcVarVector.
   */
  QList<QUuid> GetRealWSplineVarIdVector()
  { return real_wspline_id_index_map_; }

  // -------------------------------------------------------
  /*!
   * Get the real variables of this case as a Vector.
   *
   * @note This function will not work with Case objects created from
   * CaseTransferObject. This implies that, when running in parallel,
   * it will only work on the main process.
   *
   * This creates an ordering of the variables so that for future
   * use the i'th index in the vector will always correspond to the
   * same variable.
   *
   * @return Values of the real variables in a vector
   */
  Eigen::VectorXd GetRealVarVector();

  Eigen::VectorXd GetRealVarVectorNfbck();

  // -------------------------------------------------------
  /*!
   * Sets the real variable values of this case from a given vector.
   *
   * @note This function will not work with Case objects created from
   * CaseTransferObject. This implies that, when running in parallel,
   * it will only work on the main process.
   *
   * The order of the variables as they appear in vector this case is
   * preserved given that they were taken from this same case from the
   * function GetRealVector()
   *
   * @param vec
   */
  void SetRealVarValues(Eigen::VectorXd vec);
  void SetRealVarValuesNfbck(Eigen::VectorXd vec);

  // -------------------------------------------------------
  /*!
   * @brief Get a vector containing the variable UUIDs
   * in the same order they appear in the vector from
   * GetRealVarVector.
   */
  QList<QUuid> GetRealVarIdVector()
  { return real_id_index_map_; }

  // -------------------------------------------------------
  /*!
   * Get the integer variables of this case as a Vector.
   *
   * @note This function will not work with Case objects
   * created from CaseTransferObject. This implies that,
   * when running in parallel, it will only work on the
   * main process.
   *
   * This creates an ordering of the variables so that
   * for future use the i'th index in the vector will
   * always correspond to the same variable.
   *
   * @return Values of the integer variables in a vector
   */
  Eigen::VectorXi GetIntegerVarVector();

  // -------------------------------------------------------
  /*!
   * Sets the integer variable values
   * of this case from a given vector.
   *
   * @note This function will not work with Case objects
   * created from CaseTransferObject. This implies that,
   * when running in parallel, it will only work on the
   * main process.
   *
   * The order of the variables as they appear in vector
   * this case is preserved given that they were taken
   * from this same case from the function
   * GetIntegerVarVector()
   *
   * @param vec
   */
  void SetIntegerVarValues(Eigen::VectorXi vec);

  // -------------------------------------------------------
  /*!
   * @brief Set the origin info of this Case/trial point,
   * i.e. which point it was generated from, in which
   * direction it was perturbed, and with what magnitude.
   * This method is needed by some optimization algorithms.
   *
   * @param parent The Case/trial
   * point this was generated from.
   *
   * @param direction_index The direction
   * index of the perturbation.
   *
   * @param step_length The magnitude
   * of the perturbation.
   */
  void set_origin_data(Case* parent,
                       int direction_index,
                       double step_length);

  // -------------------------------------------------------
  Case* origin_case() const { return parent_; }
  int origin_direction_index() const { return direction_index_; }
  double origin_step_length() const { return step_length_; }

  // -------------------------------------------------------
  void SetSimTime(const int sec) { sim_time_sec_ = sec; }
  int GetSimTime() const { return sim_time_sec_; }

  // -------------------------------------------------------
  // Logger interface
  LogTarget GetLogTarget() override;
  map<string, string> GetState() override;
  QUuid GetId() override;
  map<string, vector<double>> GetValues() override;
  // End Logger interface

  // -------------------------------------------------------
  /*!
   * @brief Set the time spent computing t
   * he well blocs for this case.
   * @param secs The number of seconds it took.
   */
  void SetWICTime(const int secs) { wic_time_sec_ = secs; }

  // -------------------------------------------------------
  /*!
   * @brief Get the number of seconds spent
   * computing the well blocks for this case.
   */
  int GetWICTime() const { return wic_time_sec_; }

  map<string, double> GetRealVarNameMap();

  map<string, double> GetSplineVarNameMap();

  map<string, QUuid> GetUUIDNameMap();

  void set_case_num(int nc) { num_ = nc; };
  int get_case_num() { return num_; };

  void UpdateWSplineVarValues();
  QHash<QUuid, string> GetRealVarNames();

 private:
  // -------------------------------------------------------
  QUuid id_; //!< Unique ID for the case.
  int num_ = 0;
  int sim_time_sec_;

  // -------------------------------------------------------
  //!< # of seconds spent computing
  //!< the well index for this case.
  int wic_time_sec_;

  // -------------------------------------------------------
  double objective_function_value_;
  QHash<QUuid, bool> binary_variables_;
  QHash<QUuid, int> integer_variables_;

  QHash<QUuid, double> real_variables_;
  QHash<QUuid, string> real_vars_names_;

  QHash<QUuid, double> real_wspline_vars_;
  QHash<QUuid, string> real_wspline_names_;

  // -------------------------------------------------------
  // No feedback variable cases
  QHash<QUuid, double> real_variables_nfbck_;
  QHash<QUuid, double> real_wspline_vars_nfbck_;

  // -------------------------------------------------------
  QList<QUuid> real_id_index_map_;
  QList<QUuid> integer_id_index_map_;
  QList<QUuid> real_wspline_id_index_map_;

  // -------------------------------------------------------
  map<string, double> real_name_map;
  map<string, double> spline_name_map;
  map<string, QUuid> uuid_name_map;

  // -------------------------------------------------------
  //!< The parent of this trial point.
  //!< Needed by the APPS algorithm.
  Case* parent_;

  // -------------------------------------------------------
  //!< The direction index used to
  //!< generate this trial point.
  int direction_index_;

  // -------------------------------------------------------
  //!< The step length used to
  //!< generate this trial point.
  double step_length_;
};

}

#endif // CASE_H
