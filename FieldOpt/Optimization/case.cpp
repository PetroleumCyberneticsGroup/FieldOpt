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

// ---------------------------------------------------------
// Qt / BOOST
#include <QtCore/QUuid>
#include <boost/serialization/map.hpp>
#include <boost/lexical_cast.hpp>

// ---------------------------------------------------------
// FIELDOPT
#include "case.h"
#include <Utilities/time.hpp>


// ---------------------------------------------------------
namespace Optimization {

// =========================================================
Case::Case(const Model::Properties::VariablePropertyContainer* v) {

  // -------------------------------------------------------
  id_ = QUuid::createUuid();
  objective_function_value_ = std::numeric_limits<double>::max();
  sim_time_sec_ = -1;

  // -------------------------------------------------------
  // Differentiate vars by intrinsic type
  binary_variables_ = v->GetBinaryVariableValues();
  integer_variables_ = v->GetDiscreteVariableValues();
  real_variables_ = v->GetContinuousVariableValues();
  real_vars_names_ = v->GetContinuousVariableNames();

  // Store keys
  integer_id_index_map_ = integer_variables_.keys();
  real_id_index_map_ = real_variables_.keys();

  // -------------------------------------------------------
  // Differentiate vars by extrinsic type
  real_wspline_vars_ = v->GetWellSplineVariableValues(); // <- new
  real_wspline_names_ = v->GetWellSplineVariableNames();

  // Store keys
  real_wspline_id_index_map_ = real_wspline_vars_.keys(); // <- new

  // -------------------------------------------------------
  // Store NFBCK variable vectors
  real_variables_nfbck_ = real_variables_;
  real_wspline_vars_nfbck_ = real_wspline_vars_;

}

// ---------------------------------------------------------
Case::Case() {

  // -------------------------------------------------------
  id_ = QUuid::createUuid();
  objective_function_value_ = std::numeric_limits<double>::max();
  sim_time_sec_ = -1;

  // -------------------------------------------------------
  // Differentiate vars by intrinsic type
  binary_variables_ = QHash<QUuid, bool>();
  integer_variables_ = QHash<QUuid, int>();
  real_variables_ = QHash<QUuid, double>();
  real_vars_names_ = QHash<QUuid, string>();

  // -------------------------------------------------------
  // Differentiate vars by extrinsic type
  // Nullptr by default?
  real_wspline_vars_ = QHash<QUuid, double>();
  real_wspline_names_ = QHash<QUuid, string>();

  // -------------------------------------------------------
  // Set up NFBCK variable vectors
  real_variables_nfbck_ = real_variables_;
  real_wspline_vars_nfbck_ = real_wspline_vars_;

}

// ---------------------------------------------------------
Case::Case(const QHash<QUuid, bool> &binary_variables,
           const QHash<QUuid, int> &integer_variables,
           const QHash<QUuid, double> &real_variables) {

  // -------------------------------------------------------
  id_ = QUuid::createUuid();
  objective_function_value_ = std::numeric_limits<double>::max();
  sim_time_sec_ = -1;

  // -------------------------------------------------------
  // Differentiate vars by intrinsic type
  binary_variables_ = binary_variables;
  integer_variables_ = integer_variables;
  real_variables_ = real_variables;

  // Store keys
  real_id_index_map_ = real_variables_.keys();
  integer_id_index_map_ = integer_variables_.keys();

  // -------------------------------------------------------
  // Differentiate vars by extrinsic type
  // Nullptr by default?
}

// ---------------------------------------------------------
Case::Case(const Case *c) {

  // -------------------------------------------------------
  id_ = QUuid::createUuid();
  objective_function_value_ = c->objective_function_value_;

  // -------------------------------------------------------
  // Differentiate vars by intrinsic type
  binary_variables_ = QHash<QUuid, bool>(c->binary_variables());
  integer_variables_ = QHash<QUuid, int> (c->integer_variables());
  real_variables_ = QHash<QUuid, double> (c->real_variables());
  real_vars_names_ = QHash<QUuid, string> (c->real_vars_names());

  // Store keys
  real_id_index_map_ = real_variables_.keys();
  integer_id_index_map_ = integer_variables_.keys();
  sim_time_sec_ = -1;

  // -------------------------------------------------------
  // Differentiate vars by extrinsic type, i.e., parameter is
  // used to reprensent, i.e., well spline, control, etc.
  real_wspline_vars_ = QHash<QUuid, double> (c->real_wspline_vars());
  real_wspline_names_ = QHash<QUuid, string> (c->real_wspline_names());

  // Store keys
  real_wspline_id_index_map_ = real_wspline_vars_.keys();

  // -------------------------------------------------------
  // Store NFBCK variable vectors
  real_variables_nfbck_ = real_variables_;
  real_wspline_vars_nfbck_ = real_wspline_vars_;

}

// =========================================================
void Case::UpdateWSplineVarValues() {

  QHash<QUuid, double> spline_vars = QHash<QUuid, double>();
  QHash<QUuid, double> spline_vars_nfbck = QHash<QUuid, double>();

  // -------------------------------------------------------
  for (QUuid key : real_wspline_id_index_map_) {
    spline_vars[key] = real_variables_.value(key);
    spline_vars_nfbck[key] = real_variables_nfbck_.value(key);
  }

  real_wspline_vars_ = spline_vars;
  real_wspline_vars_nfbck_ = spline_vars_nfbck;
}

// =========================================================
bool Case::Equals(const Case *other,
                  double tolerance) const {

  // -------------------------------------------------------
  // Check if number of variables are equal
  if (this->binary_variables().size()
      != other->binary_variables().size()
      || this->integer_variables().size()
          != other->integer_variables().size()
      || this->real_variables().size()
          != other->real_variables().size())
    return false;

  // -------------------------------------------------------
  for (QUuid key : this->binary_variables().keys()) {
    if (std::abs(this->binary_variables()[key] - other->binary_variables()[key]) > tolerance)
      return false;
  }

  // -------------------------------------------------------
  for (QUuid key : this->integer_variables().keys()) {
    if (std::abs(this->integer_variables()[key] - other->integer_variables()[key]) > tolerance)
      return false;
  }

  // -------------------------------------------------------
  for (QUuid key : this->real_variables().keys()) {
    if (std::abs(this->real_variables()[key] - other->real_variables()[key]) > tolerance)
      return false;
  }

  return true; // All variable values are equal if we reach this point.
}

// =========================================================
double Case::objective_function_value() const {

  if (objective_function_value_ ==
      std::numeric_limits<double>::max())
    throw ObjectiveFunctionException(
        "The objective function value has not been set in this Case.");
  else
    return objective_function_value_;
}

// =========================================================
void Case::set_integer_variable_value(const QUuid id,
                                      const int val) {

  if (!integer_variables_.contains(id))
    throw VariableException(
        "Unable to set value of variable " + id.toString());
  integer_variables_[id] = val;
}

// =========================================================
void Case::set_binary_variable_value(const QUuid id,
                                     const bool val) {

  if (!binary_variables_.contains(id))
    throw VariableException(
        "Unable to set value of variable " + id.toString());
  binary_variables_[id] = val;
}

// =========================================================
void Case::set_real_variable_value(const QUuid id,
                                   const double val) {

  if (!real_variables_.contains(id))
    throw VariableException(
        "Unable to set value of variable " + id.toString());
  real_variables_[id] = val;
}

// =========================================================
void Case::set_real_variable_value_nfbck(const QUuid id,
                                         const double val) {

  if (!real_variables_nfbck_.contains(id))
    throw VariableException(
        "Unable to set value of variable " + id.toString());
  real_variables_nfbck_[id] = val;
}

// =========================================================
QList<Case *> Case::Perturb(QUuid variable_id,
                            Case::SIGN sign,
                            double magnitude) {

  // -------------------------------------------------------
  QList<Case *> new_cases = QList<Case *>();
  if (this->integer_variables().contains(variable_id)) {


    if (sign == PLUS || sign == PLUSMINUS) {
      Case *new_case_p = new Case(this);
      new_case_p->integer_variables_[variable_id] += magnitude;

      new_case_p->objective_function_value_ =
          std::numeric_limits<double>::max();
      new_cases.append(new_case_p);
    }

    if (sign == MINUS || sign == PLUSMINUS) {
      Case *new_case_m = new Case(this);
      new_case_m->integer_variables_[variable_id] -= magnitude;

      new_case_m->objective_function_value_ =
          std::numeric_limits<double>::max();
      new_cases.append(new_case_m);
    }

    // -----------------------------------------------------
  } else if (real_variables_.contains(variable_id)) {

    // -----------------------------------------------------
    if (sign == PLUS || sign == PLUSMINUS) {
      Case *new_case_p = new Case(this);
      new_case_p->real_variables_[variable_id] += magnitude;

      // ---------------------------------------------------
      new_case_p->objective_function_value_ =
          std::numeric_limits<double>::max();
      new_cases.append(new_case_p);
    }

    // -----------------------------------------------------
    if (sign == MINUS || sign == PLUSMINUS) {
      Case *new_case_m = new Case(this);
      new_case_m->real_variables_[variable_id] -= magnitude;

      // ---------------------------------------------------
      new_case_m->objective_function_value_ =
          std::numeric_limits<double>::max();
      new_cases.append(new_case_m);
    }

  }
  return new_cases;
}

// =========================================================
Eigen::VectorXd Case::GetRealWSplineVarVector() {

  // -------------------------------------------------------
  Eigen::VectorXd vec(real_wspline_id_index_map_.length());
  for (int i = 0; i < real_wspline_id_index_map_.length(); ++i) {
    vec[i] = real_variables_.value(real_wspline_id_index_map_[i]);
  }
  return vec;
}

// =========================================================
Eigen::VectorXd Case::GetRealVarVector() {

  // -------------------------------------------------------
  Eigen::VectorXd vec(real_id_index_map_.length());
  for (int i = 0; i < real_id_index_map_.length(); ++i) {
    vec[i] = real_variables_.value(real_id_index_map_[i]);
  }
  return vec;
}

// =========================================================
Eigen::VectorXd Case::GetRealVarVectorNfbck() {

  // -------------------------------------------------------
  Eigen::VectorXd vec(real_id_index_map_.length());
  for (int i = 0; i < real_id_index_map_.length(); ++i) {
    vec[i] = real_variables_nfbck_.value(real_id_index_map_[i]);
  }
  return vec;
}

// =========================================================
void Case::SetRealVarValues(Eigen::VectorXd vec) {

  for (int i = 0; i < vec.size(); ++i) {
    set_real_variable_value(real_id_index_map_[i], vec[i]);
  }
}

// =========================================================
void Case::SetRealVarValuesNfbck(Eigen::VectorXd vec) {

  for (int i = 0; i < vec.size(); ++i) {
    set_real_variable_value_nfbck(real_id_index_map_[i], vec[i]);
  }
}

// =========================================================
Eigen::VectorXi Case::GetIntegerVarVector() {

  // -------------------------------------------------------
  Eigen::VectorXi vec(integer_id_index_map_.length());
  for (int i = 0; i < integer_id_index_map_.length(); ++i) {
    vec[i] = integer_variables_.value(integer_id_index_map_[i]);
  }
  return vec;
}

// =========================================================
map<string, QUuid> Case::GetUUIDNameMap() {

  // -------------------------------------------------------
  for (int i = 0; i < real_id_index_map_.length(); ++i) {

    auto uuid = real_id_index_map_[i];
    uuid_name_map[real_vars_names_[uuid]] = uuid;

  }
  return uuid_name_map;
}

// =========================================================
QHash<QUuid, string> Case::GetRealVarNames() {
  return real_vars_names_;

};

// =========================================================
map<string, double> Case::GetRealVarNameMap() {

  // -------------------------------------------------------
  for (int i = 0; i < real_id_index_map_.length(); ++i) {

    auto uuid = real_id_index_map_[i];
    real_name_map[real_vars_names_[uuid]] =
        real_variables_[uuid];

  }
  return real_name_map;
}

// =========================================================
map<string, double> Case::GetSplineVarNameMap() {

  // -------------------------------------------------------
  for (int i = 0; i < real_wspline_id_index_map_.length(); ++i) {

    auto uuid = real_wspline_id_index_map_[i];
    spline_name_map[real_wspline_names_[uuid]] =
        real_wspline_vars_[uuid];

  }
  return spline_name_map;
}

// =========================================================
void Case::SetIntegerVarValues(Eigen::VectorXi vec) {

  // -------------------------------------------------------
  for (int i = 0; i < vec.size(); ++i) {
    set_integer_variable_value(integer_id_index_map_[i], vec[i]);
  }
}

// =========================================================
void Case::set_origin_data(Case *parent,
                           int direction_index,
                           double step_length) {

  parent_ = parent;
  direction_index_ = direction_index;
  step_length_ = step_length;
}

// =========================================================
Loggable::LogTarget Case::GetLogTarget() {
  return Loggable::LogTarget::LOG_CASE;
}

// =========================================================
map <string, string> Case::GetState() {

  map<string, string> statemap;

  switch (state.eval) {
    case CaseState::EvalStatus::E_FAILED: statemap["EvalSt"] = "FAIL"; break;
    case CaseState::EvalStatus::E_TIMEOUT: statemap["EvalSt"] = "TMOT"; break;
    case CaseState::EvalStatus::E_PENDING: statemap["EvalSt"] = "PEND"; break;
    case CaseState::EvalStatus::E_CURRENT: statemap["EvalSt"] = "CRNT"; break;
    case CaseState::EvalStatus::E_DONE: statemap["EvalSt"] = "OKAY"; break;
    case CaseState::EvalStatus::E_BOOKKEEPED: statemap["EvalSt"] = "BKPD"; break;
  }

  switch (state.cons) {
    case CaseState::ConsStatus::C_PROJ_FAILED: statemap["ConsSt"] = "PNFL"; break;
    case CaseState::ConsStatus::C_INFEASIBLE: statemap["ConsSt"] = "INFS"; break;
    case CaseState::ConsStatus::C_PENDING: statemap["ConsSt"] = "PEND"; break;
    case CaseState::ConsStatus::C_FEASIBLE: statemap["ConsSt"] = "OKAY"; break;
    case CaseState::ConsStatus::C_PROJECTED: statemap["ConsSt"] = "PROJ"; break;
    case CaseState::ConsStatus::C_PENALIZED: statemap["ConsSt"] = "PNZD"; break;
  }

  switch (state.err_msg) {
    case CaseState::ErrorMessage::ERR_SIM: statemap["ErrMsg"] = "SIML"; break;
    case CaseState::ErrorMessage::ERR_WIC: statemap["ErrMsg"] = "WLIC"; break;
    case CaseState::ErrorMessage::ERR_CONS: statemap["ErrMsg"] = "CONS"; break;
    case CaseState::ErrorMessage::ERR_UNKNOWN: statemap["ErrMsg"] = "UNWN"; break;
    case CaseState::ErrorMessage::ERR_OK: statemap["ErrMsg"] = "OKAY"; break;
  }
  return statemap;
}

// =========================================================
QUuid Case::GetId() {
  return id();
}

// =========================================================
map <string, vector<double>> Case::GetValues() {

  map<string, vector<double>> valmap;
  valmap["OFnVal"] = vector<double>{objective_function_value_};
  valmap["SimDur"] = vector<double>{sim_time_sec_};
  valmap["WicDur"] = vector<double>{wic_time_sec_};
  return valmap;
}

// =========================================================
string Case::StringRepresentation(
    Model::Properties::VariablePropertyContainer *varcont) {

  stringstream str;
  str << "|=========================================================|" << endl;
  str << "| Case:            " << id_stdstr() << " |" << endl;
  str << "|---------------------------------------------------------|" << endl;

  if (real_variables_.size() > 0) {
    str << "| Continuous variable values:                             |" << endl;
    for (auto key : real_variables_.keys()) {
      string varname = varcont->GetContinousVariables()->value(key)->name().toStdString();
      str << "| > " << varname << ": " << std::setw (51 - varname.size())
          << boost::lexical_cast<string>(real_variables_[key]) << " |" << endl;
    }
  }

  if (integer_variables_.size() > 0) {
    str << "| Discrete variable values:                             |" << endl;
    for (auto key : integer_variables_.keys()) {
      string varname = varcont->GetDiscreteVariables()->value(key)->name().toStdString();
      str << "| > " << varname << ": " << std::setw (51 - varname.size())
          << boost::lexical_cast<string>(integer_variables_[key]) << " |" << endl;
    }
  }

  if (binary_variables_.size() > 0) {
    str << "| Discrete variable values:                             |" << endl;
    for (auto key : binary_variables_.keys()) {
      string varname = varcont->GetBinaryVariables()->value(key)->name().toStdString();
      str << "| > " << varname << ": " << std::setw (51 - varname.size())
          << boost::lexical_cast<string>(binary_variables_[key]) << " |" << endl;
    }
  }

  str << "|=========================================================|" << endl;
  return str.str();

//    tringstream entry;
//    entry << setw(cas_log_col_widths_["TimeSt"]) << timestamp_string() << " ,";
//    entry << setw(cas_log_col_widths_["EvalSt"]) << obj->GetState()["EvalSt"] << " ,";
//    entry << setw(cas_log_col_widths_["ConsSt"]) << obj->GetState()["ConsSt"] << " ,";
//    entry << setw(cas_log_col_widths_["ErrMsg"]) << obj->GetState()["ErrMsg"] << " ,";
//    entry << setw(cas_log_col_widths_["SimDur"]) << timespan_string(obj->GetValues()["SimDur"][0]) << " , ";
//    entry << setw(cas_log_col_widths_["WicDur"]) << timespan_string(obj->GetValues()["WicDur"][0]) << " , ";
//    entry.precision(6);
//    entry << setw(cas_log_col_widths_["OFnVal"]) << scientific << obj->GetValues()["OFnVal"][0] << " ,";
//    entry << setw(cas_log_col_widths_["CaseId"]) << obj->GetId().toString().toStdString();
//    string str = entry.str();
//    Utilities::FileHandling::WriteLineToFile(QString::fromStdString(str), cas_log_path_);
}

}
