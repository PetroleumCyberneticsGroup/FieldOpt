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

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "ensemble.h"
#include "Utilities/filehandling.hpp"
namespace Settings {

using namespace Utilities::FileHandling;

Settings::Ensemble::Ensemble(std::string ens_path)
    : ensemble_parent_dir_(GetParentDirectoryPath(ens_path))
{
    assert(FileExists(ens_path, true));
    assert(DirectoryExists(Ensemble::ensemble_parent_dir_, true));

    std::vector<std::string> file_contents = ReadFileToStdStringList(ens_path);
    for (auto line : file_contents) {
        if (line[0] == '#') {
            std::cout << "Skipped comment" << std::endl;
            continue;
        }
        std::vector<std::string> entries;
        boost::split(entries,line,boost::is_any_of("\t"));
        assert(entries.size() == 3);

        std::string alias = entries[0];
        std::string data = entries[1];
        std::string schedule = entries[2];

        assert(realizations_.count(entries[0]) == 0); // Check that the alias has not already been used.
        assert(FileExists(ensemble_parent_dir_ + "/" + data, true));
        assert(FileExists(ensemble_parent_dir_ + "/" + GetParentDirectoryPath(data) + "/" + schedule, true));
        realizations_.insert(
            std::pair<std::string, Realization>( alias, Realization(alias, data, data) )
        );
    }
}
Ensemble::Realization const &Ensemble::GetRealization(const std::string &alias) const {
    return realizations_.at(alias);
}
std::vector<std::string> Ensemble::GetAliases() const {
    std::vector<std::string> keys;
    for (auto realization : realizations_) {
        keys.push_back(realization.first);
    }
    return keys;
}

Settings::Ensemble::Realization::Realization(std::string alias,
                                             std::string data_rel_path,
                                             std::string schedule_rel_path)
    : alias_(alias),
      data_rel_path_(data_rel_path),
      schedule_rel_path_(schedule_rel_path)
{
}

std::string Ensemble::Realization::alias() const {
    return alias_;
}

std::string Ensemble::Realization::data() const {
    return data_rel_path_;
}

std::string Ensemble::Realization::schedule() const {
    return schedule_rel_path_;
}

}
