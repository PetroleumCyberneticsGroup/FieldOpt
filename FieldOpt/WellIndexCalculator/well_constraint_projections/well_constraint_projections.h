#ifndef WELL_CONSTRAINT_PROJECTIONS_H
#define WELL_CONSTRAINT_PROJECTIONS_H


#include <RpolyPlusPlus/polynomial.h>
#include <RpolyPlusPlus/find_polynomial_roots_jenkins_traub.h>
#include "WellIndexCalculator/geometry_functions/geometryfunctions.h"
#include "Reservoir/grid/cell.h"
#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"
#include "Reservoir/grid/grid_exceptions.h"
#include <QList>
#include <Eigen/Dense>
#include <QList>

namespace WellIndexCalculator {
/*!
 * \brief WellConstraintProjections is a collection for solving projection of well constraints
 *
 * We want to collect all mathematical functions for projection wells back into feasible space
 * Most of these functions will be used for the interwell distance problem as it concerns itself
 * with finding and solving a sextic equation
 */
    namespace WellConstraintProjections
    {
        using namespace Eigen;

        // Functions to build A and b for different cases.
        Matrix3d build_A_4p(QList<Vector3d> coords);
        Vector3d build_b_4p(QList<Vector3d> coords, double d);
        Matrix3d build_A_3p(QList<Vector3d> coords);
        Vector3d build_b_3p(QList<Vector3d> coords, double d);



        // This function finds all potential KKT points (s) for an equation (A-muI)s = b, length(s) = 1.
        QList<Vector3d> kkt_eq_solutions(Matrix3d A, Vector3d b);
        VectorXd coeff_vector(Vector3d D, Matrix3d Qt, Vector3d b);

        // Solves the above (A-muI)s = b, length(s) = 1, but where the inverse does not exist.
        Vector3d non_inv_quad_coeffs(Vector3d x, Vector3d n);
        QList<Vector3d> non_inv_solution(Matrix3d A, Vector3d b);
        bool solution_existence(Matrix3d A, Vector3d b);

        // Remove all values from a matrix or vector which are below threshold value eps. Helps stabillity in some cases.
        Matrix3d rm_entries_eps_matrix(Matrix3d m, double eps);
        VectorXd rm_entries_eps_coeffs(VectorXd m, double eps);

        // Help functions. Moving ponts, shortest distance, costs, feasibillity etc.
        double shortest_distance_n_wells(QList<QList<Vector3d> > coords, int n);
        double shortest_distance(QList<Vector3d> coords);

        Vector3d project_point_to_plane(Vector3d point, Vector3d normal_vector,
                                        Vector3d plane_point);
        QList<Vector3d> move_points_4p(QList<Vector3d> coords, double d, Vector3d s);
        QList<Vector3d> move_points_3p(QList<Vector3d> coords, double d, Vector3d s);
        double movement_cost(QList<Vector3d> old_coords, QList<Vector3d> new_coords);
        bool feasible_well_length(QList<QList<Vector3d> > coords, double max, double min, double tol);
        bool feasible_interwell_distance(QList<QList<Vector3d> > coords, double d, double tol);


        // THESE FUNCTIONS SHOULD ALL BE PUBLIC. ACTUAL CONSTRAINT PROJECTIONS FOR SINGLE AND MULTIPLE WELLS

        /*!
         * \brief Well length constraint projection. Projects the heel and toe of a well so that the length of the
         * well is at least min and at most max.
         *
         * \param Coordinates of the heel of the well
         * \param Coordinates of the toe of the well
         * \param maximum allowed length of well
         * \param minimum allowed length of well
         * \param Tolerance addition epsilon. Moves the heel and toe and extra length epsilon.
         * \return List of moved heel and toe. First Vector is the heel and the second Vector is the toe.
        */
        QList<Vector3d> well_length_projection(Vector3d heel, Vector3d toe, double max, double min, double epsilon);

        /*!
         * \brief Master function for the interwell constraint projection. Projects two wells so that they are at
         * least a distance d appart.
         *
         * \param Two line segments defined by their endpoints. Two first entries belong to first line segment,
         * two last entries belong to second line segment.
         * \param d minimum distance d allowed between the two wells.
         * \return Cheapest(L2-norm) projection of line segments s.t. they are at least a distance d appart.
        */
        QList<Vector3d> interwell_constraint_projection(QList<Vector3d> coords, double d);

        /*!
         * \brief Projects any number of wells so that they are at least a distance d appart.
         * \param QList of wells. Each well is a QList of two Vector3D, the heel and toe of the well.
         * \param minimum distance d allowed between any pair of two wells.
         * \return A projection (not necessarily best one) of wells s.t. they are at least a distance d appart.
        */
        QList<QList<Vector3d> > interwell_constraint_multiple_wells(QList<QList<Vector3d>> coords,
                                                                    double d, double tol);

