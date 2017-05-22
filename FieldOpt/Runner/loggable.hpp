/******************************************************************************
   Created by einar on 4/3/17.
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
#ifndef FIELDOPT_LOGGABLE_H
#define FIELDOPT_LOGGABLE_H

#include <string>
#include <map>

using namespace std;

/*!
 * @brief The abstract Loggable class specifies the methods that must
 * be implemented by all classes that are to be logged by the Logger class.
 */
class Loggable {

 public:

  /*!
   * @brief Determines in which file the log entry will be written.
   *
   * LOG_CASE - The case log (log_cases.csv)
   * LOG_OPTIMIZER - The optimizer log (log_optimization.csv)
   * LOG_EXTENDED - The extended log (log_extended.json)
   * LOG_SUMMARY - Markdown-formatted summaries printed at the beginning and the end (summary_(pre/post)run.md)
   * STATE_RUNNER - A temporary log for debugging purposes. This log is frequently deleted as it only descibes the current state.
   */
  enum LogTarget {LOG_CASE, LOG_OPTIMIZER, LOG_EXTENDED, LOG_SUMMARY, STATE_RUNNER};

  /*!
   * @brief Get the target log for the object. E.g. if LOG_CASE is returned,
   * an entry will be written to the case log when NotifyLogger is called.
   * @return The target log.
   */
  virtual LogTarget GetLogTarget() = 0;

  /*!
   * @brief Get a string-string map describing the state of the loggable object.
   *
   * The keys should be the corresponding column headers (or JSON keys) in
   * the log, an the value should be the four-character code, e.g.
   *
   *    "EvalSt": "OKAY"
   *
   * Whitespace padding will be added by the logger.
   *
   * @return A string-string map of column headers and state indicators.
   */
  virtual map<string, string> GetState() { return map<string, string>(); }

  /*!
   * @brief Get the UUID of the object to be written to the log.
   */
  virtual QUuid GetId() = 0;

  /*!
   * @brief Get a string-vector map containing values to be logged.
   *
   * The keys should be the column headers (or JSON keys) in the corresponding log,
   * and the values should be vectors containing the value(s) to be logged.
   *
   * For case logging, string should be the OFnVal vector should be of length one
   * and contain the objective function value; it should also contain the evaluation
   * duration in second, e.g.
   *    "OFnVal": [1422.0]
   *    "SimDur": [122.1]
   * The logger will take care of converting the UUID to the name of the varible.
   *
   * For production data logs, the key should be the name of the vector, prepended
   * with "Res#" and the vector should contain the production data, e.g.
   *    "Res#CumulativeOilProductionTotal": [0.0, 200.0, 240.0]
   *
   * For variable values (from the Model to the extended log), the key should be
   * the variable name prepended with "Var#", and the value should be a vector of
   * length one containing the variable value:
   *    "Var#PROD#BHP#1": [204.1]
   * @return A string-vector map of values.
   */
  virtual map<string, vector<double>> GetValues() { return map<string, vector<double>>(); }

  /*!
   * @brief Description of a well spline used for summaries.
   */
  struct SplineDescription {
    bool is_variable;
    double heel_x;
    double heel_y;
    double heel_z;
    double toe_x;
    double toe_y;
    double toe_z;
  };

  /*!
   * @brief Description of a well control used for summaries.
   */
  struct ControlDescription {
    int time_step;
    string state;
    string control;
    double value;
  };

  /*!
   * @brief Description of a well used by summaries.
   */
  struct WellDescription {
    string name;
    string group;
    string type;
    string def_type;
    string pref_phase;
    string wellbore_radius;
    SplineDescription spline;
    vector<ControlDescription> controls;
  };

  /*!
   * @brief Get well descriptions. Used in summaries.
   */
  virtual map<string, WellDescription> GetWellDescriptions() { return map<string, WellDescription>(); }

};

#endif //FIELDOPT_LOGGABLE_H
