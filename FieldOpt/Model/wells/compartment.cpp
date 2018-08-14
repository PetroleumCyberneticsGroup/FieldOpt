/******************************************************************************
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

#include "compartment.h"

namespace Model {
namespace Wells {

Compartment::Compartment() { }
Compartment::Compartment(const double start_md,
                         const double end_md,
                         const double start_tvd,
                         const double end_tvd,
                         const Settings::Model::Well &well_settings,
                         Properties::VariablePropertyContainer *variable_container,
                         std::vector<Compartment> &compartments_) {
    Settings::Model::Well::Completion comp_settings;
    if (compartments_.size() == 0) {
        assert(start_md == 0.0);
        comp_settings.name = "Packer#" + well_settings.name + "#0";
        comp_settings.measured_depth = start_md;
        comp_settings.true_vertical_depth = start_tvd;
        comp_settings.type = Settings::Model::WellCompletionType::Packer;
        comp_settings.variable_placement = well_settings.seg_compartment_params.variable_placement;
        start_packer = new Wellbore::Completions::Packer(comp_settings, variable_container);
    }
    else {
        start_packer = compartments_[compartments_.size() - 1].end_packer;
    }

    comp_settings.name = "Packer#" + well_settings.name + "#" + QString::number(compartments_.size() + 1);
    comp_settings.measured_depth = end_md;
    comp_settings.true_vertical_depth = end_tvd;
    comp_settings.type = Settings::Model::WellCompletionType::Packer;
    comp_settings.variable_placement = well_settings.seg_compartment_params.variable_placement;
    end_packer = new Wellbore::Completions::Packer(comp_settings, variable_container);

    comp_settings.name = "ICD#" + well_settings.name + "#" + QString::number(compartments_.size());
    comp_settings.measured_depth = start_md;
    comp_settings.true_vertical_depth = start_tvd;
    comp_settings.valve_size = well_settings.seg_compartment_params.valve_size;
    comp_settings.valve_flow_coeff = well_settings.seg_compartment_params.valve_flow_coeff;
    comp_settings.variable_strength = well_settings.seg_compartment_params.variable_strength;
    comp_settings.type = Settings::Model::WellCompletionType::ICV;
    icd = new Wellbore::Completions::ICD(comp_settings, variable_container);
}
double Compartment::GetLength() const {
    return end_packer->md() - start_packer->md();
}
double Compartment::GetTVDDifference() const {
    return end_packer->tvd() - start_packer->tvd();
}
void Compartment::Update() {
    icd->setMd(start_packer->md());
    icd->setTvd(start_packer->tvd());
}
}
}
