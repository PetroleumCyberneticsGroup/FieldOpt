#include "json_summary_reader.h"
#include <stdexcept>
#include <iostream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QList>
#include <stdexcept>
#include <cmath>

namespace AdgprsResultsReader {

JsonSummaryReader::JsonSummaryReader(QString file_path)
{
    QFile *file = new QFile(file_path);
    if (!file->open(QIODevice::ReadOnly))
        throw std::runtime_error("Unable to open JSON summary file " + file_path.toStdString());
    QByteArray data = file->readAll();
    QJsonDocument json = QJsonDocument::fromJson(data);
    if (json.isNull())
        throw std::runtime_error("Unable to parse the JSON summary file " + file_path.toStdString());
    if (!json.isObject())
        throw std::runtime_error("The json summary must be a valid json object " + file_path.toStdString());

    QJsonObject json_summary = QJsonObject(json.object());
    readGlobal(json_summary["Field"].toObject());
    readFieldProps(json_summary["Field"].toObject()["Properties"].toObject());
    readWellProps(json_summary["Wells"].toArray());
    file->close();

}

JsonSummaryReader::~JsonSummaryReader()
{
    time_->clear();
    delete time_;
    foreach (auto v, field_properties_.values()) {
        delete v;
    }
    field_properties_.clear();
    foreach (auto w, wells_) {
        delete w;
    }
    wells_.clear();

}

QList<QString> JsonSummaryReader::GetAvalableFieldPropertyNames() const
{
    return field_properties_.keys();
}

QVector<double> *JsonSummaryReader::GetFieldProperty(QString prop_name) const
{
    if (QString::compare(prop_name, "TIME") == 0)
        return time_;
    if (!field_properties_.contains(prop_name))
        throw std::runtime_error("Field property " + prop_name.toStdString() + " not recognized.");
    return field_properties_[prop_name];
}

QList<QString> JsonSummaryReader::GetAvailableWellPropertyNames() const
{
    return getWell(0)->properties_.keys();
}

QVector<double> *JsonSummaryReader::GetWellProperty(int well, QString prop_name) const
{
    if (!getWell(well)->properties_.contains(prop_name))
        throw std::runtime_error("Well property " + prop_name.toStdString() + " not recognized.");
    return getWell(well)->properties_[prop_name];
}

bool JsonSummaryReader::IsInjector(int well) const
{
    return getWell(well)->is_injector;
}

int JsonSummaryReader::GetNumberOfPerforations(int well) const
{
    return getWell(well)->num_perforations;
}

JsonSummaryReader::WellData *JsonSummaryReader::getWell(int well) const
{
    if (well < 0 || well >= wells_.size())
        throw std::runtime_error(QString::number(well).toStdString() + " not a valid well number.");
    return wells_[well];
}

void JsonSummaryReader::readGlobal(QJsonObject field)
{
    num_wells_ = field["NumWells"].toInt();

    time_ = new QVector<double>();
    QJsonArray json_time = field["TIME"].toArray();
    foreach (QJsonValue t, json_time) {
        time_->append(t.toDouble());
    }
}

void JsonSummaryReader::readFieldProps(QJsonObject field_props)
{
    field_properties_ = QHash<QString, QVector<double> *>();
    QList<QString> keys = {"FGPT", "FOPT", "FWPT", "FOPR", "FGPR", "FWPR",
                           "FGIT", "FGIR", "FOIT", "FOIR", "FWIT", "FWIR"};

    foreach (QString key, keys) {
        field_properties_[key] = new QVector<double>();
        foreach (QJsonValue v, field_props[key].toArray()) {
            field_properties_[key]->append(std::abs(v.toDouble()));
        }
    }
}

void JsonSummaryReader::readWellProps(QJsonArray well_props)
{
    foreach (QJsonValue w, well_props) {
        WellData *data = new WellData();
        if (w.toObject()["IsInjector"].toInt() == 1)
            data->is_injector = true;
        else
            data->is_injector = false;

        data->num_perforations = w.toObject()["NumPerforations"].toInt();

        data->properties_ = QHash<QString, QVector<double> *>();
        QList<QString> keys = {"WBHP","WGR","WOR","WWR","WGT","WOT","WWT"};
        foreach (QString key, keys) {
            data->properties_[key] = new QVector<double>();
            foreach (QJsonValue v, w.toObject()["Properties"].toObject()[key].toArray()) {
                data->properties_[key]->append(v.toDouble());
            }
        }
        wells_.append(data);
    }
}


}
