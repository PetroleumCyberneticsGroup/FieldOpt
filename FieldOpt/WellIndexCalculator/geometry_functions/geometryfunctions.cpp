#include "geometryfunctions.h"
#include "geometryfunctions_exceptions.h"

namespace WellIndexCalculator {

    QVector3D GeometryFunctions::line_plane_intersection(QVector3D p0, QVector3D p1, QVector3D normal_vector, QVector3D point_in_plane){

        QVector3D line_vector = QVector3D(p1.x() - p0.x(), p1.y() - p0.y(), p1.z() - p0.z());

        /* Some numerical issues when the line_vector (vector between p0 and p1)
         * is much longer (or shorter) than the normal vector. Normalize both
         * to fix the issue. Resultins parameter s further down should now
         * be more numerically stable.
         */
        line_vector.normalize();
        normal_vector.normalize();

        QVector3D w = QVector3D(p0.x() - point_in_plane.x(), p0.y() - point_in_plane.y(),p0.z() - point_in_plane.z());

        /* s is a variable for the parametrized line defined by the 2 points in line.
         *Inputting a s such that s>=0 or s<=1 will give a point on the line between the 2 points in line.
         */
        double s;
        s =( QVector3D::dotProduct(normal_vector,-w) ) / ( QVector3D::dotProduct(normal_vector,line_vector) );

        // Use the found s in parametrizaton to return intersection point.
        QVector3D intersection_point = QVector3D(p0.x() + s*(line_vector.x()), p0.y() + s*(line_vector.y()), p0.z() + s*(line_vector.z()));

        return intersection_point;
    }

    QVector3D GeometryFunctions::normal_vector(QVector3D p0, QVector3D p1, QVector3D p2)
    {
        QVector3D normal_vector = QVector3D::crossProduct(p2 - p0,p1 - p0);
        QVector3D normal_vector_ptr = QVector3D(normal_vector.x(),normal_vector.y(),normal_vector.z());
        normal_vector_ptr.normalize();
        return normal_vector_ptr;
    }

    bool GeometryFunctions::point_on_same_side(QVector3D point, QVector3D plane_point, QVector3D normal_vector, double slack)
    {
        /* The dot product helps us determine if the angle between the two
         * vectors is below (positive answer), at (zero answer) or above
         * (negative answer) 90 degrees. Essentially telling us which side
         * of a plane the point is
         */
        double dot_product = QVector3D::dotProduct(point-plane_point,normal_vector);
        return dot_product >= 0.0 - slack;
    }

