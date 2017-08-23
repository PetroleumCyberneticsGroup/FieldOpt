/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#ifndef LOGGER_H
#define LOGGER_H

#include "string"
#include "map"
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QUuid>
#include "Optimization/case.h"
#include "Optimization/optimizer.h"
#include "runtime_settings.h"
#include "Settings/settings.h"
#include "Model/model.h"
#include "Simulation/results/results.h"
#include "loggable.hpp"

using namespace std;


/*!
 * \brief The Logger class is responsible for writing CSV and JSON logs to the disk.
 *
 * The logger will write data into up to three files:
 *
 * LOG_CASE - The case log (log_cases.csv). Information about the generated cases.
 * LOG_OPTIMIZER - The optimizer log (log_optimization.csv). Information about the
 *  optmizer and runner states at each iteration.
 * LOG_EXTENDED - The extended log (log_extended.json). JSON log containing extended
 *  information, such as variable values, simulated production results and calculated
 *  compdats.
 *
 * In addition to these, two markdown-formatted summary logs (summary_prerun.md and
 * summary_postrun) will be written at the start and at the end of the run.
 *
 * Finally, files indicating the current state of each worker will be written when
 * running in parallel (state_runner.txt).
 */
class Logger
{
 public:
  /*!
   * \brief Logger
   * \param rts Runtime settings
   * \param output_subdir Optional subdirectory in the output directory to write the logs in.
   * \param write_logs Whether or not the logs should be written. This setting is mainly here for tests.
   */
  Logger(Runner::RuntimeSettings *rts, QString output_subdir="", bool write_logs=true);

  void AddEntry(Loggable *obj);
  void FinalizePrerunSummary();
  void FinalizePostrunSummary();

  /*!
   * @brief Collects extended logs from worker subdirs and writes them all
   * to a single JSON file in the root output dir.
   */
  void collectExtendedLogs();

 private:
  bool is_worker_; //!< Indicates whether or not this logger is on a worker process. This determines which logs are written.
  bool write_logs_;
  bool verbose_; //!< Whether or not new log entries should also be printed to the console.
  QString output_dir_; //!< Directory in which the files will be written.
  QString opt_log_path_; //!< Path to the optimization log file.
  QString cas_log_path_; //!< Path to the case log file.
  QString ext_log_path_; //!< Path to the extended log file.
  QString run_state_path_; //!< Path to the runner state file.
  QString summary_prerun_path_; //!< Path to the pre-run summary file.
  QString summary_postrun_path_; //!< Path to the pre-run summary file.

  map<string, vector<double>> sum_mod_valmap_; //!< Model summary value map.
  map<string, vector<double>> sum_opt_valmap_; //!< Optimizer summary value map.
  map<string, vector<double>> sum_rts_valmap_; //!< Runtime settings summary value map.
  map<string, string> sum_mod_statemap_; //!< Model summary state map.
  map<string, string> sum_opt_statemap_; //!< Optimizer summary state map.
  map<string, string> sum_rts_statemap_; //!< Runtime settings summary state map.
  map<string, Loggable::WellDescription> sum_wellmap_; //!< Model summary well map.

  /*!
   * @brief The column widths count from after the leading comma (if there is one) up to the
   * last letter in the name of the column. The trailing space and comma are added separately.
   */
  map<string, int> cas_log_col_widths_ {
      {"TimeSt", 19 },
      {"EvalSt", 7},
      {"ConsSt", 7},
      {"ErrMsg", 7},
      {"SimDur", 9},
      {"OFnVal", 12},
      {"CaseId", 39}
  };
  const QString cas_log_header_ = "             TimeSt , EvalSt , ConsSt , ErrMsg ,   SimDur ,   WicDur ,       OFnVal ,                                 CaseId";
  map<string, int> opt_log_col_widths_ {
      {"TimeSt", 19},
      {"TimeEl", 9},
      {"IterNr", 6},
      {"TotlNr", 6},
      {"EvalNr", 6},
      {"BkpdNr", 6},
      {"TimONr", 6},
      {"FailNr", 6},
      {"InvlNr", 6},
      {"CBOFnV", 12},
      {"CurBst", 41}
  };
  const QString opt_log_header_ = "             TimeSt ,   TimeEl ,   TimeIt , IterNr , TotlNr , EvalNr , BkpdNr , TimONr , FailNr , InvlNr ,       CBOFnV ,                                 CurBst";

  void logCase(Loggable *obj);
  void logOptimizer(Loggable *obj);
  void logExtended(Loggable *obj);
  void logSummary(Loggable *obj);
  void logRunnerState(Loggable *obj);

  /*!
   * @brief Append a well description to the summary.
   * @param w Well to append description of.
   * @param sum Summary string to append the description to.
   */
  void appendWellDescription(pair<string, Loggable::WellDescription> w, stringstream &sum);

  /*!
   * @brief Append a well "table of contents" to the summary string.
   * @param wellmap The map containing the well descriptions to be appended.
   * @param sum The summary to append to.
   */
  void appendWellToc(map<string, Loggable::WellDescription> wellmap, stringstream &sum);


};

#endif // LOGGER_H