        /*!
         * \brief MWell length constraint projection for multiple wells. Projects the heel and toe of every well so that
         * the length of the well is at least min and at most max.
         *
         * \param ith element in QList corresponds to well i. Coordinates of the heel and toe of well stored in a QList
         * which contains two Vector3d
         * \param maximum allowed length of well
         * \param minimum allowed length of well
         * \param Tolerance addition epsilon. Moves the heel and toe and extra length epsilon.
         * \return Optimal list of moved wells such that length constraint is satisfied (+ epsilon)
        */
        QList<QList<Vector3d>> well_length_constraint_multiple_wells(QList<QList<Vector3d>> wells,
                                                                     double max, double min, double epsilon);

        /*!
         * \brief Master function for the interwell AND length constraint projection. Projects any number of wells so
         * that both constraints are satisfied.
         *
         * \param List of wells. ith element in QList corresponds to well i. Coordinates of the heel and toe of well
         * stored in a QList which contains two Vector3d
         * \param minimum distance d allowed between the two wells.
         * \param tolerance tol which is used as a stopping criterion for the alorithm. Algorithm stops if both constrains
         * are satisfied up to tolerance level.
         * \param maximum allowed length of well
         * \param minimum allowed length of well
         * \param Tolerance addition epsilon. Moves the heel and toe and extra length epsilon.
         * \return Some projection of all wells such that both constraints are satisfied.
        */
        QList<QList<Vector3d> > both_constraints_multiple_wells(QList<QList<Vector3d>> coords, double d,
                                                                double tol, double max, double min, double epsilon);

        /*!
         * \brief Given some arbitrary point and a list of cells in which the point is allowed to be, finds the projection to
         * this space of allowed domain.
         * \param some arbitrary point
         * \param List of allowed blocks to which point should be projected
         * \return projected point. If point is already legal returns the same point.
        */
        Vector3d well_domain_constraint(Vector3d point, QList<Reservoir::Grid::Cell> cells);

        Vector3d well_domain_constraint_vector(Vector3d point, std::vector<Reservoir::Grid::Cell> cells);

        Vector3d well_domain_constraint_indices(Vector3d point, Reservoir::Grid::Grid *grid, QList<int> index_list);

        /*!
         * \brief Sets all elements of a 3-by-3 matrix whose absolute value is below a certain threshold to zero.
         * \param Matrix
         * \param Threshold
         * \return Matrix with cleared sub-threshold values
         */
        Vector3d rm_entries_eps(Vector3d m, double eps);

        /*!
         * \brief Given a cell block and a point in space, computes the point in the cell block which is closest to the given point
         * \param point point to check
         * \return L2 norm of vectors of how points moved.
         */
        Vector3d point_to_cell_shortest(Reservoir::Grid::Cell cell, Vector3d point);

        /*!
         * \brief Given a face (4 corner points) and a point in 3D space, computes the point on the face which is closest to given point
         * \param face The face to project to.
         * \param point The point to project.
         * \param cell The cell the face belongs to.
         * \return point on face closest to given point
         */
        Vector3d point_to_face_shortest(Reservoir::Grid::Cell::Face face, Vector3d point, Reservoir::Grid::Cell cell);

        /*!
         * \brief computes which point on a line segment that is closest to a given point
         * \param line_segment The line segment to project to.
         * \param P0 The point to project
         * \return point on line segment closest to given point
         */
        Vector3d point_to_line_shortest(QList<Vector3d> line_segment, Vector3d P0);

        /*!
         * \brief Compute the closest points on the line segments P and Q.
         *
         * If a pair of endpoints are equal they are considered a point. The point on the line closest to
         * that point will then be computed.
         *
         * Function runs through all possible combinations of where the two closest points could be located.
         * This function is a slightly edited version of the one from:
         * http://www.geometrictools.com/GTEngine/Include/Mathematics/GteDistSegmentSegmentExact.h
         *
         * David Eberly, Geometric Tools, Redmond WA 98052
         * Copyright (c) 1998-2016
         * Distributed under the Boost Software License, Version 1.0.
         * http://www.boost.org/LICENSE_1_0.txt
         * http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
         * File Version: 2.1.0 (2016/01/25)d
         *
         * \param P0 End point on line P.
         * \param P1 End point on line P.
         * \param Q1 End point on line Q.
         * \param Q2 End point on line Q.
         * \return A pair containing the closest point on P and the closest point on Q (closest_P, closest_Q).
         */
        QPair<Vector3d, Vector3d> closest_points_on_lines(Vector3d P0, Vector3d P1, Vector3d Q0, Vector3d Q1);
    }

}
#endif // WELL_CONSTRAINT_PROJECTIONS_H