    QPair<QList<int>, QList<QVector3D > > GeometryFunctions::cells_intersected(QVector3D start_point, QVector3D end_point, Reservoir::Grid::Grid *grid)
    {
        // Lists which will contain intersected block indeces and intersection points.
        QList<int> cell_global_index;
        QList<QVector3D> entry_points;

        /* Find first and last cell blocks intersected and their indeces.
         * Add first cell and first point to lists.
         */
        Reservoir::Grid::Cell last_cell = GeometryFunctions::get_cell_enveloping_point(grid,end_point);
        Reservoir::Grid::Cell first_cell = GeometryFunctions::get_cell_enveloping_point(grid,start_point);

        int last_cell_index  = last_cell.global_index();
        int first_cell_index = first_cell.global_index();
        cell_global_index.append(first_cell_index);
        entry_points.append(start_point);

        /* If first and last block are the same then this block is
         * the only one intercepted. Return current cell and start point + end point.
         */
        if( last_cell_index == first_cell_index){
            entry_points.append(end_point);
            QPair<QList<int>,QList<QVector3D>> early_pair;
            early_pair.first = cell_global_index;
            early_pair.second = entry_points;
            return early_pair;
        }


        QVector3D exit_point = find_exit_point(first_cell,start_point,end_point,start_point);
        // Make sure we follow line in the correct direction. (i.e. dot product positive)
        if( QVector3D::dotProduct(end_point - start_point, exit_point - start_point) <= 0 ){
            std::cout << "wrong direction, try other" << std::endl;
            std::cout << "exit_point = " << exit_point.x() << " " << exit_point.y() << " " << exit_point.z() << std::endl;
            exit_point = find_exit_point(first_cell, start_point, end_point, exit_point);
        }
        double epsilon = 0.01/(exit_point.distanceToPoint(end_point));
        QVector3D move_exit_epsilon = QVector3D(exit_point*(1-epsilon) + end_point * epsilon);

        Reservoir::Grid::Cell current_cell = GeometryFunctions::get_cell_enveloping_point(grid,move_exit_epsilon);
        double epsilon_temp = epsilon;
        while(current_cell.global_index() == first_cell_index){
            epsilon_temp = 10*epsilon_temp;
            move_exit_epsilon= QVector3D(exit_point * (1 - epsilon_temp) + end_point * epsilon_temp);
            current_cell = GeometryFunctions::get_cell_enveloping_point(grid,move_exit_epsilon);
        }

        while(current_cell.global_index() != last_cell_index){

            // Add current cell and previous exit point to lists
            cell_global_index.append(current_cell.global_index());
            entry_points.append(exit_point);

            // Find other exit point.
            exit_point = GeometryFunctions::find_exit_point(current_cell,exit_point,end_point,exit_point);

            // DO SOME CHECK IF EXIT POINT IS THE SAME AS END_POINT: UNLIKELY IN PRACTICE
            if(exit_point == end_point){
                current_cell = last_cell;
            }
                // Move slightly along line to enter the new cell and find cell by using GetCellEnvelopingPoint function.
            else{
                epsilon = 0.01 / (exit_point.distanceToPoint(end_point));
                move_exit_epsilon = QVector3D(exit_point * (1-epsilon) + end_point * epsilon);
                current_cell = GeometryFunctions::get_cell_enveloping_point(grid, move_exit_epsilon);
            }

        }
        cell_global_index.append(last_cell_index);
        entry_points.append(exit_point);
        entry_points.append(end_point);

        /* Collect global indeces of intersected cells and the
         * endpoints of the line segment inside each cell in a
         * QPair type to return them both
         */
        QPair<QList<int>,QList<QVector3D>> pair;
        pair.first = cell_global_index;
        pair.second = entry_points;
        return pair;

    }

    QList<QList<QVector3D>> GeometryFunctions::cell_planes_coords(QList<Reservoir::Grid::XYZCoordinate> corners)
    {
        QList<QList<int>> points;
        QList<int> p0, p1, p2, p3, p4, p5;

        p0 << 0 << 2 << 1; //First face
        p1 << 4 << 5 << 6; //Second face
        p2 << 0 << 4 << 2; //third face
        p3 << 1 << 3 << 5; //fourth face
        p4 << 0 << 1 << 4; //fifth face
        p5 << 2 << 6 << 3; //Sixth face
        points << p0 << p1 << p2 << p3 << p4 << p5;

        /* Fill 2D QList array with 3 corner coordinates for each of the 6 faces
         * Corners have been chosen in such a way that normal_vector() function
         * returns a vector that points in towards the centre of the block.
         */
        QList<QList<QVector3D>> face_corner_coords;
        for(int ii=0; ii<6; ii++){
            QList<QVector3D> currentSideCorners;
            currentSideCorners.append(corners.at(points.at(ii).at(0)).toQvec());
            currentSideCorners.append(corners.at(points.at(ii).at(1)).toQvec());
            currentSideCorners.append(corners.at(points.at(ii).at(2)).toQvec());
            face_corner_coords.append(currentSideCorners);
        }

        return face_corner_coords;
    }

