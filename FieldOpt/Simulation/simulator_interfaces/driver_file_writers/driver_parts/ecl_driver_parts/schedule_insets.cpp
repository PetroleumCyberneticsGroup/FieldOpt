/******************************************************************************
   Copyright (C) 2019 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "schedule_insets.h"
#include "Utilities/filehandling.hpp"
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <boost/lexical_cast.hpp>

namespace Simulation {
namespace ECLDriverParts {

ScheduleInsets::ScheduleInsets() {

}

ScheduleInsets::ScheduleInsets(const std::string &inset_file_path) {
    assert(Utilities::FileHandling::FileExists(inset_file_path, true));
    std::vector<std::string> all_lines = Utilities::FileHandling::ReadFileToStdStringList(inset_file_path);
    std::deque<std::string> lines = trimEmptyLines(all_lines);

    while (lines.size() > 0 && isKeyword(lines[0])) {
        insets_.insert(parseNextInset(lines));
    }
}

bool ScheduleInsets::HasInset(const int &time) const {
    if (insets_.count(time) > 0) {
        return true;
    }
    else {
        return false;
    }
}

std::string ScheduleInsets::GetInset(const int &time) const {
    if (HasInset(time)) {
        return insets_.at(time);
    }
    else {
        return "";
    }
}

std::pair<int, std::string> ScheduleInsets::parseNextInset(std::deque<std::string> &lines) const {
    std::string kw_line = lines[0]; // First line is on the form INSET=N
    lines.pop_front(); // Remove first keyword line

    // Split kw string and get the time of insertion
    std::vector<std::string> kw_parts;
    boost::split(kw_parts, kw_line, boost::is_any_of("="));
    assert(kw_parts.size() == 2);
    int time = boost::lexical_cast<int>(kw_parts.back());

    // Loop through the lines until another keyword is encountered, adding each line
    // to a stringstream and popping the first string in the list.
    std::stringstream inset;
    inset << "-- START FIELDOPT INSET AT DAY " << time << " ~~~~~~~~~~~~~~~~\n";
    while (!isKeyword(lines[0])) {
        inset << lines[0] << "\n";
        lines.pop_front();
    }
    inset << "-- END FIELDOPT INSET AT DAY " << time << " ~~~~~~~~~~~~~~~~\n\n";
    if (lines[0] == "END_INSET") { // Pop the first element if it is the END_INSET keyword
        lines.pop_front();
    }

    return std::pair<int, std::string> (time, inset.str());
}

std::deque<std::string> ScheduleInsets::trimEmptyLines(const std::vector<std::string> &lines) const {
    std::deque<std::string> trimmed_strings;
    for (std::string line : lines) {
        std::string trimmed_line = boost::trim_copy(line);
        if (trimmed_line.size() > 0) {
            if (isKeyword(line)) {
                trimmed_strings.push_back(trimmed_line);
            }
            else {
                trimmed_strings.push_back(boost::trim_right_copy(line));
            }
        }
    }
    return trimmed_strings;
}

bool ScheduleInsets::isKeyword(const std::string line) const {
    std::string trimmed_line = boost::trim_copy(line);
    if (trimmed_line.compare(0, 5, "INSET") == 0 || trimmed_line.compare("END_INSET") == 0) {
        return true;
    }
    else {
        return false;
    }
}

}
}
