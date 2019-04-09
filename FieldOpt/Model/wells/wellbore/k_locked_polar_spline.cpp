/******************************************************************************
   Created by einar on 4/4/19.
   Copyright (C) 2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include <Utilities/verbosity.h>
#include "k_locked_polar_spline.h"

namespace Model {
namespace Wells {
namespace Wellbore {

KLockedPolarSpline::KLockedPolarSpline(::Settings::Model::Well well_settings,
                                       Properties::VariablePropertyContainer *variable_container,
                                       Reservoir::Grid::Grid *grid,
                                       Reservoir::WellIndexCalculation::wicalc_rixx *wic) {
    grid_ = grid;

    auto heel_pt = well_settings.spline_heel;
    heel_cell_ = grid_->GetCellEnvelopingPoint(heel_pt.x, heel_pt.y, heel_pt.z);
    k_ = heel_cell_.ijk_index().k();

    x_ = new Properties::ContinousProperty(heel_pt.x);
    y_ = new Properties::ContinousProperty(heel_pt.y);
    azimuth_ = new Properties::ContinousProperty(well_settings.polar_spline.azimuth);
    length_ = new Properties::ContinousProperty(well_settings.polar_spline.length);

    // This probably wont actually matter, maybe?
    spline_points_ = QList<SplinePoint*>();
    SplinePoint *sp_heel = new SplinePoint();
    sp_heel->x = new Properties::ContinousProperty(heel_pt.x);
    sp_heel->y = new Properties::ContinousProperty(heel_pt.y);
    sp_heel->z = new Properties::ContinousProperty(heel_cell_.center().z());
    SplinePoint *sp_toe = new SplinePoint(); // Will be updated by updateEndpointCells
    sp_toe->x = new Properties::ContinousProperty(heel_pt.x);
    sp_toe->y = new Properties::ContinousProperty(heel_pt.y);
    sp_toe->z = new Properties::ContinousProperty(heel_cell_.center().z());
    spline_points_.append(sp_heel);
    spline_points_.append(sp_toe);
    updateEndpointCells();
}
void KLockedPolarSpline::updateEndpointCells() {
    double pol_azm = azimuth_->value()*M_PI/180.0;
    heel_cell_ = grid_->GetCellEnvelopingPointInLayer(x_->value(), y_->value(), k_);
    double toe_x = x_->value() + cos(pol_azm) * length_->value();
    double toe_y = y_->value() + sin(pol_azm) * length_->value();

    int max_attempts = 100;
    int attempt = 0;
    double step_scale = 1;
    while (toe_cell_.global_index() != heel_cell_.global_index() && attempt <= max_attempts) {
        try {
            toe_cell_ = grid_->GetCellEnvelopingPointInLayer(toe_x, toe_y, k_);
            break;
        }
        catch (std::runtime_error e) {
            if (attempt % 10 == 0) Printer::ext_warn("Unable to find toe cell after " + Printer::num2str(attempt) + " attempts.");
            attempt++;
            step_scale = 1 - double(attempt) / double(max_attempts);
            toe_x = x_->value() + cos(pol_azm) * length_->value() * step_scale;
            toe_y = y_->value() + sin(pol_azm) * length_->value() * step_scale;
        }
    }
    if (VERB_MOD >= 3) {
        Printer::ext_info("Found toe cell " + toe_cell_.to_string(), "Model", "KLockedPolarSpline");
    }

    spline_points_[0]->FromEigenVector(Eigen::Vector3d(x_->value(), y_->value(), heel_cell_.center().z()));
    spline_points_[1]->FromEigenVector(Eigen::Vector3d(toe_x, toe_y, toe_cell_.center().z()));
}
std::vector<Reservoir::Grid::Cell> KLockedPolarSpline::getAdjacentCells(const int avoid_idx, Reservoir::Grid::Cell current_cell) const {
    vector<Reservoir::Grid::Cell> cells;
    if (current_cell.ijk_index().i() > 0) {
        auto cell = grid_->GetCell(current_cell.ijk_index().i()-1, current_cell.ijk_index().j(), k_);
        if (cell.global_index() != avoid_idx)
            cells.push_back(cell);
    }
    if (current_cell.ijk_index().i() < grid_->Dimensions().nx-1) {
        auto cell = grid_->GetCell(current_cell.ijk_index().i()+1, current_cell.ijk_index().j(), k_);
        if (cell.global_index() != avoid_idx)
            cells.push_back(cell);
    }
    if (current_cell.ijk_index().j() > 0) {
        auto cell = grid_->GetCell(current_cell.ijk_index().i(), current_cell.ijk_index().j() - 1, k_);
        if (cell.global_index() != avoid_idx)
            cells.push_back(cell);
    }
    if (current_cell.ijk_index().i() < grid_->Dimensions().ny-1) {
        auto cell = grid_->GetCell(current_cell.ijk_index().i(), current_cell.ijk_index().j() + 1, k_);
        if (cell.global_index() != avoid_idx)
            cells.push_back(cell);
    }
    return cells;
}
std::pair<Reservoir::Grid::Cell, Eigen::Vector3d> KLockedPolarSpline::findNext(Reservoir::Grid::Cell previous_cell, Reservoir::Grid::Cell current_cell, Eigen::Vector3d current_point) const {
    assert(current_cell.EnvelopsPoint(current_point));
    double minln = std::min(current_cell.dx(), current_cell.dy());
    double delta = minln / 100.0;
    double step = delta;
    auto candidate_cells = getAdjacentCells(previous_cell.global_index(), current_cell);

    if (VERB_MOD >= 4) {
        std::stringstream ss;
        ss << "Finding next cell. Origin cell: " << current_cell.to_string() << "|";
        ss << "Neighbouring cells: | ";
        for (auto c : candidate_cells) {
            ss << c.to_string() << " | ";
        }
        Printer::ext_info(ss.str(), "Model", "KLockedPolarSpline");
    }

    for (auto cand : candidate_cells) {
        double pol_azm = azimuth_->value()*M_PI/180.0;
        while (step < 2.0*minln) {
            double xstep = cos(pol_azm) * step;
            double ystep = sin(pol_azm) * step;
            double zstep = cand.center().z() - current_cell.center().z();
            Eigen::Vector3d cand_point = current_point + Eigen::Vector3d( xstep, ystep, zstep );
            if (cand.EnvelopsPoint(cand_point)) {
                std::cout << "Found neighbouring cell" << std::endl;
                return std::pair<Reservoir::Grid::Cell, Eigen::Vector3d>(cand, cand_point);
            }
            step += delta;
        }
    }
    std::stringstream ss;
    Printer::error("Unable to find next neighbouring cell.");
    throw std::runtime_error("Unable to find cell enveloping point in layer.");
}
QList<WellBlock *> *KLockedPolarSpline::GetWellBlocks() {
    updateEndpointCells();
    return computeWellBlocks();
}

vector<Vector3d> KLockedPolarSpline::getPoints() const {
    std::vector<Eigen::Vector3d> points;
    std::vector<Reservoir::Grid::Cell> cells;
    cells.push_back(heel_cell_);
    points.push_back(Eigen::Vector3d(x_->value(), y_->value(), heel_cell_.center().z()));
    while (std::abs((points.back() - points.front()).norm()) < length_->value() && cells.back().global_index() != toe_cell_.global_index()) {
        try {
            auto prev_cell = cells.size() > 1 ? cells[cells.size()-2] : cells.back();
            auto next = findNext(prev_cell, cells.back(), points.back());
            points.push_back(next.second);
            cells.push_back(next.first);
        }
        catch (std::runtime_error e) {
            Printer::ext_warn("Well spline passes outside grid before reaching target length.", "Model", "KLockedPolarSpline");
            break;
        }
    }
    assert(cells.back().global_index() == toe_cell_.global_index());
    if (cells.front().global_index() == cells.back().global_index() || cells.size() == 1) {
        Printer::ext_warn("Heel and toe cells are the same. Throwing exception.");
        throw std::runtime_error("Heel and toe cells are the same in KLockedPolarSpline.");
    }
    return points;
}

}
}
}
