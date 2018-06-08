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
#include "Model/wells/wellbore/completions/packer.h"
#include "Model/wells/wellbore/completions/icd.h"

namespace Model {
namespace Wells {

using namespace Wellbore::Completions;

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

  /*!
   * The Compartment struct models a compartment made up of two packers and an ICD.
   * The ICD is placed at the beginning of the compartment (i.e. closest to the
   * heel of the well).
   */
  struct Compartment {
    Compartment(const double start_md, const double end_md,
                const double start_tvd, const double end_tvd,
                const Settings::Model::Well &well_settings,
                Properties::VariablePropertyContainer *variable_container,
                std::vector<Compartment> &compartments_);
    Packer *start_packer;
    Packer *end_packer;
    ICD *icd;
  };

  std::vector<Compartment> GetCompartments() const;
  std::vector<Packer *> GetPackers() const;
  std::vector<ICD *> GetICDs() const;


 private:
  Settings::Model::Well well_settings_;
  double tub_diam_;            //!< Tubing (inner) diameter.
  double ann_diam_;            //!< Annular diameter.
  double tub_cross_sect_area_; //!< Tubing cross section area.
  double ann_cross_sect_area_; //!< Annular cross section area.
  double tub_roughness_;       //!< Roughness for tubing segments.
  double ann_roughness_;       //!< Roughness for annulus segments.

  std::vector<Compartment> compartments_;

};

}
}

#endif //FIELDOPT_SEGMENTED_WELL_H
