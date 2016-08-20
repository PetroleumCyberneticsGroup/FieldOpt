//
// Created by bellout on 7/30/16.
//

#include <gtest/gtest.h>
#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"
#include "Reservoir/grid/grid_exceptions.h"
#include "Utilities/file_handling/filehandling.h"
#include "Reservoir/well_index_calculation/wellindexcalculator.h"
#include "Reservoir/well_index_calculation/main.hpp"

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

        // GET LIST OF WELL FOLDERS CONTAINING PCG & RMS COMPDATS (OBTAINED USING WI_BENCHMARK CODE)
        auto file_list_ = well_dir_->GetWellDir();
        auto dir_names_ = file_list_[0]; // list of well dirs (names only) => dir name only: tw04_04
        auto dir_list_ = file_list_[1]; // list of well dirs (absolute path) => fullpath: ../tw04_04/
        auto rms_files = file_list_[2]; // fullpath: ../tw04_04/EVENTS_tw04_04_RMS.DATA
        auto pcg_files = file_list_[3]; // fullpath: ../tw04_04/EVENTS_tw04_04_PCG.DATA

        // WELL INDEX DATA OBJECTS
        WIData WIDataRMS, WIDataPCG;
        WIDataRMS.data_tag = "RMS";
        WIDataPCG.data_tag = "PCG";
        WIDataPCG.grid_file = file_path_;

        // DEBUG
        debug_msg(false, "well_dir_list", dir_names_, dir_list_, 0, WIDataRMS, WIDataPCG, 0);

        // LOOP THROUGH LIST OF WELL FOLDERS: FOR WELL FOLDER ii: READ PCG & RMS COMPDAT DATA
        int num_files = (debug_) ? 5 : rms_files.length(); //override
        QString str_out;
        QString lstr_out = "\n================================================================================";

        for (int ii = 0; ii < num_files; ++ii) {

            // WRITE TO TEX FILE
            WIDataPCG.tex_file = dir_list_[ii] + "/" + dir_names_[ii] + ".tex";
            WIDataRMS.tex_file = WIDataPCG.tex_file;
            str_out = "\\begin{alltt}" + lstr_out + "\nChecking IJK and WCF data for well: "
                      + dir_names_[ii] + " (row numbering uses 0-indexing)" + lstr_out;
            Utilities::FileHandling::WriteStringToFile(str_out, WIDataPCG.tex_file);

            // MESSAGE OUTPUT
            str_out = lstr_out + "\nChecking IJK and WCF data for well: "
                      + dir_names_[ii] + lstr_out;
            std::cout << "\033[1;36m" << str_out.toStdString() << "\033[0m";

            // READ COMPDAT + XYZ FILES
            WIDataRMS.ReadData(rms_files[ii],dir_list_[ii]);
            WIDataPCG.ReadData(pcg_files[ii],dir_list_[ii]);

            // MAKE PCG_NEW.DATA FILES USING PRECOMPILED WellIndexCalculator
            WIDataPCG.CalculateWCF();

            // DEBUG: PRINT OLD IJK/WCF VALUES B/F SHIFTING TO NEW
            WIDataPCG.PrintIJKData(dir_list_[ii] + "/DBG_" + dir_names_[ii] + "_PCG.IJK");
            WIDataPCG.PrintWCFData(dir_list_[ii] + "/DBG_" + dir_names_[ii] + "_PCG.WCF");

            // SHIFT OVER TO DATA COMPUTED USING WellIndexCalculator
            WIDataPCG.IJK = WIDataPCG.IJKN;
            WIDataPCG.WCF = WIDataPCG.WCFN;

            // DEBUG
            debug_msg(false, "RMS_PCG_IJK_data", dir_names_, dir_list_, ii, WIDataRMS, WIDataPCG, 0);

            // REMOVE ROW IF LOW WCF
            RemoveRowsLowWCF(WIDataRMS);
            RemoveRowsLowWCF(WIDataPCG);

            // REMOVE EXTRA ROWS IF DATA HAS UNEQUAL LENGTH
            RemoveSuperfluousRowsWrapper(WIDataRMS, WIDataPCG, dir_list_, dir_names_, ii);

            // COMPARE IJK AND PCG VALUES (EQUAL LENGTH DATA)
            CompareIJK(WIDataRMS, WIDataPCG);
            CompareWCF(WIDataRMS, WIDataPCG);

            Utilities::FileHandling::WriteLineToFile("\\end{alltt}", WIDataPCG.tex_file);
        }
    }
}