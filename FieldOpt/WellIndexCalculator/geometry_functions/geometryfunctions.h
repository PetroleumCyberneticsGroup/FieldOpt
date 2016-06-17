#ifndef GEOMETRYFUNCTIONS_H
#define GEOMETRYFUNCTIONS_H
#include "Reservoir/grid/cell.h"
#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"
#include "Reservoir/grid/grid_exceptions.h"
#include <QList>
#include <QPair>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>

namespace WellIndexCalculator {

/*!
 * \brief WellIndexCalculator is a toolbox for doing geometric calculations on points and lines within a Grid
 *
 * We want to collect all geometric functions necessary to determine which Cells are intercepted by a line
 * and thereafter use this to compute the Well Index for every Cell.
 */

    namespace GeometryFunctions {
        /*!
         * Find the point of inntersection between a line an a plane.
         * \param p0 Point defining one end of the line.
         * \param p1 Point defining other end of the line.
         * \param normal_vector The normal vector of the plane.
         * \param point_in_plane A point in the plane.
         * \return The point of intersection.
         */
        Eigen::Vector3d line_plane_intersection(Eigen::Vector3d p0, Eigen::Vector3d p1,
                                                Eigen::Vector3d normal_vector, Eigen::Vector3d point_in_plane);

        /*!
         * \brief point_on_same_side returns true if point is on the same side of a plane
         * (containing plane_point and with normal_vector as the normal vector) as the normal vector,
         * true if it is in the plane, and false if it's on the other side.
         *
         * In the function, a dot product helps us determine if the angle between the two
         * vectors is below (positive answer), at (zero answer) or above
         * (negative answer) 90 degrees. Essentially telling us which side
         * of a plane the point is
         *
         * \param point The point to be checked.
         * \param plane_point A point in the plane.
         * \param normal_vector The normal vector of the plane.
         * \return True if the point is on the same side as the normal vector or in the plane; otherwise false.
        */
        bool point_on_same_side(Eigen::Vector3d point, Eigen::Vector3d plane_point,
                                Eigen::Vector3d normal_vector, double slack);

        /*!
         * \brief Given a reservoir with blocks and a line(start_point to end_point), return global index of all
         * blocks interesected
         * by the line and the points of intersection
         * \param start_point The start point of the well path.
         * \param end_point The end point of the well path.
         * \param grid The grid object containing blocks/cells.
         * \return A pair containing global indeces of intersected cells and the endpoints of the line segment inside each cell.
         */
        QPair<QList<int>, QList<Eigen::Vector3d>> cells_intersected(Eigen::Vector3d start_point,
                                                                    Eigen::Vector3d end_point,
                                                                    Reservoir::Grid::Grid *grid);

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
        Eigen::Vector3d find_exit_point(Reservoir::Grid::Cell cell, Eigen::Vector3d start_point,
                                        Eigen::Vector3d end_point, Eigen::Vector3d exception_point);

        /*!
         * \brief Compute the well index (aka. transmissibility factor) for a (one) single cell/block by
         * using the Projection Well Method (Shu 2005).
         *
         * Assumption: The block is fairly regular, i.e. corners are straight angles.
         *
         * \note Corner points of Cell(s) are always listed in the same order and orientation. (see
         * Reservoir::Grid::Cell for illustration).
         *
         * \param cell Well block to compute the WI in.
         * \param start_points line segment/well
         * \param end_points line segment/well
         * \param wellbore_radius The wellbore radius.
         * \return Well index for block/cell
        */
        double well_index_cell(Reservoir::Grid::Cell cell, QList<Eigen::Vector3d> start_points,
                               QList<Eigen::Vector3d> end_points, double wellbore_radius);

        /*!
         * \brief Auxilary function for well_index_cell function
         * \param Lx lenght of projection in first direction
         * \param dy size block second direction
         * \param dz size block third direction
         * \param ky permeability second direction
         * \param kz permeability second direction
         * \param wellbore_radius wellbore radius
         * \return directional well index
        */
        double dir_well_index(double Lx, double dy, double dz, double ky, double kz, double wellbore_radius);

        /*!
         * \brief Auxilary function(2) for well_index_cell function
         * \param dx size block second direction
         * \param dy size block third direction
         * \param kx permeability second direction
         * \param ky permeability second direction
         * \return directional wellblock radius
         */
        double dir_wellblock_radius(double dx, double dy, double kx, double ky);

        /*!
         * \brief Given a reservoir and a line segment (start, end) return the calculated well
         * indeces for all cells intersected by the line segment.
         * \param grid A grid containing cells
         * \param start_points start points for line segments/wells
         * \param end_points end point line segment/well
         * \return lists of cells intersected and their calculated well indeces
         */
        QPair<QList<int>, QList<double>> well_index_of_grid(Reservoir::Grid::Grid *grid,
                                                            QList<Eigen::Vector3d> start_points,
                                                            QList<Eigen::Vector3d> end_points, double wellbore_radius);
    };

}

#endif // GEOMETRYFUNCTIONS_H
