#include <boost/lexical_cast.hpp>
#include "case_transfer_object.h"
#include <QString>

namespace Optimization {
    using namespace boost::uuids;
    using namespace std;


    CaseTransferObject::CaseTransferObject(Optimization::Case *c) {
        id_ = qUuidToBoostUuid(c->id_);
        objective_function_value_ = c->objective_function_value_;
        binary_variables_ = qHashToStdMap(c->binary_variables_);
        integer_variables_ = qHashToStdMap(c->integer_variables_);
        real_variables_ = qHashToStdMap(c->real_variables_);
    }

    Case *CaseTransferObject::CreateCase() {
        auto c = new Case();
        c->binary_variables_ = stdMapToQhash(binary_variables_);
        c->integer_variables_ = stdMapToQhash(integer_variables_);
        c->real_variables_ = stdMapToQhash(real_variables_);
        c->id_ = boostUuidToQuuid(id_);
        c->objective_function_value_ = objective_function_value_;
        return c;
    }

    QUuid CaseTransferObject::boostUuidToQuuid(const uuid buuid) const {
        return QUuid(boostUuidToQstring(buuid));
    }

    uuid CaseTransferObject::qUuidToBoostUuid(const QUuid quuid) const {
        uuid uuid(string_generator()(quuid.toString().toStdString()));
        return uuid;
    }

    QString CaseTransferObject::boostUuidToQstring(const uuid buuid) const {
        auto uuid_string = boost::lexical_cast<string>(buuid);
        auto uuid_qstring = QString(uuid_string.c_str());
        uuid_qstring.append("}");
        uuid_qstring.prepend("{");
        return uuid_qstring;
    }

    template<typename T>
    std::map<uuid, T> CaseTransferObject::qHashToStdMap(const QHash<QUuid, T> &qhash) const
    {
        std::map<uuid, T> stdmap = std::map<uuid, T>();
        for (auto key : qhash.keys()) {
            stdmap[qUuidToBoostUuid(key)] = qhash[key];
        }
        return stdmap;
    }

    template<typename T>
    QHash<QUuid, T> CaseTransferObject::stdMapToQhash(const std::map<uuid, T> &stmap) const
    {
        QHash<QUuid, T> qhash = QHash<QUuid, T>();
        for (auto const &ent : stmap) {
            qhash[boostUuidToQuuid(ent.first)] = ent.second;
        }
        return qhash;
    }

}
