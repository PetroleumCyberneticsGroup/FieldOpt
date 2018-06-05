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
    auto well_settings = settings.wells()[well_number];

    assert(trajectory_->GetDefinitionType() == Settings::Model::WellDefinitionType::WellSpline);

    tub_diam_ = well_settings.seg_tubing.diameter;
    tub_cross_sect_area_ = well_settings.seg_tubing.cross_sect_area;
    tub_roughness_ = well_settings.seg_tubing.roughness;
    ann_diam_ = well_settings.seg_annulus.diameter;
    ann_cross_sect_area_ = well_settings.seg_annulus.cross_sect_area;
    ann_roughness_ = well_settings.seg_annulus.roughness;


    // Set ICD names before constucting
    // Set packer names before constucting

    throw std::runtime_error("SegmentedWell not yet implemented.");
}

}
}
