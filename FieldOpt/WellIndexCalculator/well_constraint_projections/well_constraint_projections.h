#ifndef WELL_CONSTRAINT_PROJECTIONS_H
#define WELL_CONSTRAINT_PROJECTIONS_H


#include <RpolyPlusPlus/polynomial.h>
#include <RpolyPlusPlus/find_polynomial_roots_jenkins_traub.h>
#include "Reservoir/grid/xyzcoordinate.h"
#include "WellIndexCalculator/geometry_functions/geometryfunctions.h"
#include "WellIndexCalculator/geometry_functions/geometryfunctions_exceptions.h"
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
    class WellConstraintProjections
    {

    public:
        //All of the functions below should probably be private. Public so I can test them.

        // simple summation and product functions
        static double sum_i(double a,double b,double c);
        static double sum_ij(double a,double b,double c);
        static double prod_i(double a,double b,double c);

        // Functions to build A and b for different cases.
        static Eigen::Matrix3d build_A_4p(QList<Eigen::Vector3d> coords);
        static Eigen::Vector3d build_b_4p(QList<Eigen::Vector3d> coords, double d);
        static Eigen::Matrix3d build_A_3p(QList<Eigen::Vector3d> coords);
        static Eigen::Vector3d build_b_3p(QList<Eigen::Vector3d> coords, double d);



        // This function finds all potential KKT points (s) for an equation (A-muI)s = b, length(s) = 1.
        static QList<Eigen::Vector3d> kkt_eq_solutions(Eigen::Matrix3d A, Eigen::Vector3d b);
        static Eigen::VectorXd coeff_vector(Eigen::Vector3d D, Eigen::Matrix3d Qt, Eigen::Vector3d b);

        // Solves the above (A-muI)s = b, length(s) = 1, but where the inverse does not exist.
        static Eigen::Vector3d non_inv_quad_coeffs(Eigen::Vector3d x, Eigen::Vector3d n);
        static QList<Eigen::Vector3d> non_inv_solution(Eigen::Matrix3d A, Eigen::Vector3d b);
        static bool solution_existence(Eigen::Matrix3d A, Eigen::Vector3d b);

        // Remove all values from a matrix or vector which are below threshold value eps. Helps stabillity in some cases.
        static Eigen::Matrix3d rm_entries_eps_matrix(Eigen::Matrix3d m, double eps);
        static Eigen::VectorXd rm_entries_eps_coeffs(Eigen::VectorXd m, double eps);

        // Help functions. Moving ponts, shortest distance, costs, feasibillity etc.
        static double shortest_distance_n_wells(QList<QList<Eigen::Vector3d> > coords, int n);
        static double shortest_distance(QList<Eigen::Vector3d> coords);
        static double shortest_distance_3p(QList<Eigen::Vector3d> coords);
        static Eigen::Vector3d project_point_to_plane(Eigen::Vector3d point, Eigen::Vector3d normal_vector,
                                                      Eigen::Vector3d plane_point);
        static QList<Eigen::Vector3d> move_points_4p(QList<Eigen::Vector3d> coords, double d, Eigen::Vector3d s);
        static QList<Eigen::Vector3d> move_points_3p(QList<Eigen::Vector3d> coords, double d, Eigen::Vector3d s);
        static double movement_cost(QList<Eigen::Vector3d> old_coords, QList<Eigen::Vector3d> new_coords);
        static bool feasible_well_length(QList<QList<Eigen::Vector3d> > coords, double max, double min, double tol);
        static bool feasible_interwell_distance(QList<QList<Eigen::Vector3d> > coords, double d, double tol);


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
        static QList<Eigen::Vector3d> well_length_projection(Eigen::Vector3d heel, Eigen::Vector3d toe, double max,
                                                             double min, double epsilon);

        /*!
         * \brief Master function for the interwell constraint projection. Projects two wells so that they are at
         * least a distance d appart.
         *
         * \param Two line segments defined by their endpoints. Two first entries belong to first line segment,
         * two last entries belong to second line segment.
         * \param minimum distance d allowed between the two wells.
         * \return Cheapest(L2-norm) projection of line segments s.t. they are at least a distance d appart.
        */
        static QList<Eigen::Vector3d> interwell_constraint_projection(QList<Eigen::Vector3d> coords, double d);

        /*!
         * \brief Projects any number of wells so that they are at least a distance d appart.
         * \param QList of wells. Each well is a QList of two Vector3D, the heel and toe of the well.
         * \param minimum distance d allowed between any pair of two wells.
         * \return A projection (not necessarily best one) of wells s.t. they are at least a distance d appart.
        */
        static QList<QList<Eigen::Vector3d> > interwell_constraint_multiple_wells(QList<QList<Eigen::Vector3d> > coords, double d, double tol);

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
        static QList<QList<Eigen::Vector3d>> well_length_constraint_multiple_wells(QList<QList<Eigen::Vector3d>> wells, double max, double min, double epsilon);

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
        static QList<QList<Eigen::Vector3d> > both_constraints_multiple_wells(QList<QList<Eigen::Vector3d> > coords, double d, double tol, double max, double min, double epsilon);

        /*!
         * \brief Given some arbitrary point and a list of cells in which the point is allowed to be, finds the projection to
         * this space of allowed domain.
         * \param some arbitrary point
         * \param List of allowed blocks to which point should be projected
         * \return projected point. If point is already legal returns the same point.
        */
        static Eigen::Vector3d well_domain_constraint(Eigen::Vector3d point, QList<Reservoir::Grid::Cell> cells);

        static Eigen::Vector3d well_domain_constraint_vector(Eigen::Vector3d point, std::vector<Reservoir::Grid::Cell> cells);

        static Eigen::Vector3d well_domain_constraint_indices(Eigen::Vector3d point, Reservoir::Grid::Grid *grid, QList<int> index_list);

        /*!
         * \brief Sets all elements of a 3-by-3 matrix whose absolute value is below a certain threshold to zero.
         * \param Matrix
         * \param Threshold
         * \return Matrix with cleared sub-threshold values
        */
        static Eigen::Vector3d rm_entries_eps(Eigen::Vector3d m, double eps);

        /*!
             * \brief Given a cell block and a point in space, computes the point in the cell block which is closest to the given point
             * \param initial points
             * \param moved points
             * \return L2 norm of vectors of how points moved.
             */
        static Eigen::Vector3d point_to_cell_shortest(Reservoir::Grid::Cell cell, Eigen::Vector3d point);

        /*!
         * \brief Given a face (4 corner points) and a point in 3D space, computes the point on the face which is closest to given point
         * \param initial points
         * \param moved points
         * \return point on face closest to given point
         */
        static Eigen::Vector3d point_to_face_shortest(QList<Eigen::Vector3d> face, Eigen::Vector3d point, Reservoir::Grid::Cell cell);

        /*!
         * \brief computes which point on a line segment that is closest to a given point
         * \param line segment
         * \param given point
         * \return point on line segment closest to given point
         */
        static Eigen::Vector3d point_to_line_shortest(QList<Eigen::Vector3d> line_segment, Eigen::Vector3d P0);
    };

}
#endif // WELL_CONSTRAINT_PROJECTIONS_H
