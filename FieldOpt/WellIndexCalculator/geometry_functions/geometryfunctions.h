#ifndef GEOMETRYFUNCTIONS_H
#define GEOMETRYFUNCTIONS_H
#include "Reservoir/grid/xyzcoordinate.h"
#include "Reservoir/grid/cell.h"
#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"
#include "Reservoir/grid/grid_exceptions.h"
#include <QList>
#include <QVector3D>
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
        Eigen::Vector3d line_plane_intersection(Eigen::Vector3d p0, Eigen::Vector3d p1,
                                                Eigen::Vector3d normal_vector, Eigen::Vector3d point_in_plane);

        Eigen::Vector3d normal_vector(Eigen::Vector3d p0, Eigen::Vector3d p1, Eigen::Vector3d p2);

        /*!
         * \brief point_on_same_side returns true if point is on the same side of a plane
         * (containing plane_point and with normal_vector as the normal vector) as the normal vector,
         * true if it is in the plane, and false if it's on the other side.
         * \param point
         * \param plane_point
         * \param normal_vector
         * \return normal_vector spanned by points
        */
        bool point_on_same_side(Eigen::Vector3d point, Eigen::Vector3d plane_point,
                                Eigen::Vector3d normal_vector, double slack);

        /*!
         * \brief Given a reservoir with blocks and a line(start_point to end_point), return global index of all blocks interesected
         * by the line and the points of intersection
         * \param start point line
         * \param end point line
         * \param reservoir (containing blocks/cells)
         * \param QList for storing global indeces of intersected blocks
         * \param QList for storing line segments that are inside intersected blocks
         */
        QPair<QList<int>, QList<Eigen::Vector3d> > cells_intersected(Eigen::Vector3d start_point,
                                                                     Eigen::Vector3d end_point,
                                                                     Reservoir::Grid::Grid *grid);

        /*!
         * \brief Generates a double array with the numbers of 3 corners from each of the 6 faces of a cell that
         * will be used to create a normal vector for each face.
         * \return double list of corner numbers for each face
         */
        QList<QList<Eigen::Vector3d>> cell_planes_coords(QList<Reservoir::Grid::XYZCoordinate > corners);

        Eigen::Vector3d find_exit_point(Reservoir::Grid::Cell cell, Eigen::Vector3d start_point,
                                        Eigen::Vector3d end_point, Eigen::Vector3d exception_point);

        /*!
         * \brief project_v1_onv2 is the orthognal projection of a vector v1 onto the vector v2.
         * \param vector to be projected
         * \param vector onto which projection is done
         * \return orthogonal projection of vector
        */
        Eigen::Vector3d project_v1_on_v2(Eigen::Vector3d v1, Eigen::Vector3d v2);

        /*!
         * \brief Compute the well index for a (one) single cell/block by using the Projection Well Method (Shu 2005).
         * \param cell/block
         * \param start point line segment/well
         * \param end point line segment/well
         * \param wellbore radius
         * \return Well index for block/cell
        */
        double well_index_cell_qvector(Reservoir::Grid::Cell block,
                                       QList<Eigen::Vector3d> start_points,
                                       QList<Eigen::Vector3d> end_points, double wellbore_radius);

        /*!
         * \brief Auxilary function for well_index_cell function
         * \param lenght of projection in first direction
         * \param size block second direction
         * \param size block third direction
         * \param permeability second direction
         * \param permeability second direction
         * \param wellbore radius
         * \return directional well index
        */
        double dir_well_index(double Lx, double dy, double dz, double ky, double kz, double wellbore_radius);

        /*!
         * \brief Auxilary function(2) for well_index_cell function
         * \param size block second direction
         * \param size block third direction
         * \param permeability second direction
         * \param permeability second direction
         * \return directional wellblock radius
         */
        double dir_wellblock_radius(double dx, double dy, double kx, double ky);

        /*!
         * \brief Given a Reservoir::Grid::Cell cell and a point, return true if the given point is inside the (or on boundary of) cell
         * \param Cell containing coordinates of corners
         * \param point
         * \return true if point is inside cell, false if it is not
         */
        bool is_point_inside_cell(Reservoir::Grid::Cell cell, Eigen::Vector3d point);

        /*!
         * \brief Given a Reservoir::Grid::Grid grid and a point, return a cell that envelopes the point
         * \param A grid containing cells
         * \param point
         * \return returns cell enveloping/containing point. return 0 if no cell contains point
         */
        Reservoir::Grid::Cell get_cell_enveloping_point(Reservoir::Grid::Grid *grid, Eigen::Vector3d point);

        /*!
         * \brief Given a reservoir and a line segment (start, end) return the calculated well indeces for all cells intersected by the line segment.
         * \param A grid containing cells
         * \param start point line segment/well
         * \param end point line segment/well
         * \return list of cells intersected and their calculated well indeces
         */
        QPair<QList<int>, QList<double>> well_index_of_grid(Reservoir::Grid::Grid *grid,
                                                            QList<Eigen::Vector3d> start_points,
                                                            QList<Eigen::Vector3d> end_points, double wellbore_radius);

        /*!
         * \brief Given a grid and one or more wells (start_points to end_points), find blocks intersected
         * by the well and return the well indices of the penetrated blocks (whose well index value are
         * above min_wi) by writing it to a file.
         * \param grid
         * \param list of start points
         * \param list of end points
         * \param wellbore radius
         * \param well index treshold value. if below this level it will not be printed.
         * \param name (and path) of file to be created and written to.
         */
//        void print_well_index_file(Reservoir::Grid::Grid *grid , QList<QVector3D> start_point, QList<QVector3D> end_points, double wellbore_radius, double min_wi, QString filename);
    };

}

#endif // GEOMETRYFUNCTIONS_H
