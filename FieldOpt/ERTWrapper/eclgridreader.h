#ifndef ECLGRIDREADER_H
#define ECLGRIDREADER_H

#include <qt5/QtCore/QString>
#include <ert/ecl/ecl_grid.h>

namespace ERTWrapper {

    namespace ECLGrid {

    /*!
     * \brief The ECLGridReader class reads ECLIPSE grid files (.GRID and .EGRID).
     */
    class ECLGridReader
    {
    private:
        QString file_name_;
        ecl_grid_type* ecl_grid_;
    public:
        ECLGridReader();
        virtual ~ECLGridReader();
        void ReadEclGrid(QString file_name);

        struct dimensions {
            int x, y, z;
        };
        dimensions Dimensions();
        int ActiveCells(); //!< TODO: This is a temporary method for testing. Should be removed.
    };
    }
}

#endif // ECLGRIDREADER_H
