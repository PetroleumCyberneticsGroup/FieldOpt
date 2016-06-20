#ifndef WELLINDEXCALCULATOR_H
#define WELLINDEXCALCULATOR_H

#include <QList>
#include <QPair>
#include <Eigen/Dense>
#include <Eigen/Core>
#include "Reservoir/grid/grid.h"
#include "intersected_cell.h"

namespace Reservoir {
    namespace WellIndexCalculation {
        using namespace Eigen;

        /*!
         * \brief The WellIndexCalculation class deduces the well blocks and their respecitve well indices/transmissibility
         * factors for one or more well splines defined by a heel and a toe.
         *
         * Note that some of the internal datastructures in this class seem more complex than they need to be. This is
         * because the internal methods support well splines consisting of more than one point. This is, however, not yet
         * supported by the Model library and so have been "hidden".
         *
         * Credit for computations in this class goes to @hilmarm.
         */
        class WellIndexCalculator {
        public:
            WellIndexCalculator(){}
            WellIndexCalculator(Grid::Grid *grid, double wellbore_radius); // \todo Remove this.
            WellIndexCalculator(Grid::Grid *grid);

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

            Grid::Grid *grid_; //!< The grid used in the calculations.
            double wellbore_radius_; // \todo Remove this.
            Vector3d heel_;
            Vector3d toe_;
            QList<Vector3d> spline_points() {return QList<Vector3d>({heel_, toe_});}

        public:

            /*!
             * Find the point of inntersection between a line an a plane.
             * \param p0 Point defining one end of the line.
             * \param p1 Point defining other end of the line.
             * \param plane A plane.
             * \return The point of intersection.
             */
            static Vector3d line_plane_intersection(const Vector3d &p0, const Vector3d &p1, const Grid::Cell::Face &plane);

            /*!
             * \brief Given a reservoir with blocks and a line(start_point to end_point), return global index of all
             * blocks interesected by the line, as well as the point where the line enters the block.
             * by the line and the points of intersection
             * \param start_point The start point of the well path.
             * \param end_point The end point of the well path.
             * \param grid The grid object containing blocks/cells.
             * \return A pair containing global indeces of intersected cells and the points where it enters each cell
             * (and thereby leaves the previous cell) of the line segment inside each cell.
             */
            QList<IntersectedCell> cells_intersected();

            /*!
             * \brief Find the point where the line bethween the start_point and end_point exits a cell.
             *
             * Takes as input an entry_point end_point which defines the well path. Finds the two points on the path
             * which intersects the block faces and chooses the one that is not the entry point, i.e. the exit point.
             *
             * \todo Find a better name for the exception_point and describe it better.
             *
             * \param cell The cell to find the well paths exit point in.
             * \param start_point The start point of the well path.
             * \param end_point The end point of the well path.
             * \param exception_point A specific point we don't want the function to end up in.
             * \return The point where the well path exits the cell.
             */
            Vector3d find_exit_point(Grid::Cell &cell, Vector3d &start_point,
                                     Vector3d &end_point, Vector3d &exception_point);

            /*!
             * \brief Compute the well index (aka. transmissibility factor) for a (one) single cell/block by
             * using the Projection Well Method (Shu 2005).
             *
             * Assumption: The block is fairly regular, i.e. corners are straight angles.
             *
             * \note Corner points of Cell(s) are always listed in the same order and orientation. (see
             * Grid::Cell for illustration).
             *
             * \param icell Well block to compute the WI in.
             * \return Well index for block/cell
            */
            double well_index_cell(IntersectedCell &icell);

            /*!
             * \brief Auxilary function for well_index_cell function
             * \param Lx lenght of projection in first direction
             * \param dy size block second direction
             * \param dz size block third direction
             * \param ky permeability second direction
             * \param kz permeability second direction
             * \return directional well index
            */
            double dir_well_index(double Lx, double dy, double dz, double ky, double kz);

            /*!
             * \brief Auxilary function(2) for well_index_cell function
             * \param dx size block second direction
             * \param dy size block third direction
             * \param kx permeability second direction
             * \param ky permeability second direction
             * \return directional wellblock radius
             */
            static double dir_wellblock_radius(double dx, double dy, double kx, double ky);

            /*!
             * \brief Given a reservoir and a spline (heel, intermediate1, ..., toe) return the calculated well
             * indeces for all cells intersected by the spline segment.
             * \return lists of cells intersected and their calculated well indeces
             */
            QList<IntersectedCell> compute_well_indices();
        };

    }
}

#endif // WELLINDEXCALCULATOR_H
