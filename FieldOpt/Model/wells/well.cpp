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

#include <Utilities/printer.hpp>
#include <boost/lexical_cast.hpp>
#include "well.h"

namespace Model {
namespace Wells {

Well::Well(Settings::Model settings,
           int well_number,
           Properties::VariablePropertyContainer *variable_container,
           Reservoir::Grid::Grid *grid,
           Reservoir::WellIndexCalculation::wicalc_rixx *wic
)
{
    well_settings_ = settings.wells()[well_number];

    name_ = well_settings_.name;
    type_ = well_settings_.type;
    if (well_settings_.group.length() >= 1)
        group_ = well_settings_.group;
    else group_ = "";

    preferred_phase_ = well_settings_.preferred_phase;

    wellbore_radius_ = new Properties::ContinousProperty(well_settings_.wellbore_radius);

    controls_ = new QList<Control *>();
    for (int i = 0; i < well_settings_.controls.size(); ++i)
        controls_->append(new Control(well_settings_.controls[i], well_settings_, variable_container));

    trajectory_ = new Wellbore::Trajectory(well_settings_, variable_container, grid, wic);

    heel_.i = trajectory_->GetWellBlocks()->first()->i();
    heel_.j = trajectory_->GetWellBlocks()->first()->j();
    heel_.k = trajectory_->GetWellBlocks()->first()->k();

    if (well_settings_.use_segmented_model) {
        is_segmented_ = true;
        initializeSegmentedWell(variable_container);
    }
    else {
        is_segmented_ = false;
    }
}

bool Well::IsProducer()
{
    return type_ == ::Settings::Model::WellType::Producer;
}

bool Well::IsInjector()
{
    return type_ == ::Settings::Model::WellType::Injector;
}

void Well::Update() {
    trajectory_->UpdateWellBlocks();
    heel_.i = trajectory_->GetWellBlocks()->first()->i();
    heel_.j = trajectory_->GetWellBlocks()->first()->j();
    heel_.k = trajectory_->GetWellBlocks()->first()->k();

    if (is_segmented_) {
        for (auto compartment : compartments_) {
            compartment.Update();
        }
    }
}
void Well::initializeSegmentedWell(Properties::VariablePropertyContainer *variable_container) {
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
    assert(compartment_delimiters.size() == well_settings_.seg_n_compartments + 1);

    for (int i = 0; i < well_settings_.seg_n_compartments; ++i) {
        auto first_block = trajectory_->GetWellBlockByMd(compartment_delimiters[i]);
        auto last_block = trajectory_->GetWellBlockByMd(compartment_delimiters[i+1]);
        if (first_block->i() == last_block->i() &&
            first_block->j() == last_block->j() &&
            first_block->k() == last_block->k()) { // Compartment begins and ends in the same block
            Printer::ext_warn("Compartment " + boost::lexical_cast<std::string>(i)
                                  + " begins and ends in the same well block.", "Model", "Well");
            compartments_.push_back(Compartment(trajectory_->GetEntryMd(first_block), trajectory_->GetExitMd(last_block),
                                                first_block->getEntryPoint().z(), last_block->getExitPoint().z(),
                                                trajectory()->GetLength(),
                                                well_settings_, variable_container, compartments_));
        }
        else if (i < well_settings_.seg_n_compartments - 1) { // Not last compartment
            compartments_.push_back(Compartment(trajectory_->GetEntryMd(first_block), trajectory_->GetEntryMd(last_block),
                                                first_block->getEntryPoint().z(), last_block->getExitPoint().z(),
                                                trajectory()->GetLength(),
                                                well_settings_, variable_container, compartments_));
        }
        else { // Last compartment
            compartments_.push_back(Compartment(trajectory_->GetEntryMd(first_block), trajectory_->GetExitMd(last_block),
                                                first_block->getEntryPoint().z(), last_block->getExitPoint().z(),
                                                trajectory()->GetLength(),
                                                well_settings_, variable_container, compartments_));
        }
    }
}

// Segmented well methods
std::vector<Packer *> Well::GetPackers() const {
    assert(is_segmented_);
    std::vector<Packer *> packers;
    packers.push_back(compartments_[0].start_packer);
    for (auto comp : compartments_) {
        packers.push_back(comp.end_packer);
    }
    return packers;
}
std::vector<ICD *> Well::GetICDs() const {
    assert(is_segmented_);
    std::vector<ICD *> icds;
    for (auto comp : compartments_) {
        icds.push_back(comp.icd);
    }
    return icds;
}
std::vector<Compartment> Well::GetCompartments() const {
    assert(is_segmented_);
    return compartments_;
}
std::vector<Segment> Well::GetSegments() {
    assert(is_segmented_);
    std::vector<Segment> segments;
    auto root_segment = Segment(Segment::SegType::TUBING_SEGMENT, 1, 1, -1, -1.0,
                                compartments_.front().start_packer->tvd(),
                                -1.0, -1.0, 0.0
    );
    segments.push_back(root_segment);

    std::vector<int> tubing_indexes = createTubingSegments(segments);
    std::vector<int> icd_indexes = createICDSegments(segments, tubing_indexes);
    createAnnulusSegments(segments, icd_indexes);

    return segments;
}
void Well::createAnnulusSegments(std::vector<Segment> &segments, const std::vector<int> &icd_indexes) {
    assert(is_segmented_);
    std::vector<int> annulus_indexes;
    for (int i = 0; i < compartments_.size(); ++i) {
        auto comp_blocks = trajectory()->GetWellBlocksByMdRange(compartments_[i].start_packer->md(trajectory()->GetLength()),
                                                                compartments_[i].end_packer->md(trajectory()->GetLength())
        );
        for (int j = 0; j < comp_blocks.size(); ++j) {
            double outlet_md;
            int index = 2*compartments_.size() + annulus_indexes.size() + 2;
            int outlet_index;
            if (j == 0) { // Outlet to ICD
                outlet_index = icd_indexes[i];
                outlet_md = segments[icd_indexes[i]].OutletMD();
                segments[compartments_.size() + i + 1].AddInlet(index); // Add to ICD inlet list
            }
            else { // Outlet to previous annulus segment
                outlet_index = annulus_indexes.back();
                outlet_md = segments.back().OutletMD();
                segments.back().AddInlet(index);
            }
            auto ann_seg = Segment(
                Segment::ANNULUS_SEGMENT,
                index,                         // index
                compartments_.size() + 2 + i,  // branch
                outlet_index,                  // outlet
                (comp_blocks[j]->getExitPoint() - comp_blocks[j]->getEntryPoint()).norm(), // length
                comp_blocks[j]->getExitPoint().z() - comp_blocks[j]->getEntryPoint().z(),  // tvd delta
                ann_diam_, ann_roughness_, outlet_md
            );
            ann_seg.AddParentBlock(comp_blocks[j]);
            annulus_indexes.push_back(index);
            segments.push_back(ann_seg);
        }
    }
}
std::vector<int> Well::createICDSegments(std::vector<Segment> &segments, std::vector<int> &tubing_indexes) const {
    assert(is_segmented_);
    std::vector<int> icd_indexes;
    for (int i = 0; i < compartments_.size(); ++i) {
        int index = tubing_indexes.back() + 1 + i;
        auto icd_segment = Segment(
            Segment::ICD_SEGMENT,
            index, // index
            i + 2, // branch
            i + 1, // oulet
            0.1,   // length
            0.0,   // tvd delta
            tub_diam_, tub_roughness_,
            segments[tubing_indexes[i]].OutletMD() // outlet md
        );
        icd_segment.AddParentICD(compartments_[i].icd);
        icd_indexes.push_back(index);
        segments[i].AddInlet(index);
        segments.push_back(icd_segment);
    }
    return icd_indexes;
}
std::vector<int> Well::createTubingSegments(std::vector<Segment> &segments) const {
    assert(is_segmented_);
    std::vector<int> tubing_indexes;
    for (int i = 0; i < compartments_.size(); ++i) {
        int index = i + 2;
        auto tubing_segment = Segment(
            Segment::TUBING_SEGMENT,              // type
            index,                                // index
            1,                                    // branch
            index - 1,                            // outlet
            compartments_[i].GetLength(trajectory_->GetLength()),         // length
            compartments_[i].GetTVDDifference(),  // tvd delta
            tub_diam_, tub_roughness_,
            segments.back().OutletMD()
        );
        segments.push_back(tubing_segment);
        tubing_indexes.push_back(index);
        segments[segments.size() - 2].AddInlet(index);
    }
    return tubing_indexes;
}
std::vector<Segment> Well::GetTubingSegments() {
    assert(is_segmented_);
    std::vector<Segment> segments;
    auto root_segment = Segment(Segment::SegType::TUBING_SEGMENT, 1, 1, -1, -1.0,
                                compartments_.front().start_packer->tvd(),
                                -1.0, -1.0, 0.0
    );
    segments.push_back(root_segment);
    std::vector<int> tubing_indexes = createTubingSegments(segments);
    return segments;
}
std::vector<Segment> Well::GetICDSegments() {
    assert(is_segmented_);
    std::vector<Segment> segments;
    auto root_segment = Segment(Segment::SegType::TUBING_SEGMENT, 1, 1, -1, -1.0,
                                compartments_.front().start_packer->tvd(),
                                -1.0, -1.0, 0.0
    );
    segments.push_back(root_segment);

    std::vector<int> tubing_indexes = createTubingSegments(segments);
    std::vector<int> icd_indexes = createICDSegments(segments, tubing_indexes);
    std::vector<Segment> icd_segments;
    for (auto seg : segments) {
        if (seg.Type() == Segment::SegType::ICD_SEGMENT) {
            icd_segments.push_back(seg);
        }
    }
    return icd_segments;
}
std::vector<Segment> Well::GetAnnulusSegments() {
    assert(is_segmented_);
    std::vector<Segment> segments;
    auto root_segment = Segment(Segment::SegType::TUBING_SEGMENT, 1, 1, -1, -1.0,
                                compartments_.front().start_packer->tvd(),
                                -1.0, -1.0, 0.0
    );
    segments.push_back(root_segment);

    std::vector<int> tubing_indexes = createTubingSegments(segments);
    std::vector<int> icd_indexes = createICDSegments(segments, tubing_indexes);
    createAnnulusSegments(segments, icd_indexes);
    std::vector<Segment> ann_segments;
    for (auto seg : segments) {
        if (seg.Type() == Segment::SegType::ANNULUS_SEGMENT) {
            ann_segments.push_back(seg);
        }
    }
    return ann_segments;
}

}
}
