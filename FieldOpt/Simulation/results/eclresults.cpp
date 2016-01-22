/******************************************************************************
 *
 *
 *
 * Created: 12.10.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

#include "eclresults.h"
#include "results_exceptions.h"
#include "ERTWrapper/ertwrapper_exceptions.h"

namespace Simulation { namespace Results {

ECLResults::ECLResults()
    : Results()
{
    summary_reader_ = 0;
}

void ECLResults::ReadResults(QString file_path)
{
    file_path_ = file_path;
    if (summary_reader_ != 0) delete summary_reader_;
    try {
        summary_reader_ = new ERTWrapper::ECLSummary::ECLSummaryReader(file_path);
    }
    catch (ERTWrapper::SummaryFileNotFoundAtPathException) {
        throw ResultFileNotFoundException(file_path.toLatin1().constData());
    }

    setAvailable();
}

void ECLResults::DumpResults()
{
    if (summary_reader_ != 0) delete summary_reader_;
    summary_reader_ = 0;
    setUnavailable();
}

double ECLResults::GetValue(Results::Property prop)
{
    if (!isAvailable()) throw ResultsNotAvailableException();
    if (!keys_.contains(prop)) throw ResultPropertyKeyDoesNotExistException("ECLIPSE");

    int last_report_step = summary_reader_->GetLastReportStep();

    if (misc_keys_.contains(prop))
        return summary_reader_->GetMiscVar(keys_.value(prop), last_report_step);
    else
        return summary_reader_->GetFieldVar(keys_.value(prop), last_report_step);
}

double ECLResults::GetValue(Results::Property prop, int time_index)
{
    if (!isAvailable()) throw ResultsNotAvailableException();
    if (!keys_.contains(prop)) throw ResultPropertyKeyDoesNotExistException("ECLIPSE");
    if (!summary_reader_->HasReportStep(time_index)) throw ResultTimeIndexInvalidException(time_index);

    if (misc_keys_.contains(prop))
        return summary_reader_->GetMiscVar(keys_.value(prop), time_index);
    else
        return summary_reader_->GetFieldVar(keys_.value(prop), time_index);
}

double ECLResults::GetValue(Results::Property prop, QString well)
{
    if (!isAvailable()) throw ResultsNotAvailableException();
    if (!keys_.contains(prop)) throw ResultPropertyKeyDoesNotExistException("ECLIPSE");

    int last_report_step = summary_reader_->GetLastReportStep();
    return summary_reader_->GetWellVar(well, keys_.value(prop), last_report_step);
}

double ECLResults::GetValue(Results::Property prop, QString well, int time_index)
{
    if (!isAvailable()) throw ResultsNotAvailableException();
    if (!keys_.contains(prop)) throw ResultPropertyKeyDoesNotExistException("ECLIPSE");
    if (!summary_reader_->HasReportStep(time_index)) throw ResultTimeIndexInvalidException(time_index);
    return summary_reader_->GetWellVar(well, keys_.value(prop), time_index);
}

}}
