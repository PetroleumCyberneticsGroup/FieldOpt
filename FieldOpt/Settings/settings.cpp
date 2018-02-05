#include "settings.h"
#include "Runner/runtime_settings.h"
#include "settings_exceptions.h"
#include "Utilities/filehandling.hpp"

#include <QJsonDocument>
#include <iostream>

using std::string;
using std::cout;

namespace Settings {

// Left for backcompatibility with tests, etc -- fix that, then delete
Settings::Settings(QString driver_path,
                   QString output_directory,
                   int verbosity_level) {

    if (!::Utilities::FileHandling::FileExists(driver_path))
        throw FileNotFoundException(driver_path.toStdString());
    driver_path_ = driver_path;
    readDriverFile();

    output_directory_ = output_directory;
    simulator_->output_directory_ = output_directory;
    set_verbosity_level(verbosity_level);
}

Settings::Settings(QString driver_path,
                   QString output_directory,
                   std::vector<int> verb_vector) {

    set_verbosity_vector(verb_vector);
    set_verbosity_level(6); // back-compatibility

    if (!::Utilities::FileHandling::FileExists(driver_path))
        throw FileNotFoundException(driver_path.toStdString());
    driver_path_ = driver_path;
    readDriverFile();

    output_directory_ = output_directory;
    simulator_->output_directory_ = output_directory;
}

QString Settings::GetLogCsvString() const
{
    QStringList header  = QStringList();
    QStringList content = QStringList();
    header  << "name"
            << "maxevals"
            << "initstep"
            << "minstep";
    content << name_
            << QString::number(optimizer_->parameters().max_evaluations)
            << QString::number(optimizer_->parameters().initial_step_length)
            << QString::number(optimizer_->parameters().minimum_step_length);

    return QString("%1\n%2").arg(header.join(",")).arg(content.join(","));
}

void Settings::readDriverFile()
{
    QFile *file = new QFile(driver_path_);
    if (!file->open(QIODevice::ReadOnly))
        throw DriverFileReadException("Unable to open the driver file");

    QByteArray data = file->readAll();

    QJsonDocument json = QJsonDocument::fromJson(data);
    if (json.isNull())
        throw DriverFileJsonParsingException(
            "Unable to parse the input file to JSON.");

    if (!json.isObject())
        throw DriverFileFormatException(
            "Driver file format incorrect. Must be a JSON object.");

    json_driver_ = new QJsonObject(json.object());

    readGlobalSection();
    readSimulatorSection();
    readModelSection();
    readOptimizerSection();

    file->close();
}

void Settings::readGlobalSection()
{
    try {
        QJsonObject global = json_driver_->value("Global").toObject();
        name_ = global["Name"].toString();
        bookkeeper_tolerance_ = global["BookkeeperTolerance"].toDouble();
        if (bookkeeper_tolerance_ < 0.0) {
            throw UnableToParseGlobalSectionException(
                "The bookkeeper tolerance must be a positive number.");
        }
    }
    catch (std::exception const &ex) {
        throw UnableToParseGlobalSectionException(
            "Unable to parse driver file global section: " + string(ex.what()));
    }
}

void Settings::readSimulatorSection()
{
    // Simulator root
    try {
        QJsonObject json_simulator = json_driver_->value("Simulator").toObject();
        simulator_ = new Simulator(json_simulator);
    }
    catch (std::exception const &ex) {
        throw UnableToParseSimulatorSectionException(
            "Unable to parse driver file simulator section: " + string(ex.what()));
    }
}

void Settings::readOptimizerSection()
{
    try {
        QJsonObject optimizer = json_driver_->value("Optimizer").toObject();
        optimizer_ = new Optimizer(optimizer);
    }
    catch (std::exception const &ex) {
        throw UnableToParseOptimizerSectionException(
            "Unable to parse driver file optimizer section: " + string(ex.what()));
    }
    optimizer_->set_verbosity_vector(verb_vector());

    if (optimizer_->verb_vector_[9] > 0) { // idx:9 -> set (Settings)
        string str_out = "[set]Optimizer settings";
        cout << "\n" << BLDON << str_out << AEND << "\n" << std::string(str_out.length(), '=') << endl;
        if (optimizer_->type() == Optimizer::OptimizerType::Compass ||
            optimizer_->type() == Optimizer::OptimizerType::APPS) {
            cout << "MaxEvaluations:-------- " << optimizer_->parameters_.max_evaluations << endl;
            cout << "InitialStepLength:----- " << optimizer_->parameters_.initial_step_length << endl;
            cout << "MinimumStepLength:----- " << optimizer_->parameters_.minimum_step_length << endl;
            cout << "ContractionFactor:----- " << optimizer_->parameters_.contraction_factor << endl;
            cout << "ExpansionFactor:------- " << optimizer_->parameters_.expansion_factor << endl;
            cout << "MaxQueueSize:---------- " << optimizer_->parameters_.expansion_factor << endl;
            cout << "Pattern:--------------- " << optimizer_->parameters_.pattern.toStdString() << endl;
        } else if (optimizer_->type() == Optimizer::OptimizerType::GeneticAlgorithm) {
        }
    }
}

void Settings::readModelSection()
{
    try {
        QJsonObject model = json_driver_->value("Model").toObject();
        model_ = new Model(model);
    }
    catch (std::exception const &ex) {
        throw UnableToParseModelSectionException(
            "Unable to parse model section: " + string(ex.what()));
    }
    model_->set_verbosity_vector(verb_vector());
}

void Settings::set_build_path(const QString &build_path)
{
    if (!Utilities::FileHandling::DirectoryExists(build_path))
        throw std::runtime_error(
            "Attempted to set the build path to a non-existent directory.");
    build_path_ = build_path;
}

}