    QVector3D GeometryFunctions::find_exit_point(Reservoir::Grid::Cell cell, QVector3D entry_point, QVector3D end_point, QVector3D exception_point)
    {
        /* takes an entry point as input and an end_point
         * which just defines the line of the well. Find
         * the two points which intersect the block faces
         * and choose the one of them which is not the entry
         * point. This will be the exit point.
         */

        QVector3D line = QVector3D(end_point - entry_point);

        // First find normal vectors of all faces of block/cell.
        QList<QList<QVector3D>> face_corner_coords = GeometryFunctions::cell_planes_coords(cell.corners());
        QList<QVector3D> normal_vectors;
        for( int ii=0; ii<6; ii++) {
            QVector3D cur_normal_vector = GeometryFunctions::normal_vector(face_corner_coords.at(ii).at(0),
                                                                           face_corner_coords.at(ii).at(1),
                                                                           face_corner_coords.at(ii).at(2));
            normal_vectors.append(cur_normal_vector);
        }

        /* For loop through all faces untill we find a face that
         * intersects with line on face of the block and not just
         * the extension of the face to a plane
         */
        for(int face_number = 0; face_number<6; face_number++) {
            // Normal vector
            QVector3D cur_normal_vector = normal_vectors[face_number];
            QVector3D cur_face_point = face_corner_coords.at(face_number).at(0);
            /* If the dot product of the line vector and the face normal vector is
             * zero then the line is paralell to the face and won't intersect it
             * unless it lies in the same plane, which in any case won't be the
             * exit point.
             */

            if(QVector3D::dotProduct(cur_normal_vector, line) != 0) {
                // Finds the intersection point of line and the current face
                QVector3D intersect_point = GeometryFunctions::line_plane_intersection(entry_point,end_point,cur_normal_vector,cur_face_point);

                /* Loop through all faces and check that intersection point is on the correct side of all of them.
                 * i.e. the same direction as the normal vector of each face
                 */
                bool feasible_point = true;
                for( int ii=0; ii<6; ii++) {
                    if(!GeometryFunctions::point_on_same_side(intersect_point, face_corner_coords.at(ii).at(0), normal_vectors[ii],10e-6)) {
                        feasible_point = false;
                    }
                }

                // If point is feasible(i.e. on/inside cell), not identical to given entry point, and going in correct direction
                if (feasible_point && intersect_point.distanceToPoint(exception_point) > 10-10
                    && QVector3D::dotProduct(end_point - entry_point, end_point - intersect_point) >= 0){
                    return intersect_point;
                }

            }

        }
        // If all fails, the line intersects the cell in a single point(corner or edge) -> return entry_point
        return entry_point;
    }

    QVector3D GeometryFunctions::project_point_to_plane(QVector3D point, QVector3D normal_vector, QVector3D plane_point)
    {
        QVector3D proj_point = point - normal_vector * (QVector3D::dotProduct(point-plane_point, normal_vector));
        QVector3D ptr_proj_point = QVector3D(proj_point);
        return ptr_proj_point;
    }

    QVector3D GeometryFunctions::project_v1_on_v2(QVector3D v1, QVector3D v2)
    {
        QVector3D proj_v = v2 * (QVector3D::dotProduct(v2, v1)/(QVector3D::dotProduct(v2, v2)) );
        QVector3D ptr_proj_v = QVector3D(proj_v);
        return ptr_proj_v;
    }

