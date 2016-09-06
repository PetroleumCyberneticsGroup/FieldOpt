/*!
 * @brief This file contains helper methods for the main.cpp file in this folder.
 *
 * It contains methods to parse the program arguments, as well as well as methods to
 * print the output in various formats.
 */

#ifndef WIC_MAIN_H
#define WIC_MAIN_H

#include "intersected_cell.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <stdlib.h>
#include "Utilities/file_handling/filehandling.hpp"

namespace po = boost::program_options;
using namespace Reservoir::WellIndexCalculation;

void printCsv(QList<IntersectedCell> &well_blocks) {
    std::cout << "i,\tj,\tk,\twi" << std::endl;
    for (auto block : well_blocks) {
        auto line = QString("%1,\t%2,\t%3,\t%4")
                .arg(block.ijk_index().i() + 1)         // %1
                .arg(block.ijk_index().j() + 1)         // %2
                .arg(block.ijk_index().k() + 1)         // %3
                .arg(block.well_index()).toStdString(); // %4
        std::cout << line << std::endl;
    }
}

void printCompdat(QList<IntersectedCell> &well_blocs, QString well_name, double wellbore_radius) {
    QString head = "COMPDAT\n";
    QString foot = "\n/";
    QStringList body;
    for (auto block : well_blocs) {
        //                       NAME I   J  K1  K2  OP/SH ST WI  RAD
        auto entry = QString("   %1  %2  %3  %4  %4  OPEN  1  %5  %6")
                .arg(well_name)             // %1
                .arg(block.ijk_index().i() + 1) // %2
                .arg(block.ijk_index().j() + 1) // %3
                .arg(block.ijk_index().k() + 1) // %4
                .arg(block.well_index())        // %5
                .arg(wellbore_radius);          // %6
        body.append(entry);
    }
    std::string full = (head + body.join("\n") + foot).toStdString();
    std::cout << full << std::endl;
}

po::variables_map createVariablesMap(int argc, const char **argv) {
    //
    // This function parses the runtime arguments and creates a boost::program_options::variable_map from them.
    // It also displays help if the --help flag is passed.
    //
    po::options_description desc("FieldOpt options");
    desc.add_options()
            ("help", "print help message")
            ("grid,g", po::value<std::string>(),
             "path to model grid file (e.g. *.GRID)")
            ("heel,h", po::value<std::vector<double>>()->multitoken(),
             "Heel coordinates (x y z)")
            ("toe,t", po::value<std::vector<double>>()->multitoken(),
             "Toe coordinates (x y z)")
            ("radius,r", po::value<double>(),
             "wellbore radius")
            ("compdat,c", po::value<int>()->implicit_value(0),
             "print in compdat format instead of CSV")
            ("well-name,w", po::value<std::string>(),
             "well name to be used when writing compdat")
            ;
    // Positional arguments
    po::positional_options_description p;
    p.add("grid", 1);

    // Process arguments to variable map
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
            options(desc).positional(p).run(), vm);
    po::notify(vm);

    // If called with --help or -h flag:
    if (vm.count("help")) { // Print help if --help present or input file/output dir not present
        std::cout << "Usage: ./WellIndexCalculator gridpath --heel x1 y1 z1 --toe x2 y2 z2 --radius r [options]" << std::endl;
        std::cout << desc << std::endl;
        std::exit(EXIT_SUCCESS);
    }

    assert(vm.count("grid"));
    assert(vm.count("heel"));
    assert(vm.count("toe"));
    assert(vm.count("radius"));
    if (vm.count("compdat"))
        assert(vm.count("well-name"));
    assert(vm["heel"].as<std::vector<double>>().size() == 3);
    assert(vm["toe"].as<std::vector<double>>().size() == 3);
    assert(Utilities::FileHandling::FileExists(QString::fromStdString(vm["grid"].as<std::string>())));
    assert(vm["radius"].as<double>() > 0);

    return vm;
}

#endif // WIC_MAIN_H
