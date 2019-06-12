/******************************************************************************
   Created by einar on 6/11/19.
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

#ifndef JSON_RESULTS_H
#define JSON_RESULTS_H

#include "string"
#include "vector"
#include "map"

namespace Simulation {
namespace Results {

/*!
 * @brief Class for reading results from a JSON file
 * containing additional result components, including single
 * values, monthly vectors and yearly vectors.
 *
 * The file should be structured as follows:
 *
 * {
 *     "Components": [
 *          {
 *              "Name": "WaterfrontPenalty",
 *              "Type": "Single",
 *              "Value": 5.0
 *          },
 *          {
 *              "Name": "ReservoirPressure",
 *              "Type": "Monthly",
 *              "Values": [1.0, 2.0, 3.0, 4.0, 5.0, 6.0]
 *          },
 *          {
 *              "Name": "RecoveryFactor",
 *              "Type": "Yearly",
 *              "Values": [1.0, 2.0]
 *          }
 *     ]
 * }
 */
class JsonResults {
    public:
     JsonResults(){}
     JsonResults(std::string file_path);

     double GetSingleValue(std::string name);
     std::vector<double> GetMonthlyValues(std::string name);
     std::vector<double> GetYearlyValues(std::string name);

    private:
     std::map<std::string, double> singles_;
     std::map<std::string, std::vector<double>> monthlies_;
     std::map<std::string, std::vector<double>> yearlies_;
};

}
}

#endif // JSON_RESULTS_H
