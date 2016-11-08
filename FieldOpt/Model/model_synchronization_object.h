#ifndef FIELDOPT_MODEL_SYNCHRONIZATION_OBJECT_H
#define FIELDOPT_MODEL_SYNCHRONIZATION_OBJECT_H

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_serialize.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/serialization/map.hpp>
#include <map>
#include <string>

#include "model.h"

using namespace boost::uuids;
using namespace std;

namespace Model {
    class Model;

    /*!
     * @brief The ModelSynchronization class is used by MPI-based runners to synchronize the variable
     * IDs across process instances. This is necessary to make the transferred Case objects work.
     */
    class ModelSynchronizationObject {
        friend class boost::serialization::access;
        template<class Archive> void serialize(Archive & ar, const unsigned int version) {
            ar & binary_variable_ids_;
            ar & discrete_variable_ids_;
            ar & continous_variable_ids_;
        }

    public:
        ModelSynchronizationObject() {}
        ModelSynchronizationObject(Model *model);

        QHash<QString, QUuid> GetDiscreteVariableMap();
        QHash<QString, QUuid> GetContinousVariableMap();
        QHash<QString, QUuid> GetBinaryVariableMap();
        void UpdateVariablePropertyIds(Model *model);

    private:
        std::map<string, uuid> discrete_variable_ids_; //!< Mapping from variable name to variable UUID, for discrete variables.
        std::map<string, uuid> continous_variable_ids_; //!< Mapping from variable name to variable UUID, for continous variables.
        std::map<string, uuid> binary_variable_ids_; //!< Mapping from variable name to variable UUID, for binary variables.

        std::map<string, uuid> createNameToIdMapping(const QHash<QUuid, Properties::DiscreteProperty *> *qhash) const; //!< Create a standard library hash map from a QHash
        std::map<string, uuid> createNameToIdMapping(const QHash<QUuid, Properties::ContinousProperty *> *qhash) const; //!< Create a standard library hash map from a QHash
        std::map<string, uuid> createNameToIdMapping(const QHash<QUuid, Properties::BinaryProperty *> *qhash) const; //!< Create a standard library hash map from a QHash
        QHash<QString, QUuid> convertToQtMapping(const std::map<string, uuid> map); //!< Convert a std/boost based mapping to a Qt based mapping to be used by the rest of the model.

        QUuid boostUuidToQuuid(const uuid buuid) const; //!< Create a QUuid from a boost uuid
        uuid qUuidToBoostUuid(const QUuid quuid) const; //!< Create a boost uuid from a QUuid
        QString boostUuidToQstring(const uuid buuid) const; //!< Get the string representation of a boost uuid
    };

}


#endif //FIELDOPT_MODEL_SYNCHRONIZATION_OBJECT_H
