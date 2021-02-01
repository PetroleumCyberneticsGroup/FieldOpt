/******************************************************************************
   Copyright (C) 2021 Brage Strand Kristoffersen <brage_sk@hotmail.com>

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

#include "awp.h"
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
#include <Utilities/random.hpp>
#include "Model/cpp-spline/src/Bezier.h"
#include "WellIndexCalculation/wicalc_rixx.h"
#include "Utilities/filehandling.hpp"
#include "Settings/settings_exceptions.h"
#include <cstring>


#include <QJsonDocument>

namespace Model {
namespace Wells {
namespace Wellbore {


AWP::AWP(Settings::Model::Well well_settings,
         Properties::VariablePropertyContainer *variable_container,
         Reservoir::Grid::Grid *grid,
         Reservoir::WellIndexCalculation::wicalc_rixx *wic
)
{
    grid_ = grid;
    assert(grid_ != nullptr);
    well_settings_ = well_settings;
    is_variable_ = false;
    use_bezier_spline_ = well_settings.use_bezier_spline;

    if (wic == nullptr) { // Initialize WIC if this is the first spline well initialized.
        wic = new Reservoir::WellIndexCalculation::wicalc_rixx(grid_);
        wic_ = wic;
    }
    else { // If not, use existing WIC object.
        wic_ = wic;
    }

    bool variable = well_settings.AWP_spline.is_variable;
    heel_ = new HorizontalPoint();
    heel_->x = new Properties::ContinousProperty(well_settings.AWP_spline.xh);
    heel_->y = new Properties::ContinousProperty(well_settings.AWP_spline.yh);

    toe_ = new HorizontalPoint();
    toe_->x = new Properties::ContinousProperty(well_settings.AWP_spline.xt);
    toe_->y = new Properties::ContinousProperty(well_settings.AWP_spline.yt);

    QString base_name = "AWPSpline#"+ well_settings.name + "#";

    if(variable){
        heel_->x->setName(base_name+"heel_x");
        heel_->y->setName(base_name+"heel_y");
        toe_->x->setName(base_name+"toe_x");
        toe_->y->setName(base_name+"toe_y");

        variable_container->AddVariable(heel_->x);
        variable_container->AddVariable(heel_->y);
        variable_container->AddVariable(toe_->x);
        variable_container->AddVariable(toe_->y);
    }

    horizontal_point_ = QList<HorizontalPoint*>();
    horizontal_point_.append(heel_);
    horizontal_point_.append(toe_);
}


QList<WellBlock *> *AWP::computeWellBlocks() {
    last_computed_grid_ = grid_->GetGridFilePath();
    last_computed_spline_ = create_spline_point_vector();

    WellDefinition welldef;
    welldef.wellname = well_settings_.name.toStdString();

    auto spline_points = getPoints();
    for (int w = 0; w < spline_points.size() - 1; ++w) {
        welldef.radii.push_back(well_settings_.wellbore_radius);
        welldef.skins.push_back(0.0);
        welldef.skins.push_back(0.0);
        welldef.heels.push_back(spline_points[w]);
        welldef.toes.push_back(spline_points[w+1]);
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

QList<WellBlock *> *AWP::GetWellBlocks()
{
    return computeWellBlocks();
}


vector<Eigen::Vector3d> AWP::getPoints() const {
    return createAWPTrajectory();
}



vector<Eigen::Vector3d> AWP::createAWPTrajectory() const {
    if (VERB_MOD >= 2) {
        Printer::ext_info("Generating bezier spline for well " + well_settings_.name.toStdString() + ". N original points: " + Printer::num2str(spline_points_.size()), "Model", "WellSpline");
    }
    //assert(spline_points_.size() == 2);
    QVariantMap map;
    map.insert("heel_x", heel_->x->value());
    map.insert("heel_y", heel_->y->value());

    map.insert("toe_x", toe_->x->value());
    map.insert("toe_y", toe_->y->value());


    auto gen = get_random_generator();
    int rand_int = random_integer(gen, 0, 2147483647);
    QJsonObject object = QJsonObject::fromVariantMap(map);
    QJsonDocument document;
    document.setObject(object);
    SaveJson(document, QString::fromStdString("src_python/from_fieldopt_to_python_"+to_string(rand_int)+".json"));
    //SaveJson(document, QString::fromStdString("/work/bragesk/git/PCG/PSO_test/TrajectoryTest_arch_0/fo_input/src_python/from_fieldopt_to_python_"+to_string(rand_int)+".json"));
    string string_syscall = "python3.8 src_python/ResDisp_apply.py "+to_string(rand_int);
    //string string_syscall = "python3 /work/bragesk/git/PCG/PSO_test/TrajectoryTest_arch_0/fo_input/src_python/Runner.py "+to_string(rand_int);
    cout << string_syscall << endl;
    cout << toe_->y->value() << endl;
    cout << toe_->x->value() << endl;
    cout << heel_->y->value() << endl;
    cout << heel_->x->value() << endl;
    system(string_syscall.c_str());
    auto d = LoadJson(QString::fromStdString("src_python/current_well_for_fieldopt_"+to_string(rand_int)+".json"));
    //auto d = LoadJson(QString::fromStdString("/work/bragesk/git/PCG/PSO_test/TrajectoryTest_arch_0/fo_input/src_python/current_well_for_fieldopt_"+to_string(rand_int)+".json"));
    cout << "Done with Python" << endl;
    auto custom_trajectory = new QJsonObject(d.object());
    auto custom_spline_points = QList<CustomSplinePoint *>();
    try {
        auto json_comp = custom_trajectory->value("trajectory_xyz").toObject();
        for (int i = 0; i < json_comp.size(); i++){
            auto custom_point = new CustomSplinePoint();
            auto raw_call = "point_number" + to_string(i);
            auto querry = QString::fromStdString(raw_call);
            auto from_point_array = json_comp[querry].toArray();
            custom_point->x = from_point_array.at(0).toDouble();
            custom_point->y = from_point_array.at(1).toDouble();
            custom_point->z = from_point_array.at(2).toDouble();
            custom_spline_points.push_back(custom_point);
        }
    }
    catch (std::exception const &ex) {
        //throw UnableToParseOptimizerSectionException("Unable to parse driver file optimizer section: " + std::string(ex.what()));
    }


    Curve *curve = new Bezier();
    curve->set_steps(50);
    for (int j = 0; j < custom_spline_points.size(); ++j) {
        curve->add_way_point(Vector(custom_spline_points[j]->x, custom_spline_points[j]->y, custom_spline_points[j]->z));
    }
    vector<Eigen::Vector3d> bezier_points;
    for (int i = 0; i < curve->node_count(); ++i) {
        bezier_points.push_back(Eigen::Vector3d(curve->node(i).x, curve->node(i).y, curve->node(i).z));
    }
    delete curve;
    return bezier_points;

}

}
}
}