    double GeometryFunctions::well_index_cell_qvector(Reservoir::Grid::Cell cell, QList<QVector3D> start_points, QList<QVector3D> end_points, double wellbore_radius)
    {
        /* corner points of Cell(s) are always listen in the same order and orientation. (see
         * Reservoir::Grid::Cell for illustration as it is included in the code.
         * Assumption: The block is fairly regular, i.e. corners are right angles.
         * Determine the 3(orthogonal, or very close to orth.) vectors to project line onto.
         * Corners 4&5, 4&6 and 4&0 span the cell from the front bottom left corner.
         */

        QList<Reservoir::Grid::XYZCoordinate > corners = cell.corners();
        QVector3D xvec = corners[4].vectorTo(corners[5]);
        QVector3D yvec = corners[4].vectorTo(corners[6]);
        QVector3D zvec = corners[4].vectorTo(corners[0]);

        // Finds the dimensional sizes (i.e. length in each direction) of the cell block
        double dx = xvec.length();
        double dy = yvec.length();
        double dz = zvec.length();
        // Get directional permeabilities
        double kx = cell.permx();
        double ky = cell.permy();
        double kz = cell.permz();

        double Lx=0;
        double Ly=0;
        double Lz=0;

        // Need to add projections of all segments, each line is one segment.
        for (int ii = 0; ii < start_points.length(); ++ii) { // Current segment ii
            // Compute vector from segment
            QVector3D current_vec = QVector3D(end_points.at(ii) - start_points.at(ii));

            /* Proejcts segment vector to directional spanning vectors and determines the length.
             * of the projections. Note that we only only care about the length of the projection,
             * not the spatial position. Also adds the lengths of previous segments in case there
             * is more than one segment within the well.
             */
            Lx = Lx + GeometryFunctions::project_v1_on_v2(current_vec,xvec).length();
            Ly = Ly + GeometryFunctions::project_v1_on_v2(current_vec,yvec).length();
            Lz = Lz + GeometryFunctions::project_v1_on_v2(current_vec,zvec).length();
        }

        // Compute Well Index from formula provided by Shu
        double well_index_x = (GeometryFunctions::dir_well_index(Lx,dy,dz,ky,kz,wellbore_radius));
        double well_index_y = (GeometryFunctions::dir_well_index(Ly,dx,dz,kx,kz,wellbore_radius));
        double well_index_z = (GeometryFunctions::dir_well_index(Lz,dx,dy,kx,ky,wellbore_radius));
        return sqrt(well_index_x*well_index_x + well_index_y*well_index_y + well_index_z*well_index_z);
    }

    double GeometryFunctions::dir_well_index(double Lx, double dy, double dz, double ky, double kz, double wellbore_radius)
    {
        // wellbore radius should probably be taken as input. CAREFUL
        //double wellbore_radius = 0.1905;
        double silly_eclipse_factor =0.008527;
        double well_index_i = silly_eclipse_factor*(2*M_PI*sqrt(ky*kz)*Lx)/(log(GeometryFunctions::dir_wellblock_radius(dy,dz,ky,kz)/wellbore_radius));
        return well_index_i;
    }

    double GeometryFunctions::dir_wellblock_radius(double dx, double dy, double kx, double ky)
    {
        double r = 0.28*sqrt( (dx*dx)*sqrt(ky/kx) + (dy*dy)*sqrt(kx/ky) )/ ( sqrt(sqrt(kx/ky)) + sqrt(sqrt(ky/kx))  );
        return r;
    }

    bool GeometryFunctions::is_point_inside_cell(Reservoir::Grid::Cell cell, QVector3D point)
    {
        // First find normal vectors of all faces of block/cell.
        QList<QList<QVector3D>> face_corner_coords = GeometryFunctions::cell_planes_coords(cell.corners());
        QList<QVector3D> normal_vectors;
        for( int ii=0; ii<6; ii++){
            normal_vectors.append(GeometryFunctions::normal_vector(face_corner_coords.at(ii).at(0),
                                                                   face_corner_coords.at(ii).at(1),
                                                                   face_corner_coords.at(ii).at(2)));
        }

        /* For loop through all faces to check that point
         * is on right side of every face (i.e. in the same
         * direction as the computed normal vector) by
         * taking the dot product of the normal vector with
         * the vector going from one corner of the face to
         * point
         */
        bool point_inside = true;
        for(int face_number = 0; face_number<6; face_number++){
            if( QVector3D::dotProduct(point - face_corner_coords.at(face_number).at(0), normal_vectors.at(face_number)) < 0){
                point_inside = false;
            }
        }

        return point_inside;
    }

