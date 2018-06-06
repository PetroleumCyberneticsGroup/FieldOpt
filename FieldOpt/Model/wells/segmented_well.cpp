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

#include "segmented_well.h"

namespace Model {
namespace Wells {

SegmentedWell::SegmentedWell(const Settings::Model &settings,
                             int well_number,
                             Properties::VariablePropertyContainer *variable_container,
                             Reservoir::Grid::Grid *grid)
    : Well(settings, well_number, variable_container, grid)
{
    well_settings_ = settings.wells()[well_number];

    assert(well_settings_.seg_n_compartments > 0);
    assert(trajectory_->GetDefinitionType() == Settings::Model::WellDefinitionType::WellSpline);

    tub_diam_ = well_settings_.seg_tubing.diameter;
    tub_cross_sect_area_ = well_settings_.seg_tubing.cross_sect_area;
    tub_roughness_ = well_settings_.seg_tubing.roughness;
    ann_diam_ = well_settings_.seg_annulus.diameter;
    ann_cross_sect_area_ = well_settings_.seg_annulus.cross_sect_area;
    ann_roughness_ = well_settings_.seg_annulus.roughness;

    double compartment_length = trajectory_->GetLength() / well_settings_.seg_n_compartments;
    std::vector<double> compartment_delimiters;
    compartment_delimiters.push_back(0.0);
    for (int i = 1; i < well_settings_.seg_n_compartments; ++i) {
        compartment_delimiters.push_back(i * compartment_length);
    }
    compartment_delimiters.push_back(trajectory_->GetLength());
    assert(compartment_delimiters.size() == well_settings_.seg_n_compartments);

    for (int i = 0; i < well_settings_.seg_n_compartments; ++i) {
        auto first_block = trajectory_->GetWellBlockByMd(compartment_delimiters[i]);
        auto last_block = trajectory_->GetWellBlockByMd(compartment_delimiters[i+1]);
        if (first_block->i() == last_block->i() &&
            first_block->j() == last_block->j() &&
            first_block->k() == last_block->k()) { // Compartment begins and ends in the same block
            std::cout << "WARNING: Compartment " << i << " begins and ends in the same well block." << std::endl;
            compartments_.push_back(Compartment(trajectory_->GetEntryMd(first_block), trajectory_->GetExitMd(last_block),
                                                first_block->getEntryPoint().z(), last_block->getExitPoint().z(),
                                                well_settings_, variable_container, compartments_));
        }
        else if (i < well_settings_.seg_n_compartments - 1) { // Not last compartment
            compartments_.push_back(Compartment(trajectory_->GetEntryMd(first_block), trajectory_->GetEntryMd(last_block),
                                                first_block->getEntryPoint().z(), last_block->getExitPoint().z(),
                                                well_settings_, variable_container, compartments_));
        }
        else { // Last compartment
            compartments_.push_back(Compartment(trajectory_->GetEntryMd(first_block), trajectory_->GetExitMd(last_block),
                                                first_block->getEntryPoint().z(), last_block->getExitPoint().z(),
                                                well_settings_, variable_container, compartments_));
        }
    }

    throw std::runtime_error("SegmentedWell not yet implemented.");
}

SegmentedWell::Compartment::Compartment(const double start_md, const double end_md,
                                        const double start_tvd, const double end_tvd,
                                        const Settings::Model::Well &well_settings,
                                        Properties::VariablePropertyContainer *variable_container,
                                        std::vector<Compartment> &compartments_) {
    Settings::Model::Well::Completion comp_settings;
    if (compartments_.size() == 0) {
        assert(start_md == 0.0);
        comp_settings.name = "Packer#" + well_settings.name + "#0";
        comp_settings.measured_depth = start_md;
        comp_settings.true_vertical_depth = start_tvd;
        start_packer = new Wellbore::Completions::Packer(comp_settings, variable_container);
    }
    else {
        start_packer = compartments_[compartments_.size() - 1].end_packer;
    }

    comp_settings.name = "Packer#" + well_settings.name + "#" + QString::number(compartments_.size() + 1);
    comp_settings.measured_depth = end_md;
    comp_settings.true_vertical_depth = end_tvd;
    end_packer = new Wellbore::Completions::Packer(comp_settings, variable_container);

    comp_settings.name = "ICD#" + well_settings.name + "#" + QString::number(compartments_.size());
    comp_settings.measured_depth = start_md;
    comp_settings.true_vertical_depth = start_tvd;
    comp_settings.valve_size = well_settings.seg_compartment_params.valve_size;
    icd = new Wellbore::Completions::ICD(comp_settings, variable_container);
}
}
}
