/******************************************************************************
 *
 * eclsummaryreader.h
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

#ifndef ECLSUMMARYREADER_H
#define ECLSUMMARYREADER_H

#include <ert/ecl/ecl_sum.h>
#include <QString>

namespace ERTWrapper {
namespace ECLSummary {


class ECLSummaryReader
{
public:
    ECLSummaryReader(QString file_name);
    ~ECLSummaryReader();

    double GetMiscVar(QString var_name, int time_index);

    int GetLastReportStep();
    int GetFirstReportStep();

private:
    const QString file_name_;
    ecl_sum_type *ecl_sum_;

    bool hasReportStep(int report_step);

    bool hasWellVar(QString well_name, QString var_name);
    bool hasGroupVar(QString group_name, QString var_name);
    bool hasFieldVar(QString var_name);
    bool hasBlockVar(int block_nr, QString var_name);
    bool hasMiscVar(QString var_name);
};

}
}

#endif // ECLSUMMARYREADER_H
