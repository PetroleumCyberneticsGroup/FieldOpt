#ifndef WELLINDEXCALCULATOR_H
#define WELLINDEXCALCULATOR_H

#include <QList>
#include <QPair>
#include "Reservoir/grid/grid.h"

namespace WellIndexCalculator {
    using namespace Eigen;

    /*!
     * \brief The WellIndexCalculator class deduces the well blocks and their respecitve well indices/transmissibility
     * factors for one or more well splines defined by a heel and a toe.
     *
     * Note that some of the internal datastructures in this class seem more complex than they need to be. This is
     * because the internal methods support well splines consisting of more than one point. This is, however, not yet
     * supported by the Model library and so have been "hidden".
     *
     * Credit for computations in this class goes to @hilmarm.
     */
    class WellIndexCalculator
    {
    public:
        WellIndexCalculator(Reservoir::Grid::Grid *grid, double wellbore_radius); // \todo Remove this.
        WellIndexCalculator(Reservoir::Grid::Grid *grid);

        /*!
         * \brief The BlockData Holds well block data computed by this class, including their (i,j,k) coordinate and
         * the well index for the block.
         */
        struct BlockData {
            int i;
            int j;
            int k;
            double well_index;
        };

        QList<BlockData> GetBlocks(QList<Eigen::Vector3d> points); // \todo Remove this.

        /*!
         * \brief Compute the well block data for a single well.
         * \param heel The heel end point of the spline defining the well.
         * \param toe The toe end point of the spline defining the well.
         * \param wellbore_radius The radius of the well.
         * \return A list of BlockData objects containing the (i,j,k) index and well index/transmissibility factor
         * for every block intersected by the spline.
         */
        QList<BlockData> ComputeWellBlocks(Vector3d heel, Vector3d toe, double wellbore_radius);

    private:
        /*!
         * \brief The Well struct holds the information needed to compute the well blocks and their
         * respective well indices for a well spline consisting of a heel and a toe.
         */
        struct Well {
            double wellbore_radius;
            Vector3d heel;
            Vector3d toe;
        };

        Reservoir::Grid::Grid *grid_; //!< The grid used in the calculations.
        double wellbore_radius_; // \todo Remove this.
        Well current_well_; //!< Struct holding information about the well currently being treated.
    };

}

#endif // WELLINDEXCALCULATOR_H
