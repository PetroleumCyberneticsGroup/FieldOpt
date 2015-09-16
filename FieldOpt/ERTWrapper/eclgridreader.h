#ifndef ECLGRIDREADER_H
#define ECLGRIDREADER_H

#include <qt5/QtCore/QString>
#include <ert/ecl/ecl_grid.h>
#include <QVector3D>
#include <QList>

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
        virtual ~ECLGridReader(); //!< Frees the grid object if it has been set.

        /*!
         * \brief ReadEclGrid reads an ECLIPSE .GRID or .EGRID file.
         * \param file_name The path to the grid to be read, including suffix.
         */
        void ReadEclGrid(QString file_name);

        /*!
         * \brief ConvertIJKToGlobalIndex Converts a set of zero-offset (i,j,k) coordinates to the global index to that cell.
         * \param ijk Zero-offset coordinates for a cell.
         * \return global index
         */
        int ConvertIJKToGlobalIndex(QVector3D ijk);

        /*!
         * \brief ConvertGlobalIndexToIJK Converts a global index for a cell to the corresponding zero-offset (i,j,k) coordinates.
         * \param global_index Global index for a cell.
         * \return (i,j,k) Zero-offset coordinates
         */
        QVector3D ConvertGlobalIndexToIJK(int global_index);

        /*!
         * \brief The Dims struct holds the x,y and z dimensions of a grid.
         */
        struct Dims {
            int nx, ny, nz;
        };

        /*!
         * \brief Dimensions returns the total dimensions of the grid that has been read.
         * \return Dims struct containing the number of blocks in x, y and z direction.
         */
        Dims Dimensions();

        /*!
         * \brief ActiveCells Number of active cells in the grid that has been read.
         */
        int ActiveCells();

        /*!
         * \brief The Cell struct represents a cell in the grid.
         *
         * The corners list contains all the corner points specifying the grid.
         * The four first coordinates specifies the corners in the lower layer
         * counter-clockwise, beginning in the lower left. The four last coordinates
         * specifies the same for the upper layer.
         */
        struct Cell {
            int global_index;
            QList<QVector3D*>* corners;
            QVector3D* center;
        };

        /*!
         * \brief GetGridCell get a Cell struct describing the cell with the specified global index.
         * \param global_index The global index of the cell to get.
         * \return Cell struct.
         */
        Cell GetGridCell(int global_index);
    };
    }
}

#endif // ECLGRIDREADER_H
