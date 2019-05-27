/******************************************************************************
   Created by einar on 5/2/19.
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
#ifndef FIELDOPT_ACTIONX_H
#define FIELDOPT_ACTIONX_H

#include <map>
#include <sstream>

namespace Simulation {
namespace ECLDriverParts {
namespace ActionX {

/*!
 * @brief This file implements functions needed for the ACTIONX keyword for the ECLIPSE 100 reservoir simulator.
 *
 * The ACTIONX keyword may be used to "store" schedule actions for later use, when some
 * condition is met. E.g. it may be used to alter valve sizes at certain times without
 * having to place the keyword in the chronologically correct place in the schedule.
 *
 * For now, this is limited to actions that are not repeated.
 *
 * Example:
 *
 * ACTIONX
 * --  Action name   Nr. of allowed repeats  Minimum interval between repeats
 *     ACT_NAME                1                          0   /
 * --  Condition: when simulation time is at (or past) day 0
 *     DAY  >= 0 /
 * /
 *
 * WSEGVALV
 *     PROD-3 21 0.7 8.54E-5 /
 * /
 *
 * ENDACTIO
 */

enum ACTX_LHQuantity { Day, Year };
enum ACTX_Operator { EQ, GT, LT, GE, LE };

std::map<ACTX_LHQuantity, std::string> LHQuantityString = {
    std::pair<ACTX_LHQuantity, std::string> {Day, "DAY"},
    std::pair<ACTX_LHQuantity, std::string> {Year, "YEAR"}
};
std::map<ACTX_Operator, std::string> OperatorString = {
    std::pair<ACTX_Operator, std::string> {EQ, "="},
    std::pair<ACTX_Operator, std::string> {GT, ">"},
    std::pair<ACTX_Operator, std::string> {LT, "<"},
    std::pair<ACTX_Operator, std::string> {GE, ">="},
    std::pair<ACTX_Operator, std::string> {LE, "<="}
};


/*!
 * @brief Generate teh ACTIONX keyword string with the provided action properly placed inside it.
 * @param name Name of action (must be unique).
 * @param lhq Left-hand quantity for condition.
 * @param op Operator for condition.
 * @param rhq Right-hand quantity for condition.
 * @param actions String containing the actions to be placed inside the action block.
 */
inline std::string ACTIONX(std::string name, ACTX_LHQuantity lhq, ACTX_Operator op, double rhq, std::string actions) {
  std::stringstream actionx;
  actionx << "ACTIONX" << std::endl;
  actionx << "    " << name << "  1  0  /" << std::endl;
  actionx << "    " << LHQuantityString[lhq] << "  " << OperatorString[op] << "  " << rhq << "  /" << std::endl;
  actionx << "/" << std::endl;
  actionx << std::endl;
  actionx << actions << std::endl;
  actionx << "ENDACTIO" << std::endl;
  return actionx.str();
}


}
}
}

#endif //FIELDOPT_ACTIONX_H
