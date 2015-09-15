#include "eclgridreader.h"
#include <iostream>

namespace ERTWrapper {
    namespace ECLGrid {
        ECLGridReader::ECLGridReader()
        {
            ecl_grid_ = 0;
        }

        ECLGridReader::~ECLGridReader()
        {
            ecl_grid_free(ecl_grid_);
        }

        void ECLGridReader::ReadEclGrid(QString file_name)
        {
            file_name_ = file_name;
            if (ecl_grid_ == 0) {
                ecl_grid_ = ecl_grid_alloc(file_name.toStdString().c_str());
            }
            else {
                ecl_grid_free(ecl_grid_);
                ecl_grid_ = ecl_grid_alloc(file_name.toStdString().c_str());
            }
        }

        int ECLGridReader::ActiveCells()
        {
            if (ecl_grid_ == 0) return 0;
            else return ecl_grid_get_nactive(ecl_grid_);
        }
    }
}
