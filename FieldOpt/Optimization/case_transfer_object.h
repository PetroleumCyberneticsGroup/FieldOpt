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

#ifndef FIELDOPT_CASE_TRANSFER_OBJECT_H
#define FIELDOPT_CASE_TRANSFER_OBJECT_H

#include "case.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_serialize.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/serialization/map.hpp>
#include <map>

using namespace boost::uuids;
using namespace std;

namespace Optimization {

/*!
* \brief The CaseTransferObject class can contain all the data a Case object can contain, but it uses
* Boost and standard library datatypes to enable serialization through the Boost Serialization library.
*
* This class was created in an effort to keep the Case class simple, and to avoid bringing Boost datatypes
* into other parts of the model.
*/
class CaseTransferObject {
  friend class boost::serialization::access;
  template<class Archive> void serialize(Archive & ar, const unsigned int version) {
      ar & id_;
      ar & objective_function_value_;
      ar & binary_variables_;
      ar & integer_variables_;
      ar & real_variables_;
      ar & ensemble_realization_;
      ar & wic_time_secs_;
      ar & sim_time_secs_;
      ar & status_eval_;
      ar & status_cons_;
      ar & status_queue_;
      ar & status_err_msg_;
  }

 public:
  CaseTransferObject() {}

  /*!
   * @brief Create a CaseTransferObject representing a Case object.
   * @param c The case to be represented.
   * @return CaseTransferObject containing the same ids and values as the input Case.
   */
  CaseTransferObject(Case *c);

  /*!
   * @brief Create a Case object corresponding to this CaseTransferObject.
   * @return
   */
  Case *CreateCase();

  // Getters
  uuid id() const { return id_; }
  QString id_string() const { return boostUuidToQstring(id_); }
  string id_stdstr() const { return boostUuidToQstring(id_).toStdString(); }
  double objective_function_value() const { return objective_function_value_; }
  map<uuid, bool> binary_variables() const { return binary_variables_; }
  map<uuid, int> integer_variables() const { return integer_variables_; }
  map<uuid, double> real_variables() const { return real_variables_; }
  int wic_time_secs() { return wic_time_secs_; }
  int sim_time_secs() { return sim_time_secs_; }

  QString ensemble_realization() const { return QString::fromStdString(ensemble_realization_); }
  string  ensemble_realization_stdstr() const { return ensemble_realization_; }

 private:
  uuid id_;
  double objective_function_value_;
  int wic_time_secs_;
  int sim_time_secs_;
  map<uuid, bool> binary_variables_;
  map<uuid, int> integer_variables_;
  map<uuid, double> real_variables_;

  string ensemble_realization_;

  int status_eval_;
  int status_cons_;
  int status_queue_;
  int status_err_msg_;

  QUuid boostUuidToQuuid(const uuid buuid) const; //!< Create a QUuid from a boost::uuid.
  uuid qUuidToBoostUuid(const QUuid quuid) const; //!< Create a boost::uuid from a Quuid.
  QString boostUuidToQstring(const uuid buuid) const; //!< Get the string representing a boost::uuid.

  template<typename T> std::map<uuid, T> qHashToStdMap(const QHash<QUuid, T> &qhash) const; //!< Create a standard library hash map from a QHash
  template<typename T> QHash<QUuid, T> stdMapToQhash(const std::map<uuid, T> &stmap) const; //!< Create a QHash from a standard library hash map

};

}

#endif //FIELDOPT_CASE_TRANSFER_OBJECT_H
