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

#ifndef ECLRESULTS_H
#define ECLRESULTS_H

#include "results.h"
#include "ERTWrapper/eclsummaryreader.h"
#include <QHash>

namespace Simulation {
namespace Results {

/*!
 * \brief The ECLResults class uses the ECLSummaryReader class in the ERTWrapper library
 * to read properties from ECLIPSE summary files.
 */
class ECLResults : public Results
{
public:
    ECLResults();

    void ReadResults(QString file_path);
    void DumpResults();


    double GetValue(Property prop);
    double GetValue(Property prop, int time_index);
    double GetValue(Property prop, QString well);
    double GetValue(Property prop, QString well, int time_index);

private:
    QString file_path_;
    ERTWrapper::ECLSummary::ECLSummaryReader *summary_reader_;

    // Mappings to ECLIPSE summary keys.
    QHash<Property, QString> keys_ {
        {CumulativeOilProduction, "FOPT"},
        {CumulativeGasProduction, "FGPT"},
        {CumulativeWaterProduction, "FWPT"},
        {CumulativeWellOilProduction, "WOPT"},
        {CumulativeWellGasProduction, "WGPT"},
        {CumulativeWellWaterProduction, "WWPT"},
        {Time, "TIME"}
    };

    // Keys that are counted as Misc. by ECLIPSE.
    QList<Property> misc_keys_ {
        Time
    };
};

}
}

#endif // ECLRESULTS_H
