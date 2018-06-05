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

#ifndef FIELDOPT_SEGMENTED_WELL_H
#define FIELDOPT_SEGMENTED_WELL_H

#include "well.h"

namespace Model {
namespace Wells {

/*!
 * The SegmentedWell class is an extension of the Well class, used
 * to represent wells that are to be simulated using the segmented
 * well model. The primary methods contained within this class are
 * ones used to ease the generation and validation of valid
 * segmentations for wells.
 */
class SegmentedWell : public Well {
 public:
  SegmentedWell(const Settings::Model &settings,
                int well_number,
                Properties::VariablePropertyContainer *variable_container,
                Reservoir::Grid::Grid *grid);


 private:
  double tub_diam_;            //!< Tubing (inner) diameter.
  double ann_diam_;            //!< Annular diameter.
  double tub_cross_sect_area_; //!< Tubing cross section area.
  double ann_cross_sect_area_; //!< Annular cross section area.
  double tub_roughness_;       //!< Roughness for tubing segments.
  double ann_roughness_;       //!< Roughness for annulus segments.

};

}
}

#endif //FIELDOPT_SEGMENTED_WELL_H
