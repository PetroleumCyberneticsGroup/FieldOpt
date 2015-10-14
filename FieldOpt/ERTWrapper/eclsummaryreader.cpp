/******************************************************************************
 *
 * eclsummaryreader.cpp
 *
 * Created: 07.10.2015 2015 by einar
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

#include "eclsummaryreader.h"
#include "ertwrapper_exceptions.h"
#include "string"
#include <iostream>

namespace ERTWrapper {
namespace ECLSummary {

ECLSummaryReader::ECLSummaryReader(QString file_name)
{
    file_name_ = file_name;
    ecl_sum_ = ecl_sum_fread_alloc_case(file_name_.toLatin1().constData(), "");
    if (ecl_sum_ == NULL) throw SummaryFileNotFoundAtPathException(file_name.toLatin1().constData());
}

ECLSummaryReader::~ECLSummaryReader()
{
    ecl_sum_free(ecl_sum_);
}

double ECLSummaryReader::GetMiscVar(QString var_name, int time_index)
{
    if (!hasMiscVar(var_name))
        throw SummaryVariableDoesNotExistException("Misc variable " + std::string(var_name.toLatin1().constData()) + " does not exist.");
    if (!HasReportStep(time_index))
        throw SummaryTimeStepDoesNotExistException("Time step does not exist");
    return ecl_sum_get_misc_var(ecl_sum_, time_index, var_name.toLatin1().constData());
}

double ECLSummaryReader::GetFieldVar(QString var_name, int time_index)
{
    if (!hasFieldVar(var_name))
        throw SummaryVariableDoesNotExistException("Field variable " + std::string(var_name.toLatin1().constData()) + " does not exist.");
    if (!HasReportStep(time_index))
        throw SummaryTimeStepDoesNotExistException("Time step does not exist");
    return ecl_sum_get_field_var(ecl_sum_, time_index, var_name.toLatin1().constData());
}

double ECLSummaryReader::GetWellVar(QString well_name, QString var_name, int time_index)
{
    if (!hasWellVar(well_name, var_name))
        throw SummaryVariableDoesNotExistException("Well variable " + std::string(well_name.toLatin1().constData()) + ":"
                                                   + std::string(var_name.toLatin1().constData()) + " does not exist.");
    if (!HasReportStep(time_index))
        throw SummaryTimeStepDoesNotExistException("Time step does not exist");
    return ecl_sum_get_well_var(ecl_sum_, time_index, well_name.toLatin1().constData(), var_name.toLatin1().constData());
}

int ECLSummaryReader::GetLastReportStep()
{
    int last_step = ecl_sum_get_last_report_step(ecl_sum_);
    return ecl_sum_iget_report_end(ecl_sum_, last_step);
}

int ECLSummaryReader::GetFirstReportStep()
{
    int first_step = ecl_sum_get_first_report_step(ecl_sum_);
    return ecl_sum_iget_report_start(ecl_sum_, first_step);
}

bool ECLSummaryReader::HasReportStep(int report_step) {
    return report_step <= GetLastReportStep() && report_step >= GetFirstReportStep();
}

bool ECLSummaryReader::hasWellVar(QString well_name, QString var_name) {
    return ecl_sum_has_well_var(ecl_sum_,
                                well_name.toLatin1().constData(),
                                var_name.toLatin1().constData());
}

bool ECLSummaryReader::hasGroupVar(QString group_name, QString var_name) {
    return ecl_sum_has_group_var(ecl_sum_,
                                 group_name.toLatin1().constData(),
                                 var_name.toLatin1().constData());
}

bool ECLSummaryReader::hasFieldVar(QString var_name) {
    return ecl_sum_has_field_var(ecl_sum_,
                                 var_name.toLatin1().constData());
}

bool ECLSummaryReader::hasBlockVar(int block_nr, QString var_name) {
    return ecl_sum_has_block_var(ecl_sum_,
                                 var_name.toLatin1().constData(),
                                 block_nr);
}

bool ECLSummaryReader::hasMiscVar(QString var_name) {
    return ecl_sum_has_misc_var(ecl_sum_,
                                var_name.toLatin1().constData());
}

}
}
