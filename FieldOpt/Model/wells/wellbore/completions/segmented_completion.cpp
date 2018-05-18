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
    md_ = new Properties::ContinousProperty(completion_settings.measured_depth);
    md_->setName(completion_settings.name);

    tvd_ = completion_settings.true_vertical_depth;
    roughness_ = completion_settings.roughness;
    diameter_ = completion_settings.diameter;

    if (completion_settings.variable_placement == true) {
        variable_container->AddVariable(md_);
    }

    seg_branch_ = -1;
    seg_index_ = -1;
    seg_outlet_ = -1;
}

double SegmentedCompletion::md() const {
    if (md_->value() < 0)
        throw std::runtime_error("Attemting to get unset MD from segmented completion.");
    return md_->value();
}

double SegmentedCompletion::tvd() const {
    if (tvd_ < 0)
        throw std::runtime_error("Attemting to get unset TVD from segmented completion.");
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
int SegmentedCompletion::segIdx() const {
    if (seg_idx_ < 0)
        throw std::runtime_error("Attempting to get unset segment index from segmented completion.");
    return seg_idx_;
}
int SegmentedCompletion::segOutletIdx() const {
    if (seg_outlet_idx_ < 0)
        throw std::runtime_error("Attempting to get unset segment outlet index from segmented completion.");
    return seg_outlet_idx_;
}
int SegmentedCompletion::segBranchIdx() const {
    if (seg_branch__idx_ < 0)
        throw std::runtime_error("Attempting to get unset segment branch index from segmented completion.");
    return seg_branch__idx_;
}
void SegmentedCompletion::setMd(const double &md) {
    if (md < 0)
        throw std::runtime_error("Attemting to set invalid MD for segmented completion.");
    md_->setValue(md);

}
void SegmentedCompletion::setTvd(const double &tvd) {
    if (tvd < 0)
        throw std::runtime_error("Attemting to set invalid TVD for segmented completion.");
    tvd_ = tvd;

}
void SegmentedCompletion::setSegIdx(const int &seg_idx) {
    if (seg_idx < 0)
        throw std::runtime_error("Attemting to set invalid segment index for segmented completion.");
    seg_idx_ = seg_idx;

}
void SegmentedCompletion::setSegOutletIdx(const int &seg_outlet_idx) {
    if (seg_outlet_idx < 0)
        throw std::runtime_error("Attemting to set invalid segment outlet index for segmented completion.");
    seg_outlet_idx_ = seg_outlet_idx;

}
void SegmentedCompletion::setSegBranchIdx(const int &seg_branch_idx) {
    if (seg_branch_idx < 0)
        throw std::runtime_error("Attemting to set invalid segment branch index for segmented completion.");
    seg_branch_idx_ = seg_branch_idx;

}

}
}
}
}
