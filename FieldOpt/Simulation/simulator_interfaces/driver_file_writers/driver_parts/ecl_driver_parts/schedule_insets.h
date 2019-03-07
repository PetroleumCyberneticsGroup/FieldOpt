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
#ifndef FIELDOPT_SCHEDULE_INSETS_H
#define FIELDOPT_SCHEDULE_INSETS_H

#include <string>
#include <map>
#include <vector>
#include <deque>

namespace Simulation {
namespace ECLDriverParts {

/*!
 * @brief The ScheduleInsets class parses a file describing code snippets to be inserted
 * at spcific times in the schedule and provides convenient access to these.
 *
 * The file describing the insets may have any name. The path to the file should be provided
 * when launching FieldOpt, using the --sim-inset flag.
 *
 * The file should be formatted as follows:
 *
 * @code
 * INSET=-1
 * -- Code inserted at the very beginning of the schedule file written by FieldOpt, before anything else
 * ECLCMD
 *  SOMETHING 5 /
 * END_INSET
 *
 * INSET=365
 * -- Code inserted after 365 days (must be a day in the control times array), after WELSPECS
 * OTHERCMD
 *  SHORTABBRV 69 /
 * END_INSET
 * @endcode
 *
 * Notice the first line: INSET=-1
 * - INSET is the keyword used to indicate the start of a new inset.
 * - INSET should be followed by an`=`, followed by the day at which the code should be inserted.
 * - INSET=-1 indicates that the snippet should be inserted at the very beginning of the schedule written by FieldOpt,
 *   before the first WELSPECS.
 * - INSET=N will insert the snippet after the WELSPECS keyword on day N.
 * - Inserts/snippets should be terminated/ended using the END_INSET keyword.
 * - The keywords are case sensitive.
 * - Everything between `INSET=N` and `END_INSET` will be inserted verbatim.
 *
 * **Note:** The inset time _must_ be a control time specified in the driver file.
 *
 * An example inset file is shown in examples/ECLIPSE/schedule_inset.txt
 */
class ScheduleInsets {

 public:
  /*!
   * @brief Constructor used when no inserts path is given.
   */
  ScheduleInsets();

  /*!
   * @brief Build the day-snippet mapping by parsing the file at the provided path.
   * @param inset_file_path Path to inset-file.
   */
  ScheduleInsets(const std::string &inset_file_path);

  /*!
   * @brief Check whether an inset is specified at a control time step.
   * @param time Time/day to check.
   * @return True if an inset is specified; otherwise false.
   */
  bool HasInset(const int &time) const;

  /*!
   * @brief Get the inset at the specified time.
   * @param time Time to get snippet for.
   * @return Snippet at specified time. If no snippet is specified, an empty string is returned.
   */
  std::string GetInset(const int &time) const;

 private:
  std::map<int, std::string> insets_;

  std::pair<int, std::string> parseNextInset(std::deque<std::string> &lines) const;
  std::deque<std::string> trimEmptyLines(const std::vector<std::string> &lines) const;
  bool isKeyword(const std::string line) const;

};

}
}
#endif //FIELDOPT_SCHEDULE_INSETS_H
