//
// Created by bellout on 7/30/16.
//

#include <gtest/gtest.h>
#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"
#include "Reservoir/grid/grid_exceptions.h"
#include "Utilities/file_handling/filehandling.h"
#include "Reservoir/well_index_calculation/wellindexcalculator.h"

#include "Reservoir/tests/test_resource_wic_welldir.h"
#include "Reservoir/tests/test_resource_wic_diff_functions.h"

#include <QList>
#include <QVector>
#include <QProcess>
#include <QTextStream>
#include <fstream>
#include <Eigen/Dense>

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <QTextCodec>

using namespace Reservoir::Grid;
using namespace Reservoir::WellIndexCalculation;
using namespace TestResources::WIBenchmark;

namespace {

    class DeviatedWellIndexTest : public ::testing::Test {

    protected:
        DeviatedWellIndexTest() {
            grid_ = new ECLGrid(file_path_);
            well_dir_ = new WellDir();
        }

        virtual ~DeviatedWellIndexTest() {
            delete grid_;
        }

        Grid *grid_;
        WellDir *well_dir_;
        QString file_path_ = "../examples/ADGPRS/5spot/ECL_5SPOT.EGRID";
        bool debug_ = false;
    };

    TEST_F(DeviatedWellIndexTest, test) {

    }

    TEST_F(DeviatedWellIndexTest, compareCOMPDAT) {

        // GET LIST OF WELL FOLDERS CONTAINING PCG & RMS COMPDATS
        auto file_list_ = well_dir_->GetWellDir();
        auto dir_names_ = file_list_[0]; // list of well dirs (names only) => dir name only: tw04_04
        auto dir_list_ = file_list_[1]; // list of well dirs (absolute path) => fullpath: ../tw04_04/
        auto rms_files = file_list_[2]; // fullpath: ../tw04_04/EVENTS_tw04_04_RMS.DATA
        auto pcg_files = file_list_[3]; // fullpath: ../tw04_04/EVENTS_tw04_04_PCG.DATA

        // WELL INDEX DATA OBJECTS
        WIData WIDataRMS, WIDataPCG;
        WIDataRMS.data_tag = "RMS";
        WIDataPCG.data_tag = "PCG";

        // DEBUG
        debug_msg(false, "well_dir_list", dir_names_, dir_list_, 0, WIDataRMS, WIDataPCG, 0);

        // LOOP THROUGH LIST OF WELL FOLDERS: FOR WELL FOLDER ii,
        // READ PCG & RMS COMPDAT DATA
        int num_files = (debug_) ? 5 : rms_files.length(); //override

        for (int ii = 0; ii < num_files; ++ii) {

            // READ COMPDAT + XYZ FILES
            WIDataRMS.ReadData(rms_files[ii],dir_names_[ii],dir_list_[ii]);
            WIDataPCG.ReadData(pcg_files[ii],dir_names_[ii],dir_list_[ii]);

            // MAKE NEW WIData OBJECT USING CURRENT WIC LIBRARIES
            

            // MESSAGE OUTPUT
            std::cout << "\n\n\033[1;36m" << std::setfill('=') << std::setw(80) << "=" << "\033[0m" << std::endl;
            std::cout << "\033[1;36mChecking IJK and WCF data for well: "
                      << WIDataRMS.dir_name.toStdString() << "\033[0m" << std::endl;
            std::cout << "\033[1;36m" << std::setfill('=') << std::setw(80) << "=" << "\033[0m" << std::endl;

            // DEBUG
            debug_msg(false, "RMS_PCG_IJK_data", dir_names_, dir_list_, ii, WIDataRMS, WIDataPCG, 0);

            // REMOVE ROW IF LOW WCF
            RemoveRowsLowWCF(WIDataRMS);
            RemoveRowsLowWCF(WIDataPCG);

            // PRINT IJK, WCF DATA TO INDIVIDUAL FILES TO TREAT WITH diff COMMAND LATER:
            // MAKE DIFF FILE NAMES
            QStringList diff_files = {
                    dir_list_[ii] + "/DIFF_" + dir_names_[ii] + "_RMS.IJK",
                    dir_list_[ii] + "/DIFF_" + dir_names_[ii] + "_PCG.IJK",
                    dir_list_[ii] + "/DIFF_" + dir_names_[ii] + "_RMS.WCF",
                    dir_list_[ii] + "/DIFF_" + dir_names_[ii] + "_PCG.WCF"
            };

            // PRINT DIFF FILES
            WIDataRMS.PrintIJKData(diff_files[0]);
            WIDataPCG.PrintIJKData(diff_files[1]);
            WIDataRMS.PrintWCFData(diff_files[2]);
            WIDataPCG.PrintWCFData(diff_files[3]);

            // REMOVE EXTRA ROWS IF DATA HAS UNEQUAL LENGTH
            if (DiffVectorLength(WIDataRMS, WIDataPCG)) {
                // IF VECTOR LENGTHS ARE EQUAL => COMPARE DIRECTLY
                std::cout << "\033[1;36m" << WIDataRMS.dir_name.toStdString() <<
                          ": >>> Vector lengths are equal. Making comparison.\033[0m" <<
                          std::setfill(' ') << std::endl;
            } else {
                // IF VECTOR LENGTHS ARE UNEQUAL => MAKE EQUAL, THEN COMPARE DIRECTLY
                std::cout << "\033[1;36m" << WIDataRMS.dir_name.toStdString() <<
                          ": >>> Vector lengths are unequal. Making them equal.\033[0m" << std::endl;
                RemoveSuperfluousRows(WIDataRMS, WIDataPCG, diff_files);
            }

            // COMPARE IJK AND PCG VALUES (EQUAL LENGTH DATA)
            CompareIJK(WIDataRMS, WIDataPCG);
            CompareWCF(WIDataRMS, WIDataPCG);
        }
    }
}