    Reservoir::Grid::Cell GeometryFunctions::get_cell_enveloping_point(Reservoir::Grid::Grid *grid, QVector3D point)
    {
        // Get total number of cells
        int total_cells = grid->Dimensions().nx*grid->Dimensions().ny*grid->Dimensions().nz;

        for(int ii=0; ii<total_cells; ii++){
            if( GeometryFunctions::is_point_inside_cell(grid->GetCell(ii),point) ){
                return grid->GetCell(ii);
            }
        }
        // Throw an exception if no cell was found
        throw std::runtime_error("WellIndexCalculator::get_cell_enveloping_point: Cell is outside grid ("
                                 + std::to_string(point.x()) + ", "
                                 + std::to_string(point.y()) + ", "
                                 + std::to_string(point.z()) + ")"
        );
    }

    QPair<QList<int>, QList<double> > GeometryFunctions::well_index_of_grid(Reservoir::Grid::Grid *grid, QList<QVector3D> start_points, QList<QVector3D> end_points, double wellbore_radius)
    {
        // Find intersected blocks and the points of intersection
        QPair<QList<int>, QList<QVector3D>> temp_pair = GeometryFunctions::cells_intersected(start_points.at(0),end_points.at(0),grid);
        QPair<QList<int>, QList<double>> pair;

        QList<double> well_indeces;
        for(int ii=0; ii<temp_pair.first.length(); ii++){
            QList<QVector3D> entry_points;
            QList<QVector3D> exit_points;
            entry_points.append(temp_pair.second.at(ii));
            exit_points.append(temp_pair.second.at(ii+1));
            well_indeces.append(GeometryFunctions::well_index_cell_qvector(grid->GetCell(temp_pair.first.at(ii)),entry_points, exit_points, wellbore_radius));
        }
        pair.first = temp_pair.first;
        pair.second = well_indeces;
        return pair;
    }

    double GeometryFunctions::movement_cost(QList<QVector3D> old_coords, QList<QVector3D> new_coords)
    {
        double n_of_points = old_coords.length();
        if(new_coords.length()!=n_of_points){throw geometryfunctions::MovementCost_VectorsNotSameLength("Lists of points are not the same length");}
        double cost_squares = 0;
        for (int ii=0; ii<n_of_points; ii++){
            cost_squares += pow(old_coords.at(ii).distanceToPoint(new_coords.at(ii)),2);
        }

        return sqrt(cost_squares);
    }

    Eigen::Vector3d GeometryFunctions::point_to_cell_shortest(Reservoir::Grid::Cell cell, Eigen::Vector3d point)
    {
        // Create a list of Eigen::Vector3d for corners.
        QList<Eigen::Vector3d> corners = cell.corners_eigen();

        // Check if point is already inside cell
        QVector3D qv_point = QVector3D(point(0),point(1),point(2));
        if(is_point_inside_cell(cell, qv_point)){
            std::cout << "point is inside cell" << std::endl;
            return point;
        }

        // Shortest distance so far
        double minimum = INFINITY;
        Eigen::Vector3d closest_point = point;

        // face_indices[ii] contain the indices to
        // find the corners that belong to face ii

        //TODO: Compare face indices with those in GeometryFunctions::cell_planes_coords
        int face_indices[6][4] = { { 0, 1 ,2, 3},
                                   { 4, 5, 6, 7},
                                   { 2, 0, 6, 4},
                                   { 1, 3, 5, 7},
                                   { 0, 1, 4, 5},
                                   { 2, 3, 6, 7} };

        // Loop through all faces.
        for(int ii = 0; ii<6; ii++){
            QList<Eigen::Vector3d> temp_face({corners.at(face_indices[ii][0]), corners.at(face_indices[ii][1]), corners.at(face_indices[ii][2]), corners.at(face_indices[ii][3]) });
            Eigen::Vector3d temp_point = point_to_face_shortest(temp_face, point, cell);
            Eigen::Vector3d projected_length = point-temp_point;
            if(projected_length.norm()<minimum){
                closest_point = temp_point;
                minimum = projected_length.norm();
            }
        }
        return closest_point;
    }

