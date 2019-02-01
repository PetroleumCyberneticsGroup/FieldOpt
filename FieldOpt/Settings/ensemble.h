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

#ifndef FIELDOPT_ENSEMBLE_H
#define FIELDOPT_ENSEMBLE_H

#include <map>
#include "string"
#include <vector>

namespace Settings {

/*!
 * Class containing description of the deck layout
 * for an ensemble of reservoir realizations.
 */
class Ensemble {
 public:
  /*!
   * Create an ensemble object to be used when optimizing
   * multiple realizations by reading an ensemble
   * description file.
   *
   * The proveded ensemble description file is expected to
   * be a CSV file in the format
   *
   * \verbatim
   * Alias , RelativeDataPath , RelativeSchedulePath, RelativeGridPath
   * \endverbatim
   *
   * Lines starting with '#' will be interpreted as comment
   * lines (i.e. they will be ignored).
   *
   * The first entry on each line will be used as an Alias,
   * and will be used in the reports and internally. It
   * has to be unique for each realization.
   *
   * The second entry on each line should be the path to the
   * .DATA file for a specific realization. The path should be
   * _relative to the placement of the ensemble description
   * file_.
   *
   * The third entry on each line should be the path to the
   * schedule file for one specific realization (containing
   * at least the WELSPECS, COMPDAT and control keywords for
   * the variable wells, and _not_ containing the SHEDULE
   * keyword itself).
   *
   * The fourth entry should be the path to the .EGRID file
   * for the specific realization.
   *
   *
   *
   * @param ens_path
   */
  Ensemble(const std::string &ens_path);

  Ensemble();


  struct Realization {
    Realization() = delete;
    Realization(std::string alias, std::string data_rel_path,
                std::string schedule_rel_path, std::string grid_rel_path);

    std::string alias() const;
    std::string data() const;
    std::string schedule() const;
    std::string grid() const;

   private:
    const std::string alias_;
    const std::string data_rel_path_;
    const std::string schedule_rel_path_;
    const std::string grid_rel_path_;
  };

  const Realization &GetRealization(const std::string &alias) const;
  std::vector<std::string> GetAliases() const;

 private:
  std::string ensemble_parent_dir_;
  std::map<std::string, Realization> realizations_;


};

}

#endif //FIELDOPT_ENSEMBLE_H
