#include <boost/lexical_cast.hpp>
#include "model_synchronization_object.h"

namespace Model {
    using namespace std;
    using namespace boost::uuids;

    ModelSynchronizationObject::ModelSynchronizationObject(Model *model) {
        discrete_variable_ids_ = createNameToIdMapping(model->variables()->GetDiscreteVariables());
        continous_variable_ids_ = createNameToIdMapping(model->variables()->GetContinousVariables());
        binary_variable_ids_ = createNameToIdMapping(model->variables()->GetBinaryVariables());
    }

    map<string, uuid>
    ModelSynchronizationObject::createNameToIdMapping(const QHash<QUuid, Properties::DiscreteProperty *> *qhash) const {
        auto name_id_map = map<string, uuid>();
        for (auto prop : qhash->values())
            name_id_map[prop->name().toStdString()] = qUuidToBoostUuid(prop->id());
        return name_id_map;
    }

    map<string, uuid>
    ModelSynchronizationObject::createNameToIdMapping(const QHash<QUuid, Properties::ContinousProperty *> *qhash) const
    {
        auto name_id_map = map<string, uuid>();
        for (auto prop : qhash->values())
            name_id_map[prop->name().toStdString()] = qUuidToBoostUuid(prop->id());
        return name_id_map;
    }

    map<string, uuid>
    ModelSynchronizationObject::createNameToIdMapping(const QHash<QUuid, Properties::BinaryProperty *> *qhash) const
    {
        auto name_id_map = map<string, uuid>();
        for (auto prop : qhash->values())
            name_id_map[prop->name().toStdString()] = qUuidToBoostUuid(prop->id());
        return name_id_map;
    }

    QHash<QString, QUuid> ModelSynchronizationObject::convertToQtMapping(const std::map<string, uuid> map) {
        QHash<QString, QUuid> qmap = QHash<QString, QUuid>();
        for (auto const &ent : map) {
            qmap[QString::fromStdString((ent.first))] = boostUuidToQuuid(ent.second);
        }
        return qmap;
    }

    QHash<QString, QUuid> ModelSynchronizationObject::GetDiscreteVariableMap() {
        return convertToQtMapping(discrete_variable_ids_);
    }

    QHash<QString, QUuid> ModelSynchronizationObject::GetContinousVariableMap() {
        return convertToQtMapping(continous_variable_ids_);
    }

    QHash<QString, QUuid> ModelSynchronizationObject::GetBinaryVariableMap() {
        return convertToQtMapping(binary_variable_ids_);
    }

    void ModelSynchronizationObject::UpdateVariablePropertyIds(Model *model) {
        auto vpc = model->variable_container_;

        // Binary variables
        if (vpc->BinaryVariableSize() > 0) {
            for (auto name : GetBinaryVariableMap().keys()) {
                auto prop = vpc->GetBinaryVariable(name);
                QUuid new_uuid = GetBinaryVariableMap()[name];
                QUuid old_uuid = prop->id();
                prop->UpdateId(new_uuid);
                vpc->binary_variables_->insert(new_uuid, prop);
                vpc->binary_variables_->remove(old_uuid);
            }
        }

        // Continous variables
        if (vpc->ContinousVariableSize() > 0) {
            for (auto name : GetContinousVariableMap().keys()) {
                auto prop = vpc->GetContinousVariable(name);
                QUuid new_uuid = GetContinousVariableMap()[name];
                QUuid old_uuid = prop->id();
                prop->UpdateId(new_uuid);
                vpc->continous_variables_->insert(new_uuid, prop);
                vpc->continous_variables_->remove(old_uuid);
            }
        }

        // Discrete variables
        if (vpc->DiscreteVariableSize() > 0) {
            for (auto name : GetDiscreteVariableMap().keys()) {
                auto prop = vpc->GetDiscreteVariable(name);
                QUuid new_uuid = GetDiscreteVariableMap()[name];
                QUuid old_uuid = prop->id();
                prop->UpdateId(new_uuid);
                vpc->discrete_variables_->insert(new_uuid, prop);
                vpc->discrete_variables_->remove(old_uuid);
            }
        }
    }

    QUuid ModelSynchronizationObject::boostUuidToQuuid(const uuid buuid) const {
        return QUuid(boostUuidToQstring(buuid));
    }

    uuid ModelSynchronizationObject::qUuidToBoostUuid(const QUuid quuid) const {
        uuid buuid(string_generator()(quuid.toString().toStdString()));
        return buuid;
    }

    QString ModelSynchronizationObject::boostUuidToQstring(const uuid buuid) const {
        const std::string uuid_string = boost::lexical_cast<std::string>(buuid);
        QString uuid_qstring = QString(uuid_string.c_str());
        uuid_qstring.append("}");
        uuid_qstring.prepend("{");
        return uuid_qstring;
    }


}
