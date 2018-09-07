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

#include "wellspline.h"
#include <iostream>
#include <time.h>
#include <cmath>
#include <wells/well_exceptions.h>
#include <QtCore/QDateTime>
#include <Utilities/time.hpp>
#include <boost/algorithm/string.hpp>
#include <QList>
#include <Utilities/verbosity.h>
#include <Utilities/printer.hpp>
#include "WellIndexCalculation/wicalc_rixx.h"

namespace Model {
namespace Wells {
namespace Wellbore {
using namespace Reservoir::WellIndexCalculation;

WellSpline::WellSpline(Settings::Model::Well well_settings,
                       Properties::VariablePropertyContainer *variable_container,
                       Reservoir::Grid::Grid *grid,
                       Reservoir::WellIndexCalculation::wicalc_rixx *wic
)
{
    assert(grid_ != nullptr && grid_ != 0);
    grid_ = grid;
    well_settings_ = well_settings;
    is_variable_ = false;
    wic_ = new Reservoir::WellIndexCalculation::wicalc_rixx(grid_);
    wic = wic_;

    if (!well_settings.imported_wellblocks_.empty()) { // Imported blocks present
        spline_points_from_import(well_settings);
        imported_wellblocks_ = well_settings.imported_wellblocks_;
    }
    if (VERB_MOD >= 2) {
        Printer::ext_info("Initializing well spline for well " + well_settings.name.toStdString()
            + ". N points: " + Printer::num2str(well_settings.spline_points.size()) + "; First spline point name: "
            + well_settings.spline_points[0].name.toStdString(),
            "Model", "WellSpline");
    }

    for (auto point : well_settings.spline_points) {
        if (VERB_MOD >= 2) {
            Printer::ext_info("Adding new spline point for well " + well_settings.name.toStdString() + ": "
                + Printer::num2str(point.x) + ", " + Printer::num2str(point.y) + ", " + Printer::num2str(point.z)
                + " (" + point.name.toStdString() + ")",
                "Model", "WellSpline");
        }
        SplinePoint *pt = new SplinePoint();
        pt->x = new ContinousProperty(point.x);
        pt->y = new ContinousProperty(point.y);
        pt->z = new ContinousProperty(point.z);
        assert(point.name.size() > 0);
        pt->x->setName(point.name + "#x");
        pt->y->setName(point.name + "#y");
        pt->z->setName(point.name + "#z");
        if (point.is_variable) {
            is_variable_ = true;
            variable_container->AddVariable(pt->x);
            variable_container->AddVariable(pt->y);
            variable_container->AddVariable(pt->z);
        }
        spline_points_.push_back(pt);
    }
    seconds_spent_in_compute_wellblocks_ = 0;

    last_computed_grid_ = "";
    last_computed_spline_ = std::vector<Eigen::Vector3d>();
}
void WellSpline::spline_points_from_import(Settings::Model::Well &well_settings) {
    QString name_base = "SplinePoint#" + well_settings.name + "#";
    well_settings_.spline_points = QList<Settings::Model::Well::SplinePoint> ();
    auto first_spline_point = Settings::Model::Well::SplinePoint();
    first_spline_point.x = well_settings.imported_wellblocks_[0].in().x();
    first_spline_point.y = well_settings.imported_wellblocks_[0].in().y();
    first_spline_point.z = well_settings.imported_wellblocks_[0].in().z();
    if (well_settings.is_variable_spline) {
            first_spline_point.is_variable = true;
    }
    first_spline_point.name =  name_base + "heel";

    well_settings.spline_points.push_back(first_spline_point);
    int i = 1;
    for (auto block : well_settings.imported_wellblocks_) {
            auto spline_point = Settings::Model::Well::SplinePoint();
            spline_point.x = block.out().x();
            spline_point.y = block.out().y();
            spline_point.z = block.out().z();
            if (well_settings.is_variable_spline) {
                spline_point.is_variable = true;
            }
            spline_point.name = name_base + "P" + QString::number(i++);
            well_settings.spline_points.push_back(spline_point);
    }
    well_settings.spline_points.back().name = name_base + "heel";
}

QList<WellBlock *> *WellSpline::GetWellBlocks()
{
    assert(spline_points_.size() >= 2);
    assert(grid_ != nullptr && grid_ != 0);

    if (!wic_->HasGrid(grid_->GetGridFilePath())) {
        wic_->AddGrid(grid_);
    }
    wic_->SetGridActive(grid_);


    last_computed_grid_ = grid_->GetGridFilePath();
    last_computed_spline_ = create_spline_point_vector();

    WellDefinition welldef;
    welldef.wellname = well_settings_.name.toStdString();

    for (int w = 0; w < spline_points_.size() - 1; ++w) {
        welldef.radii.push_back(well_settings_.wellbore_radius);
        welldef.skins.push_back(0.0);
        welldef.skins.push_back(0.0);
        welldef.heels.push_back(spline_points_[w]->ToEigenVector());
        welldef.toes.push_back(spline_points_[w+1]->ToEigenVector());
        if (welldef.heel_md.size() == 0) {
            welldef.heel_md.push_back(0.0);
        }
        else {
            double prev_toe = welldef.toe_md.back();
            welldef.heel_md.push_back(prev_toe);
        }
        welldef.toe_md.push_back(
            welldef.heel_md.back() + (welldef.toes.back() - welldef.heels.back()).norm()
        );
    }


    if (VERB_MOD >= 2) {
        Printer::info("Starting well index calculation.");
    }
    auto start = QDateTime::currentDateTime();
    vector<IntersectedCell> block_data;
    if (imported_wellblocks_.empty() || is_variable_) {
        wic_->ComputeWellBlocks(block_data, welldef);
    }
    else {
        if (VERB_MOD >= 1) {
            Printer::ext_warn("Well index calculation for imported paths is not properly implemented at this time.",
                              "Model", "WellSpline");
        }
        block_data = convertImportedWellblocksToIntersectedCells();
        for (int i = 0; i < block_data.size(); ++i) {
            wic_->ComputeWellBlocks(block_data, welldef);
        }
    }
    auto end = QDateTime::currentDateTime();
    seconds_spent_in_compute_wellblocks_ = time_span_seconds(start, end);

    QList<WellBlock *> *blocks = new QList<WellBlock *>();
    for (int i = 0; i < block_data.size(); ++i) {
        blocks->append(getWellBlock(block_data[i]));
        blocks->last()->setEntryPoint(block_data[i].get_segment_entry_point(0));
        blocks->last()->setExitPoint(block_data[i].get_segment_exit_point(0));
        blocks->last()->setEntryMd(block_data[i].get_segment_entry_md(0));
        blocks->last()->setExitMd(block_data[i].get_segment_exit_md(0));
    }
    if (blocks->size() == 0) {
        throw WellBlocksNotDefined("WIC could not compute.");
    }
    if (VERB_MOD >= 2) {
        Printer::info("Done computing WIs after " + boost::lexical_cast<std::string>(seconds_spent_in_compute_wellblocks_) + " seconds");
    }
    if (VERB_MOD >=2) {
        Printer::ext_info("Computed " + Printer::num2str(blocks->size()) + " well blocks from "
                        + Printer::num2str(block_data.size()) + " intersected cells for well " + welldef.wellname,
                        "Model", "WellSpline");
    }
    return blocks;
}

WellBlock *WellSpline::getWellBlock(Reservoir::WellIndexCalculation::IntersectedCell block_data)
{
    if (VERB_MOD >= 3) {
        Printer::info("Creating WellBlock for IC " + block_data.ijk_index().to_string() + " with WI " + Printer::num2str(block_data.cell_well_index_matrix()));
    }
    auto wb = new WellBlock(block_data.ijk_index().i()+1, block_data.ijk_index().j()+1, block_data.ijk_index().k()+1);
    auto comp = new Completions::Perforation();
    comp->setTransmissibility_factor(block_data.cell_well_index_matrix());
    wb->AddCompletion(comp);
    return wb;
}
std::vector<Eigen::Vector3d> WellSpline::create_spline_point_vector() const {
    std::vector<Eigen::Vector3d> spline_points;
    for (auto point : spline_points_) {
        spline_points.push_back(point->ToEigenVector());
    }
    return spline_points;
}
bool WellSpline::HasGridChanged() const {
    return last_computed_grid_.size() == 0 || !boost::equals(last_computed_grid_, grid_->GetGridFilePath());
}
bool WellSpline::HasSplineChanged() const {
    if (last_computed_spline_.size() == 0) {
        return true;
    }

    std::vector<Eigen::Vector3d> new_spline_points;
    for (auto point : spline_points_) {
        new_spline_points.push_back(point->ToEigenVector());
    }
    assert(new_spline_points.size() == last_computed_spline_.size());

    double point_difference_sum = 0;
    for (int i = 0; i < last_computed_spline_.size(); ++i) {
        point_difference_sum += std::abs((last_computed_spline_[i] - new_spline_points[i]).norm());
    }
    return point_difference_sum > 1e-7;
}
std::vector<Reservoir::WellIndexCalculation::IntersectedCell> WellSpline::convertImportedWellblocksToIntersectedCells() {
    auto intersected_cells = vector<IntersectedCell>();
    double md_in = 0;
    double md_out = 0;
    for (auto iwb : imported_wellblocks_) {
        auto cell = grid_->GetCell(iwb.ijk().x()-1, iwb.ijk().y()-1, iwb.ijk().z()-1);
        auto ic = Reservoir::WellIndexCalculation::IntersectedCell(cell);
        md_out = md_in + (iwb.out() - iwb.in()).norm();
        ic.add_new_segment(iwb.in(), iwb.out(), md_in, md_out, well_settings_.wellbore_radius, 0.0);
        intersected_cells.push_back(ic);
        md_in = md_out;
    }
    return intersected_cells;
}

Eigen::Vector3d WellSpline::SplinePoint::ToEigenVector() const {
    return Eigen::Vector3d(this->x->value(), this->y->value(), this->z->value());
}
}
}
}
