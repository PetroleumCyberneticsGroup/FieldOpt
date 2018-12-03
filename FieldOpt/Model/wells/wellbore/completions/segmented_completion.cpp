/******************************************************************************
   Copyright (C) 2015-2018 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#include "segmented_completion.h"

namespace Model {
namespace Wells {
namespace Wellbore {
namespace Completions {

SegmentedCompletion::SegmentedCompletion(Settings::Model::Well::Completion completion_settings,
                                         Model::Properties::VariablePropertyContainer *variable_container)
    : Completion(completion_settings)
{
    placement_ = new Properties::ContinousProperty(completion_settings.placement);

    tvd_ = completion_settings.true_vertical_depth;
    roughness_ = completion_settings.roughness;
    diameter_ = completion_settings.diameter;
}

double SegmentedCompletion::md(const double &well_length) const {
    if (placement_->value() < 0) {
        throw std::runtime_error("Attemting to get unset MD from segmented completion.");
    }
    else
        return placement_->value() * well_length;
}
double SegmentedCompletion::placement() const {
    if (placement_->value() < 0) {
        throw std::runtime_error("Attemting to get unset MD from segmented completion.");
    }
    else
        return placement_->value();
}

double SegmentedCompletion::tvd() const {
    if (tvd_ < 0)
        throw std::runtime_error("Attempting to get unset TVD from segmented completion.");
    return tvd_;
}

double SegmentedCompletion::roughness() const {
    if (roughness_ < 0)
        throw std::runtime_error("Attemting to get unset roughness from segmented completion.");
    return roughness_;
}

double SegmentedCompletion::diam() const {
    if (diameter_ < 0)
        throw std::runtime_error("Attemting to get unset diameter from segmented completion.");
    return diameter_;
}
void SegmentedCompletion::setPlacement(const double &placement) {
    if (placement < 0)
        throw std::runtime_error("Attemting to set invalid MD for segmented completion.");
    placement_->setValue(placement);

}
void SegmentedCompletion::setTvd(const double &tvd) {
    if (tvd < 0)
        throw std::runtime_error("Attemting to set invalid TVD for segmented completion.");
    tvd_ = tvd;

}
}
}
}
}
