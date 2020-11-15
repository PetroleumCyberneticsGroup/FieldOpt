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

#include "model.h"
#include <boost/lexical_cast.hpp>
#include "Utilities/verbosity.h"
#include "Utilities/printer.hpp"

namespace Model {

Model::Model(Settings::Settings settings, Logger *logger)
{
    if (settings.paths().IsSet(Paths::GRID_FILE)) {
        grid_ = new Reservoir::Grid::ECLGrid(settings.paths().GetPath(Paths::GRID_FILE));
        wic_ = new Reservoir::WellIndexCalculation::wicalc_rixx(grid_);
    }
    else {
        grid_ = 0;
        wic_ = nullptr;
    }
    current_case_ = nullptr;

    variable_container_ = new Properties::VariablePropertyContainer();

    wells_ = new QList<Wells::Well *>();
    for (int well_nr = 0; well_nr < settings.model()->wells().size(); ++well_nr) {
        wells_->append(new Wells::Well(*settings.model(), well_nr, variable_container_, grid_, wic_));
    }

    variable_container_->CheckVariableNameUniqueness();

    if (settings.optimizer()->restart()) {
        UpdateModel(settings);
    }

    logger_ = logger;
    logger_->AddEntry(new Summary(this));
}

void Model::Finalize() {
    if (current_case_->GetRealizationOFVMap().size() > 0) {
        realization_ofv_map_ = current_case_->GetRealizationOFVMap();
        ensemble_avg_ofv_ = current_case_->GetEnsembleExpectedOfv().first;
        ensemble_ofv_st_dev_ = current_case_->GetEnsembleExpectedOfv().second;
    }
    logger_->AddEntry(this); // Removing this causes the last case to not be in the JSON file
    logger_->AddEntry(new Summary(this));
}

void Model::ApplyCase(Optimization::Case *c)
{

    // Notify the logger to log previous case.
    if (current_case_ != nullptr && current_case_->state.eval != Optimization::Case::CaseState::EvalStatus::E_PENDING) {
        if (current_case_->GetRealizationOFVMap().size() > 0) {
            realization_ofv_map_ = current_case_->GetRealizationOFVMap();
            ensemble_avg_ofv_ = current_case_->GetEnsembleExpectedOfv().first;
            ensemble_ofv_st_dev_ = current_case_->GetEnsembleExpectedOfv().second;
        }
        logger_->AddEntry(this);
    }

    for (QUuid key : c->binary_variables().keys()) {
        variable_container_->SetBinaryVariableValue(key, c->binary_variables()[key]);
    }
    for (QUuid key : c->integer_variables().keys()) {
        variable_container_->SetDiscreteVariableValue(key, c->integer_variables()[key]);
    }
    for (QUuid key : c->real_variables().keys()) {
        variable_container_->SetContinousVariableValue(key, c->real_variables()[key]);
    }
    int cumulative_wic_time = 0;
    bool wic_used = false;
    for (Wells::Well *w : *wells_) {
        w->Update();
        if (w->trajectory()->GetDefinitionType() == Settings::Model::WellDefinitionType::WellSpline) {
            cumulative_wic_time += w->GetTimeSpentInWIC();
            wic_used = true;
        }
    }
    if (wic_used) {
        c->SetWICTime(cumulative_wic_time);
    }
    else {
        c->SetWICTime(0);
    }
    verify();

    current_case_id_ = c->id();
    current_case_ = c;
//    results_.clear();
}

void Model::verify()
{
    verifyWells();
}

void Model::verifyWells()
{
    for (Wells::Well *well : *wells_) {
        verifyWellTrajectory(well);
        if (well->IsSegmented()) {
            verifyWellCompartments(well);
        }
    }
}

Model::Economy* Model::wellCostConstructor(){
    return &well_economy_;
}



void Model::wellCost(Settings::Optimizer *settings) {
  if (settings->objective().use_well_cost) {
    well_economy_.cost = settings->objective().wellCost;
    well_economy_.costXY = settings->objective().wellCostXY;
    well_economy_.costZ = settings->objective().wellCostZ;
    well_economy_.separate = settings->objective().separatehorizontalandvertical;
    well_economy_.use_well_cost = settings->objective().use_well_cost;
    well_economy_.wells_pointer = *wells_;
    for (auto well : *wells_) {
        auto spline_points = well->trajectory()->GetWellSpline()->GetSplinePoints();
        well_economy_.well_xy[well->name().toStdString()] = 0;
        well_economy_.well_z[well->name().toStdString()] = 0;
        well_economy_.well_lengths[well->name().toStdString()] = 0;
        for(int j = 0; j < spline_points.size()-1; j++){
            double well_length = (spline_points[j+1]->ToEigenVector() - spline_points[j]->ToEigenVector()).norm();
            double well_spline_length_z = abs(spline_points[j+1]->ToEigenVector()[2]-spline_points[j]->ToEigenVector()[2]);
            double well_spline_length_xy =  sqrt(pow((spline_points[j]->ToEigenVector()[1] - spline_points[j+1]->ToEigenVector()[1]), 2) + pow((spline_points[j]->ToEigenVector()[0] - spline_points[j+1]->ToEigenVector()[0]), 2));
            well_economy_.well_xy[well->name().toStdString()] += well_spline_length_xy;
            well_economy_.well_z[well->name().toStdString()] += well_spline_length_z;
            well_economy_.well_lengths[well->name().toStdString()] += well_length;
        }
    }
  }
}


void Model::verifyWellTrajectory(Wells::Well *w)
{
    for (Wells::Wellbore::WellBlock *wb : *w->trajectory()->GetWellBlocks()) {
        verifyWellBlock(wb);
    }
}

void Model::verifyWellBlock(Wells::Wellbore::WellBlock *wb)
{
    if (wb->i() < 1 || wb->i() > grid()->Dimensions().nx ||
        wb->j() < 1 || wb->j() > grid()->Dimensions().ny ||
        wb->k() < 1 || wb->k() > grid()->Dimensions().nz)
        throw std::runtime_error("Invalid well block detected: ("
                                     + boost::lexical_cast<std::string>(wb->i()) + ", "
                                     + boost::lexical_cast<std::string>(wb->j()) + ", "
                                     + boost::lexical_cast<std::string>(wb->k()) + ")"
        );
}

void Model::SetResult(const std::string key, std::vector<double> vec) {
    results_[key] = vec;
}
Loggable::LogTarget Model::GetLogTarget() {
    return Loggable::LogTarget::LOG_EXTENDED;
}
map<string, string> Model::GetState() {
    map<string, string> statemap;
    statemap["COMPDAT"] = compdat_.toStdString();
    return statemap;
}
QUuid Model::GetId() {
    return current_case_id_;
}
map<string, vector<double>> Model::GetValues() {
    map<string, vector<double>> valmap;
    for (auto const item : results_) {
        valmap["Res#"+item.first] = item.second;
    }
    for (auto const var : variable_container_->GetContinousVariables()->values()) {
        valmap["Var#"+var->name().toStdString()] = vector<double>{var->value()};
    }
    for (auto const var : variable_container_->GetDiscreteVariables()->values()) {
        valmap["Var#"+var->name().toStdString()] = vector<double>{var->value()};
    }
    for (auto const var : variable_container_->GetBinaryVariables()->values()) {
        valmap["Var#"+var->name().toStdString()] = vector<double>{var->value()};
    }
    if (realization_ofv_map_.keys().size() > 0) {
        for (auto const key : realization_ofv_map_.keys()) {
            valmap["Rea#"+key.toStdString()] = vector<double>{realization_ofv_map_[key]};
        }
        valmap["Rea#OFVStDev"] = vector<double>{ensemble_ofv_st_dev_};
        valmap["Rea#OFVAvg"] = vector<double>{ensemble_avg_ofv_};
    }
    return valmap;
}
void Model::set_grid_path(const std::string &grid_path) {
    if (wic_->HasGrid(grid_path) == false) {
        if (VERB_MOD >= 2) Printer::ext_info("Initializing new Grid: " + grid_path, "Model", "Model");
        grid_ = new Reservoir::Grid::ECLGrid(grid_path);
        wic_->AddGrid(grid_);
        wic_->SetGridActive(grid_);
    }
    else {
        if (VERB_MOD >= 2) Printer::ext_info("Getting existing grid object from WIC: " + grid_path, "Model", "Model");
        grid_ = wic_->GetGrid(grid_path);
        wic_->SetGridActive(grid_);
    }
}
void Model::verifyWellCompartments(Wells::Well *w) {
    double well_length = w->trajectory()->GetLength();
    for (int i = 0; i < w->GetCompartments().size() - 1; ++i) {
        if (w->GetCompartments()[i].icd->md(well_length) != w->GetCompartments()[i].start_packer->md(well_length)) {
            throw std::runtime_error("The ICD MD for compartment "
                + boost::lexical_cast<std::string>(i)
                + "is different from the start-packer MD.");
        }
        if (w->GetCompartments()[i].icd->valveSize() > 7.8540E-3) {
            throw std::runtime_error("A valve cross sectional area is larger than the simulator maximum (7.8540E-3).");
        }
        if (w->GetCompartments()[i].start_packer->md(well_length) > w->GetCompartments()[i+1].end_packer->md(well_length)) {
            throw std::runtime_error("The start-packer MD is greater than the end-packer md in compartment "
                + boost::lexical_cast<string>(i));
        }
    }
    if (w->GetCompartments()[0].start_packer->md(well_length) < 0) {
        throw std::runtime_error("The start-packer MD for the first compartment is negative.");
    }

    double length;
    for (int i = 0; i < w->trajectory()->GetWellSpline()->GetSplinePoints().size() - 1; ++i) {
        length += (w->trajectory()->GetWellSpline()->GetSplinePoints()[i+1]->ToEigenVector()
                    - w->trajectory()->GetWellSpline()->GetSplinePoints()[i]->ToEigenVector()).norm();
    }
    if (w->GetCompartments().back().end_packer->md(well_length) > length) {
        throw std::runtime_error("The end-packer MD for the final compartment is past the end of the well spline. (length: "
                                 + boost::lexical_cast<string>(w->trajectory()->GetLength())
                                 + ", position: "
                                 + boost::lexical_cast<string>(w->GetCompartments().back().end_packer->md(well_length))
                                 + ")"
        );
    }
}

void Model::UpdateModel(Settings::Settings settings) {
    QHash<QUuid, Properties::ContinousProperty *> *continuos_variables = variable_container_->GetContinousVariables();
    QHash<QString, double> restart_base_case_variables = settings.optimizer()->restart_base_case_variables();

    if (continuos_variables->size() != restart_base_case_variables.size()) {
        Printer::error("The base case has fewer/more variables than the RestartBaseCase");
        exit(1);
    }

    QList<QUuid> continuos_variables_keys = continuos_variables->keys();
    QList<QString> restart_base_case_variables_keys = restart_base_case_variables.keys();
    for (int i = 0; i < continuos_variables->size(); ++i) {
        QUuid variable_id = continuos_variables_keys[i];
        QString variable_name = continuos_variables->value(variable_id)->name();
        for (int j = 0; j < restart_base_case_variables.size(); ++j) {
            if (variable_name == restart_base_case_variables_keys[j]) {
                double variable_value = restart_base_case_variables.value(variable_name);
                variable_container_->SetContinousVariableValue(variable_id, variable_value);
                break;
            }

            if (j == restart_base_case_variables.size() - 1) {
                Printer::error("A variable in the base case cannot be found in the RestartBaseCase: " + variable_name.toStdString());
                exit(1);
            }
        }
    }
}

Loggable::LogTarget Model::Summary::GetLogTarget() {
    return LOG_SUMMARY;
}
map<string, string> Model::Summary::GetState() {
    map<string, string> statemap;
    statemap["compdat"] = model_->compdat_.toStdString();
    return statemap;
}
QUuid Model::Summary::GetId() {
    return nullptr;
}
map<string, vector<double>> Model::Summary::GetValues() {
    map<string, vector<double>> valmap;
    return valmap;
}
map<string, Loggable::WellDescription> Model::Summary::GetWellDescriptions() {
    map<string, Loggable::WellDescription> wellmap;

    for (auto well : *model_->wells()) {
        Loggable::WellDescription wdesc;
        wdesc.name = well->name().toStdString();
        wdesc.group = well->group().toStdString();
        wdesc.wellbore_radius = boost::lexical_cast<string>(well->wellbore_radius());
        wdesc.type = well->IsProducer() ? "Producer" : "Injector";

        switch (well->preferred_phase()) {
            case Settings::Model::PreferredPhase::Oil: wdesc.pref_phase = "Oil"; break;
            case Settings::Model::PreferredPhase::Gas: wdesc.pref_phase = "Gas"; break;
            case Settings::Model::PreferredPhase::Water: wdesc.pref_phase = "Water"; break;
            case Settings::Model::PreferredPhase::Liquid: wdesc.pref_phase = "Liquid"; break;
        }

        // Spline
        if (model_->variables()->GetWellSplineVariables(well->name()).size() > 0) {
            wdesc.def_type = "Spline";
            for (auto prop : model_->variables()->GetWellSplineVariables(well->name())) {
                if (prop->propertyInfo().spline_end == Properties::Property::SplineEnd::Heel) {
                    switch (prop->propertyInfo().coord) {
                        case Properties::Property::Coordinate::x: wdesc.spline.heel_x = prop->value(); break;
                        case Properties::Property::Coordinate::y: wdesc.spline.heel_y = prop->value(); break;
                        case Properties::Property::Coordinate::z: wdesc.spline.heel_z = prop->value(); break;
                    }
                }
                else {
                    switch (prop->propertyInfo().coord) {
                        case Properties::Property::Coordinate::x: wdesc.spline.toe_x = prop->value(); break;
                        case Properties::Property::Coordinate::y: wdesc.spline.toe_y = prop->value(); break;
                        case Properties::Property::Coordinate::z: wdesc.spline.toe_z = prop->value(); break;
                    }
                }
            }
        }
        else {
            wdesc.def_type =  "Blocks";
        }

        // Controls
        for (Wells::Control *cont : *well->controls()) {
            Loggable::ControlDescription cd;
            if (cont->mode() == Settings::Model::ControlMode::LRATControl) {
                cd.control = "Rate";
                cd.value = cont->rate();
            }
            else {
                cd.control = "BHP";
                cd.value = cont->bhp();
            }
            cd.state = cont->open() ? "Open" : "Shut";
            cd.time_step = cont->time_step();
            wdesc.controls.push_back(cd);
        }
        wellmap[well->name().toStdString()] = wdesc;
    }

    return wellmap;
}
}
