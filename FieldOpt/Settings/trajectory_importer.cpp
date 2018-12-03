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

#include <Utilities/filehandling.hpp>
#include "trajectory_importer.h"
#include <algorithm>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

using namespace Utilities::FileHandling;

namespace Settings {

TrajectoryImporter::TrajectoryImporter(const std::string &traj_dir_path,
                                       const std::vector<std::string> &wells) {
    std::cout << "Importing well trajectories ... " << std::endl;
    assert(DirectoryExists(traj_dir_path));
    traj_dir_path_ = GetAbsoluteFilePath(traj_dir_path);
    well_names_ = wells;
    findTrajectoryFiles();

    for (auto wname : well_names_) {
        imported_trajectories_.insert(std::pair<std::string, std::vector<ImportedWellBlock> >(
            wname, parseFile(wname)
        ));
    }
    std::cout << "Done importing well trajectories." << std::endl;
}
TrajectoryImporter::TrajectoryImporter() {}
std::vector<TrajectoryImporter::ImportedWellBlock> TrajectoryImporter::GetImportedTrajectory(const std::string well_name) const {
    return imported_trajectories_.at(well_name);
}
bool TrajectoryImporter::ContainsTrajectory(const std::string well_name) const {
    return std::find(well_names_.begin(), well_names_.end(), well_name) != well_names_.end();
}
void TrajectoryImporter::findTrajectoryFiles() {
    for (auto wname : well_names_) {
        std::string file_path = traj_dir_path_ + "/" + wname;
        assert(FileExists(file_path));
        traj_file_paths_.insert(std::pair<std::string, std::string>(wname, file_path));
    }
}
std::vector<TrajectoryImporter::ImportedWellBlock> TrajectoryImporter::parseFile(const std::string &well_name) {
    std::vector<std::string> file_contents = ReadFileToStdStringList(traj_file_paths_.at(well_name));
    file_contents.erase(file_contents.begin()); // Remove first line (header)
    std::vector<ImportedWellBlock> trajectory;
    for (auto line : file_contents) {
        trajectory.push_back(parseLine(line));
    }
    return trajectory;
}
TrajectoryImporter::ImportedWellBlock TrajectoryImporter::parseLine(const std::string &line) {
    std::vector<std::string> entries;
    boost::split(entries,line,boost::is_any_of(","));
    assert(entries.size() == 9);
    for (int i = 0; i < entries.size(); ++i) {
        boost::algorithm::trim(entries[i]);
    }

    auto i  = boost::lexical_cast<int>(entries[0]);
    auto j  = boost::lexical_cast<int>(entries[1]);
    auto k  = boost::lexical_cast<int>(entries[2]);
    auto ix = boost::lexical_cast<double>(entries[3]);
    auto iy = boost::lexical_cast<double>(entries[4]);
    auto iz = boost::lexical_cast<double>(entries[5]);
    auto ox = boost::lexical_cast<double>(entries[6]);
    auto oy = boost::lexical_cast<double>(entries[7]);
    auto oz = boost::lexical_cast<double>(entries[8]);

    return ImportedWellBlock(i, j, k,
                             ix, iy, iz,
                             ox, oy, oz
    );

}
TrajectoryImporter::ImportedWellBlock::ImportedWellBlock(const int i, const int j, const int k,
                                                         const double in_x,  const double in_y,  const double in_z,
                                                         const double out_x, const double out_y, const double out_z)
    : i_(i), j_(j), k_(k), in_x_(in_x), in_y_(in_y), in_z_(in_z), out_x_(out_x), out_y_(out_y), out_z_(out_z) {}
Eigen::Vector3i TrajectoryImporter::ImportedWellBlock::ijk() const {
    return Eigen::Vector3i(i_, j_, k_);
}
Eigen::Vector3d TrajectoryImporter::ImportedWellBlock::in() const {
    return Eigen::Vector3d(in_x_, in_y_, in_z_);
}
Eigen::Vector3d TrajectoryImporter::ImportedWellBlock::out() const {
    return Eigen::Vector3d(out_x_, out_y_, out_z_);
}

}
