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

#ifndef FIELDOPT_COMPARTMENT_H
#define FIELDOPT_COMPARTMENT_H

#include "wellbore/completions/icd.h"
#include "wellbore/completions/packer.h"

namespace Model {
namespace Wells {

using namespace Wellbore::Completions;

/*!
 * The Compartment struct models a compartment made up of two packers and an ICD.
 * The ICD is placed at the beginning of the compartment (i.e. closest to the
 * heel of the well). This class is used by segmented wells to build the segmentation.
 */
class Compartment {
 public:
  Compartment();
  Compartment(const double start_md, const double end_md,
              const double start_tvd, const double end_tvd,
              const Settings::Model::Well &well_settings,
              Properties::VariablePropertyContainer *variable_container,
              std::vector<Compartment> &compartments_);
  double GetLength() const;
  double GetTVDDifference() const;
  Packer *start_packer;
  Packer *end_packer;
  ICD *icd;
};

}
}
#endif //FIELDOPT_COMPARTMENT_H