    Eigen::Vector3d GeometryFunctions::point_to_face_shortest(QList<Eigen::Vector3d> face, Eigen::Vector3d point, Reservoir::Grid::Cell cell)
    {
        /* Assumes that corner points of face are given in the following order
         * (front/back doesn't really matter here)
         *
         *          2 *---* 3
         *            |   |
         *          0 *---* 1
         */

        // Calculate normal vector and normalize
        Eigen::Vector3d vec1 = face.at(0)-face.at(1);
        Eigen::Vector3d vec2 = face.at(0)-face.at(2);
        Eigen::Vector3d n_vec = vec1.cross(vec2);
        n_vec.normalize();

        // Project point onto plane spanned by face
        Eigen::Vector3d proj_point = point - n_vec.dot(point-face.at(0))*n_vec;

        /* Check if point is inside the face.
         * Equivalently we can just check if the
         * point is inside cell
         */
        QVector3D qv_point = QVector3D(proj_point(0),proj_point(1),proj_point(2));
        if(is_point_inside_cell(cell, qv_point)){
            return proj_point;
        }

        /* If the above is false, projected point is outside face.
         * Closest point lies on one of the four lines.
         * create array for locating line segments, and
         * loop through all lines to find best point.
         */
        int line_indices[4][2] = { { 0, 1 },
                                   { 1, 3 },
                                   { 3, 2 },
                                   { 2, 0 } };
        double minimum = INFINITY;
        Eigen::Vector3d closest_point;
        for(int ii=0; ii<4; ii++){
            QList<Eigen::Vector3d> temp_line({face.at(line_indices[ii][0]),face.at(line_indices[ii][1])});
            Eigen::Vector3d temp_point = point_to_line_shortest(temp_line, point);
            Eigen::Vector3d projected_length = point-temp_point;

            if(projected_length.norm()<minimum){
                closest_point = temp_point;
                minimum = projected_length.norm();
            }
        }
        return closest_point;
    }

