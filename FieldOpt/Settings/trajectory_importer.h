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

#ifndef FIELDOPT_TRAJECTORY_IMPORTER_H
#define FIELDOPT_TRAJECTORY_IMPORTER_H

#include <string>
#include <map>
#include <vector>
#include <Eigen/Core>

namespace Settings {

/*!
 * The TrajectoryImporter class implements importing of pre-computed
 * well trajectories.
 *
 * The trajectory files are expected to be found in a directory named
 * 'trajectories' on the same level as the simulator driver file;
 * the 'trajectories' directory is expected to contain one file per
 * trajectory we want to import, and the file should have the same
 * name as the well it represents. The file is also expected to have
 * one header line (which will be ignored).
 *
 * The file should be in the CSV format, and should have one row
 * per intersected grid block. Each line should have (in this order)
 * the I, J and K indices for the block; the X, Y, Z coordinates
 * for the intersection point _into_ the grid block; and the X, Y, Z
 * coordinates for the intersection _out of_ the grid block.
 *
 * Example: Importing the trajectory for the D-2H well for the
 * NORNE_SIMPLIFIED deck.
 * Directory structure:
 *
 * norne-simplified/
 *      NORNE_SIMPLIFIED.DATA
 *      trajectories/
 *          D-2H
 *
 * File contents:
 *      I,	J,	K,  INX,	INY,	        INZ,	        OUTX,	        OUTY,	    OUTZ
 *      13,	11,	21,	455681,	7.32052e+06,	2766.9455681,	7.32052e+06,	2766.9
 *      13,	12,	21,	455761,	7.32058e+06,	2754.84455790,	7.3206e+06, 	2750.41
 *      13,	12,	20,	455790,	7.3206e+06,	    2750.4455838,	7.32064e+06,	2743.32
 *
 */
class TrajectoryImporter {
 public:

  /*!
   * Default constructor.
   * @param traj_dir_path Path to the 'trajectories' directory.
   * @param wells List of wells to be imported.
   */
  TrajectoryImporter(const std::string &traj_dir_path, const std::vector<std::string> &wells);

  TrajectoryImporter();

  struct ImportedWellBlock {
    ImportedWellBlock(const int i, const int j, const int k,
                      const double in_x,  const double in_y,  const double in_z,
                      const double out_x, const double out_y, const double out_z);

    Eigen::Vector3i ijk() const;
    Eigen::Vector3d in() const;
    Eigen::Vector3d out() const;

   private:
    int i_, j_, k_;
    double in_x_, in_y_, in_z_, out_x_, out_y_, out_z_;
  };

  /*!
   * Get the trajectory for an imported well.
   * @param well_name Name of well to import.
   * @return Vector containing the imported well blocks.
   */
  std::vector<ImportedWellBlock> GetImportedTrajectory(const std::string well_name) const;

  bool ContainsTrajectory(const std::string well_name) const;

 private:
  std::vector<std::string> well_names_; //!< Vector containing the names of wells to be imported.
  std::string traj_dir_path_; //!< Absolute path to the trajectories directory.
  std::map<std::string, std::string> traj_file_paths_; //!< Map from well name to absolute path of trajectory file.
  std::map<std::string, std::vector<ImportedWellBlock> > imported_trajectories_; //!< Map from well name to imported trajectory.

  /*!
   * Build the traj_file_paths_ map by searching the directory at traj_dir_path_
   * for files containing the trajectories for the wells in well_names_.
   */
  void findTrajectoryFiles();

  /*!
   * Parse a trajectory file.
   * @param well_name Well name. Used to find file path in the traj_file_paths_ map.
   * @return A vector of ImportedWellBlock objects, representing the imported trajectory.
   */
  std::vector<ImportedWellBlock> parseFile(const std::string &well_name);

  /*!
   * Parse one line from a csv file to an ImportedWellBlock object.
   * @param csv_line
   * @return An ImportedWellBlock object, representing the well's path
   * through one well block.
   */
  ImportedWellBlock parseLine(const std::string &line);

};

}

#endif //FIELDOPT_TRAJECTORY_IMPORTER_H
