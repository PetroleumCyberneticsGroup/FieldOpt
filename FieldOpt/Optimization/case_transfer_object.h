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

    private:
        uuid id_;
        double objective_function_value_;
        map<uuid, bool> binary_variables_;
        map<uuid, int> integer_variables_;
        map<uuid, double> real_variables_;

        QUuid boostUuidToQuuid(const uuid buuid) const; //!< Create a QUuid from a boost::uuid.
        uuid qUuidToBoostUuid(const QUuid quuid) const; //!< Create a boost::uuid from a Quuid.
        QString boostUuidToQstring(const uuid buuid) const; //!< Get the string representing a boost::uuid.

        template<typename T> std::map<uuid, T> qHashToStdMap(const QHash<QUuid, T> &qhash) const; //!< Create a standard library hash map from a QHash
        template<typename T> QHash<QUuid, T> stdMapToQhash(const std::map<uuid, T> &stmap) const; //!< Create a QHash from a standard library hash map

    };

}

#endif //FIELDOPT_CASE_TRANSFER_OBJECT_H