    Eigen::Vector3d GeometryFunctions::point_to_line_shortest(QList<Eigen::Vector3d> line_segment, Eigen::Vector3d P0)
    {
        /* Function runs through all possible combinations of
         * where the two closest points could be located. Return
         * when a solution is found. This function is a slightly
         * editet version of the one from:
         * http://www.geometrictools.com/GTEngine/Include/Mathematics/GteDistSegmentSegmentExact.h
         */

        // David Eberly, Geometric Tools, Redmond WA 98052
        // Copyright (c) 1998-2016
        // Distributed under the Boost Software License, Version 1.0.
        // http://www.boost.org/LICENSE_1_0.txt
        // http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
        // File Version: 2.1.0 (2016/01/25)d

        Eigen::Vector3d P1 = P0;
        Eigen::Vector3d Q0 = line_segment.at(0);
        Eigen::Vector3d Q1 = line_segment.at(1);

        Eigen::Vector3d P1mP0 = P1 - P0;
        Eigen::Vector3d Q1mQ0 = Q1 - Q0;
        Eigen::Vector3d P0mQ0 = P0 - Q0;

        double a = P1mP0.transpose()*P1mP0;
        double b = P1mP0.transpose()*Q1mQ0;
        double c = Q1mQ0.transpose()*Q1mQ0;
        double d = P1mP0.transpose()*P0mQ0;
        double e = Q1mQ0.transpose()*P0mQ0;
        double const zero = 0;
        double const one  = 1;
        double det = a * c - b * b;
        double s, t, nd, bmd, bte, ctd, bpe, ate, btd;

        if (det > zero) {
            bte = b * e;
            ctd = c * d;
            if (bte <= ctd) {  // s <= 0
                s = zero;
                if (e <= zero) { // t <= 0, region 6
                    t = zero;
                    nd = -d;
                    if (nd >= a)
                        s = one;
                    else if (nd > zero)
                        s = nd / a;
                    // else: s is already zero
                }
                else if (e < c) { // 0 < t < 1, region 5
                    t = e / c;
                }
                else { // t >= 1, region 4
                    t = one;
                    bmd = b - d;
                    if (bmd >= a)
                        s = one;
                    else if (bmd > zero)
                        s = bmd / a;
                    // else:  s is already zero
                }
            }
            else { // s > 0
                s = bte - ctd;
                if (s >= det) { // s >= 1
                    s = one; // s = 1
                    bpe = b + e;
                    if (bpe <= zero) { // t <= 0, region 8
                        t = zero;
                        nd = -d;
                        if (nd <= zero)
                            s = zero;
                        else if (nd < a)
                            s = nd / a;
                        // else: s is already one
                    }
                    else if (bpe < c) { // 0 < t < 1, region 1
                        t = bpe / c;
                    }
                    else { // t >= 1, region 2
                        t = one;
                        bmd = b - d;
                        if (bmd <= zero)
                            s = zero;
                        else if (bmd < a)
                            s = bmd / a;
                        // else:  s is already one
                    }
                }
                else { // 0 < s < 1
                    ate = a * e;
                    btd = b * d;
                    if (ate <= btd) { // t <= 0, region 7
                        t = zero;
                        nd = -d;
                        if (nd <= zero)
                            s = zero;
                        else if (nd >= a)
                            s = one;
                        else
                            s = nd / a;
                    }
                    else { // t > 0
                        t = ate - btd;
                        if (t >= det) { // t >= 1, region 3
                            t = one;
                            bmd = b - d;
                            if (bmd <= zero)
                                s = zero;
                            else if (bmd >= a)
                                s = one;
                            else
                                s = bmd / a;
                        }
                        else { // 0 < t < 1, region 0
                            s /= det;
                            t /= det;
                        }
                    }
                }
            }
        }
        else {
            // The segments are parallel.  The quadratic factors to R(s,t) =
            // a*(s-(b/a)*t)^2 + 2*d*(s - (b/a)*t) + f, where a*c = b^2,
            // e = b*d/a, f = |P0-Q0|^2, and b is not zero.  R is constant along
            // lines of the form s-(b/a)*t = k, and the minimum of R occurs on the
            // line a*s - b*t + d = 0.  This line must intersect both the s-axis
            // and the t-axis because 'a' and 'b' are not zero.  Because of
            // parallelism, the line is also represented by -b*s + c*t - e = 0.
            //
            // The code determines an edge of the domain [0,1]^2 that intersects
            // the minimum line, or if none of the edges intersect, it determines
            // the closest corner to the minimum line.  The conditionals are
            // designed to test first for intersection with the t-axis (s = 0)
            // using -b*s + c*t - e = 0 and then with the s-axis (t = 0) using
            // a*s - b*t + d = 0.

            // When s = 0, solve c*t - e = 0 (t = e/c).
            if (e <= zero) { // t <= 0
                // Now solve a*s - b*t + d = 0 for t = 0 (s = -d/a).
                t = zero;
                nd = -d;
                if (nd <= zero) // s <= 0, region 6
                    s = zero;
                else if (nd >= a) // s >= 1, region 8
                    s = one;
                else // 0 < s < 1, region 7
                    s = nd / a;
            }
            else if (e >= c) { // t >= 1
                // Now solve a*s - b*t + d = 0 for t = 1 (s = (b-d)/a).
                t = one;
                bmd = b - d;
                if (bmd <= zero) // s <= 0, region 4
                    s = zero;
                else if (bmd >= a) // s >= 1, region 2
                    s = one;
                else // 0 < s < 1, region 3
                    s = bmd / a;
            }
            else { // 0 < t < 1
                // The point (0,e/c) is on the line and domain, so we have one
                // point at which R is a minimum.
                s = zero;
                t = e / c;
            }
        }

        Eigen::Vector3d closest_P;
        Eigen::Vector3d closest_Q;
        closest_P = P0 + s*P1mP0;
        closest_Q = Q0 + t*Q1mQ0;
        return closest_Q;
    }


