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

namespace Model {

Model::Model(Settings::Settings settings, Logger *logger)
{
    if (settings.paths().IsSet(Paths::GRID_FILE)) {
        grid_ = new Reservoir::Grid::ECLGrid(settings.paths().GetPath(Paths::GRID_FILE));
    }
    else {
        grid_ = 0;
    }
    wic_ = nullptr;

    variable_container_ = new Properties::VariablePropertyContainer();

    wells_ = new QList<Wells::Well *>();
    for (int well_nr = 0; well_nr < settings.model()->wells().size(); ++well_nr) {
        wells_->append(new Wells::Well(*settings.model(), well_nr, variable_container_, grid_, wic_));
    }

    variable_container_->CheckVariableNameUniqueness();
    logger_ = logger;
    logger_->AddEntry(new Summary(this));
}

void Model::Finalize() {
    logger_->AddEntry(this); // Removing this causes the last case to not be in the JSON file
    logger_->AddEntry(new Summary(this));
}

void Model::ApplyCase(Optimization::Case *c)
{
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

    // Notify the logger, and after that clear the results.
    // First check that we have results (if not, this is the first evaluation,
    // and we have nothing to notify the logger about).
    if (results_.size() > 0 && c->GetEnsembleRealization().length() == 0){
        if (c->GetRealizationOFVMap().count() > 0) {
            realization_ofv_map_ = c->GetRealizationOFVMap();
        }
        logger_->AddEntry(this);
    }
    current_case_id_ = c->id();
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

Model::economy Model::wellCost(Settings::Optimizer::Objective objective) {
  economy wellEconomy;
  if (objective.useWellCost) {
    wellEconomy.n_wells = wells_->size();
    wellEconomy.cost = objective.wellCost;
    wellEconomy.costXY = objective.wellCostXY;
    wellEconomy.costZ = objective.wellCostZ;
    wellEconomy.separate = objective.separatehorizontalandvertical;
    wellEconomy.useWellCost = objective.useWellCost;
    wellEconomy.wells_ = *wells_;
    for (Wells::Well *well : *wells_) {
      auto variable = well->trajectory()->GetWellSpline()->GetSplinePoints();
      auto well_spline_heel_x = variable[0]->x->value();
      auto well_spline_heel_y = variable[0]->y->value();
      auto well_spline_heel_z = variable[0]->z->value();
      auto well_spline_toe_x = variable[1]->x->value();
      auto well_spline_toe_y = variable[1]->y->value();
      auto well_spline_toe_z = variable[1]->z->value();

      wellEconomy.well_xy[well->name().toStdString()] = sqrt(
          pow((well_spline_heel_x - well_spline_toe_x), 2) + pow((well_spline_heel_y - well_spline_toe_y), 2));
      wellEconomy.well_z[well->name().toStdString()] = abs(well_spline_heel_z - well_spline_toe_z);
      wellEconomy.well_lengths[well->name().toStdString()] = sqrt(
          pow((well_spline_heel_x - well_spline_toe_x), 2) + pow((well_spline_heel_y - well_spline_toe_y), 2)
              + pow((well_spline_heel_z - well_spline_toe_z), 2));

    }

  }

  return wellEconomy;

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
    for (auto const key : realization_ofv_map_.keys()) {
        valmap["Rea#"+key.toStdString()] = vector<double>{realization_ofv_map_[key]};
    }
    return valmap;
}
void Model::set_grid_path(const std::string &grid_path) {
    if (grid_ != 0) {
//        delete grid_; // This should not be deleted because wicalc_rixx keeps the object to avoid having to re-read it.
    }
    grid_ = new Reservoir::Grid::ECLGrid(grid_path);
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
            if (cont->mode() == Settings::Model::ControlMode::RateControl) {
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
