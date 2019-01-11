/******************************************************************************
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2018 Einar J.M. Baumann <einar.baumann@ntnu.no>
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

#ifndef FIELDOPT_REPORT_TUNING_H
#define FIELDOPT_REPORT_TUNING_H

#include <string>
#include <sstream>
#include <vector>
namespace Simulation {
namespace IXParts {

inline std::string FieldManagementStandardReport() {
    std::stringstream ss;
    ss << "FieldManagementStandardReport"          << std::endl;
    ss << "{"                                      << std::endl;
    ss << "    GroupReportVerbosity = 0"           << std::endl;
    ss << "    GroupReportFrequency = OFF"         << std::endl;
    ss << "    WellReportVerbosity = 0"            << std::endl;
    ss << "    WellReportFrequency = OFF"          << std::endl;
    ss << "    CompletionReportVerbosity = 0"      << std::endl;
    ss << "    CompletionReportFrequency = OFF"    << std::endl;
    ss << "    GasAccountingReportVerbosity = 0"   << std::endl;
    ss << "    GasAccountingReportFrequency = OFF" << std::endl;
    ss << "    DeviceReportVerbosity = 0"          << std::endl;
    ss << "    DeviceReportFrequency = OFF"        << std::endl;
    ss << "}"                                      << std::endl;
    return ss.str();
}

inline std::string EclReports() {
    std::stringstream ss;
    ss << "EclReports" << std::endl;
    ss << "{"                                      << std::endl;
    ss << "    MaxSegments = 200"                  << std::endl;
    ss << "}"                                      << std::endl;
    return ss.str();
}

inline std::string XYPlotSummaryReport(bool use_segments=false, const std::vector<int> segments=std::vector<int>()) {
    std::stringstream ss;
    ss << "XYPlotSummaryReport \"IXSummaryReport\" {"                                     << std::endl;
    ss << "    On=TRUE"                                                                   <<  std::endl;
    ss << "    Units=ECLIPSE_METRIC"                                                      << std::endl;
    ss << "    FileFormat=BINARY"                                                         << std::endl;
    ss << "    Frequency=TARGET_TIMES"                                                    << std::endl;
    ss << "    Unified=TRUE"                                                              << std::endl;
    ss << "    FileName=SUMMARYVECS"                                                      << std::endl;
    ss << "    OutputRSM=FALSE"                                                           << std::endl;
    ss << "    WellProperties \"AllWells\" {"                                             << std::endl;
    ss << "        WellNames=[\"*\"]"                                                     << std::endl;
    ss << "        add_property( property_name=\"OIL_PRODUCTION_RATE\""                   << std::endl;
    ss << "                      report_label=\"WOPR\" )"                                 << std::endl;
    ss << "        add_property( property_name=\"WATER_PRODUCTION_RATE\""                 << std::endl;
    ss << "                      report_label=\"WWPR\" )"                                 << std::endl;
    ss << "        add_property( property_name=\"GAS_PRODUCTION_RATE\""                   << std::endl;
    ss << "                      report_label=\"WGPR\" )"                                 << std::endl;
    ss << "        add_property( property_name=\"WATER_INJECTION_RATE\""                  << std::endl;
    ss << "                      report_label=\"WWIR\" )"                                 << std::endl;
    ss << "        add_property( property_name=\"GAS_INJECTION_RATE\""                    << std::endl;
    ss << "                      report_label=\"WGIR\" )"                                 << std::endl;
    ss << "        add_property ( property_name=\"LIQUID_PRODUCTION_RATE\""               << std::endl;
    ss << "                       report_label=\"WLPR\""                                  << std::endl;
    ss << "    }"                                                                         << std::endl;
    ss << "    FieldProperties \"Reservoir\" {"                                           << std::endl;
    ss << "        add_property( property_name=\"OIL_PRODUCTION_RATE\""                   << std::endl;
    ss << "                      report_label=\"FOPR\" )"                                 << std::endl;
    ss << "        add_property( property_name=\"WATER_PRODUCTION_RATE\""                 << std::endl;
    ss << "                      report_label=\"FWPR\" )"                                 << std::endl;
    ss << "        add_property( property_name=\"GAS_PRODUCTION_RATE\""                   << std::endl;
    ss << "                      report_label=\"FGPR\" )"                                 << std::endl;
    ss << "        add_property( property_name=\"WATER_INJECTION_RATE\""                  << std::endl;
    ss << "                      report_label=\"FWIR\" )"                                 << std::endl;
    ss << "        add_property( property_name=\"GAS_INJECTION_RATE\""                    << std::endl;
    ss << "                      report_label=\"FGIR\" )"                                 << std::endl;
    ss << "        add_property( property_name=\"OIL_PRODUCTION_CUML\""                   << std::endl;
    ss << "                      report_label=\"FOPT\" )"                                 << std::endl;
    ss << "        add_property( property_name=\"WATER_PRODUCTION_CUML\""                 << std::endl;
    ss << "                      report_label=\"FWPT\" )"                                 << std::endl;
    ss << "        add_property( property_name=\"GAS_PRODUCTION_CUML\""                   << std::endl;
    ss << "                      report_label=\"FGPT\" )"                                 << std::endl;
    ss << "        add_property( property_name=\"WATER_INJECTION_CUML\""                  << std::endl;
    ss << "                      report_label=\"FWIT\" )"                                 << std::endl;
    ss << "        add_property( property_name=\"GAS_INJECTION_CUML\""                    << std::endl;
    ss << "                      report_label=\"FGIT\" )"                                 << std::endl;
    ss << "        add_property ( property_name=\"LIQUID_PRODUCTION_RATE\""               << std::endl;
    ss << "                       report_label=\"FLPR\""                                  << std::endl;
    ss << "        add_property ( property_name=\"LIQUID_PRODUCTION_CUML\""               << std::endl;
    ss << "                       report_label=\"FLPT\""                                  << std::endl;
    ss << "    }"                                                                         << std::endl;
    if (use_segments) {
        ss << "    SegmentProperties \"Segments\" {"                                      << std::endl;
        ss << "        WellNames=[\"*\"]"                                                 << std::endl;
        ss << "        Segments=[ ";
        for (auto seg : segments) {
            ss << seg << " ";
        }
        ss << "]"                                                                             << std::endl;
        ss << "        add_property(property_name=\"OIL_FLOW_RATE\" report_label=\"SOFR\")"   << std::endl;
        ss << "        add_property(property_name=\"WATER_FLOW_RATE\" report_label=\"SWFR\")" << std::endl;
        ss << "    }" << std::endl;
    }
    ss << "}" << std::endl;
    return ss.str();

}


}
}

#endif //FIELDOPT_REPORT_TUNING_H