    void GeometryFunctions::print_well_index_file(Reservoir::Grid::Grid *grid, QList<QVector3D> start_points, QList<QVector3D> end_points, double wellbore_radius, double min_wi, QString filename)
    {
        // Find intersected blocks and the points of intersection
        QPair<QList<int>, QList<QVector3D>> temp_pair = GeometryFunctions::cells_intersected(start_points.at(0),end_points.at(0),grid);
        QPair<QList<int>, QList<double>> pair;

        QList<double> well_indeces;
        for(int ii=0; ii<temp_pair.first.length(); ii++){
            QList<QVector3D> entry_points;
            QList<QVector3D> exit_points;
            entry_points.append(temp_pair.second.at(ii));
            exit_points.append(temp_pair.second.at(ii+1));
            well_indeces.append(GeometryFunctions::well_index_cell_qvector(grid->GetCell(temp_pair.first.at(ii)),entry_points, exit_points, wellbore_radius));
        }
        pair.first = temp_pair.first;
        pair.second = well_indeces;

        std::ofstream myfile;
        myfile.open (filename.toUtf8().constData());
        myfile <<"-- ==================================================================================================\n";
        myfile <<"-- \n";
        myfile <<"-- Exported from ECL_5SPOT\n";
        myfile <<"-- \n";
        myfile <<"-- Exported by user hilmarm from WellIndexCalculator \n";
        myfile <<"-- \n";
        myfile <<"-- ==================================================================================================\n";
        myfile <<"\n";
        myfile <<"WELSPECS\n";
        myfile <<" 'TW01' 'PRODUC'    1    1  1712.00 'OIL'    0.0 'STD' 'SHUT' 'YES' 0 'SEG' /\n";
        myfile <<"/\n";
        myfile <<" \n";
        myfile <<"\n";
        myfile <<"GRUPTREE\n";
        myfile <<" 'PRODUC' 'FIELD' /\n";
        myfile <<" 'INJECT' 'FIELD' /\n";
        myfile <<"/\n";
        myfile <<"\n";
        myfile <<"COMPDAT\n";
        myfile <<"-- --------------------------------------------------------------------------------------------------\n";
        for( int ii = 0; ii<pair.first.length(); ii++){

            if(pair.second.at(ii)>min_wi){
                myfile << " 'TW01'" ;
                myfile << std::setw(5) << grid->GetCell(pair.first.at(ii)).ijk_index().i()+1;
                myfile << std::setw(5) << grid->GetCell(pair.first.at(ii)).ijk_index().j()+1;
                myfile << std::setw(5) << grid->GetCell(pair.first.at(ii)).ijk_index().k()+1;
                myfile << std::setw(5) << grid->GetCell(pair.first.at(ii)).ijk_index().k()+1;

                myfile << "  'OPEN' 0 " ;
                float temp = pair.second.at(ii);
                myfile << std::setprecision(5) << std::fixed;
                myfile <<std::setw(13) << temp;

                myfile.unsetf(std::ios_base::fixed);
                myfile << std::setw(8)  << wellbore_radius*2 ;

                float temp2 = grid->GetCell(pair.first.at(ii)).permx()*temp_pair.second.at(ii).distanceToPoint(temp_pair.second.at(ii+1));
                myfile << std::setprecision(5) << std::fixed;
                myfile << std::setw(13) << temp2;
                myfile << "    0.00 0.0 'X'    4.75 /\n";
                myfile.unsetf(std::ios_base::fixed);
            }
        }
        myfile << "-- --------------------------------------------------------------------------------------------------\n";
        myfile << "/";
        myfile << "\n";
        myfile << "\n";

        myfile.close();
    }

    Eigen::Vector3d GeometryFunctions::qvec_to_evec(QVector3D vec) {
        return Eigen::Vector3d(vec.x(), vec.y(), vec.z());
    }

    QVector3D GeometryFunctions::evec_to_qvec(Eigen::Vector3d vec) {
        return QVector3D(vec.x(), vec.y(), vec.z());
    }


}
