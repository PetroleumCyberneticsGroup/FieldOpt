
#include <gtest/gtest.h>
#include <QList>
#include <QVector3D>
#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/grid_exceptions.h"
#include "Reservoir/grid/eclgrid.h"
#include "WellIndexCalculator/geometry_functions/geometryfunctions.h"
#include "WellIndexCalculator/well_constraint_projections/well_constraint_projections.h"

using namespace WellIndexCalculator;
using namespace Reservoir::Grid;

namespace {

class WellConstraintProjectionsTests : public ::testing::Test {
protected:
    WellConstraintProjectionsTests() {
    }

    virtual ~WellConstraintProjectionsTests() {
    }

    virtual void SetUp() {

        x1 = Reservoir::Grid::XYZCoordinate(-4,20,0);
        x2 = Reservoir::Grid::XYZCoordinate(2,2,0);
        x3 = Reservoir::Grid::XYZCoordinate(-1,-2,-3);
        x4 = Reservoir::Grid::XYZCoordinate(1,-20,2);
        x5 = Reservoir::Grid::XYZCoordinate(0,0,0.2);
        coords.append(x1);
        coords.append(x2);
        coords.append(x3);
        coords.append(x4);
        coords_3d.append(x5);
        coords_3d.append(x1);
        coords_3d.append(x2);

        x_1 = QVector3D(3,2,0);
        x_2 = QVector3D(-2,2,0);
        x_3 = QVector3D(2,-2,0);
        x_4 = QVector3D(-3,-2,0);
        coords_qvec.append(x_1);
        coords_qvec.append(x_2);
        coords_qvec.append(x_3);
        coords_qvec.append(x_4);
        coords_3d_qvec.append(x_3);
        coords_3d_qvec.append(x_1);
        coords_3d_qvec.append(x_2);
        d=2;

    }

    virtual void TearDown() { }

    Reservoir::Grid::XYZCoordinate x1;
    Reservoir::Grid::XYZCoordinate x2;
    Reservoir::Grid::XYZCoordinate x3;
    Reservoir::Grid::XYZCoordinate x4;
    Reservoir::Grid::XYZCoordinate x5;

    QVector3D x_1;
    QVector3D x_2;
    QVector3D x_3;
    QVector3D x_4;

    QList<Reservoir::Grid::XYZCoordinate> coords;
    QList<Reservoir::Grid::XYZCoordinate> coords_3d;

    QList<QVector3D> coords_qvec;
    QList<QVector3D> coords_3d_qvec;

    double d;

};


TEST_F(WellConstraintProjectionsTests, sumTests) {
    EXPECT_TRUE( WellConstraintProjections::sum_i(2,3,-10)==-5 );
    EXPECT_FALSE( WellConstraintProjections::sum_i(2,3,-10)== 5 );
    EXPECT_TRUE( WellConstraintProjections::sum_ij(2,5,1)==17 );
    EXPECT_FALSE( WellConstraintProjections::sum_ij(2,5,5)==-5 );
    EXPECT_TRUE( WellConstraintProjections::prod_i(2,5,3)==30 );
    EXPECT_FALSE( WellConstraintProjections::prod_i(2,5,3)==10 );

}

TEST_F(WellConstraintProjectionsTests, root_alg_test){
    Eigen::VectorXd coeffs(7);
    coeffs(0)=1;
    for (int ii=1; ii<7; ii++){
            coeffs(ii) = 0;
    }

    coeffs(1)=-9;
    coeffs(2)=135.0/4;
    coeffs(3)=-135.0/2;
    coeffs(4)=1215.0/16;
    coeffs(5)=-729.0/16;
    coeffs(6)=729.0/64;
    Eigen::VectorXd real_roots(6);
    Eigen::VectorXd img_roots(6);
    rpoly_plus_plus::FindPolynomialRootsJenkinsTraub(coeffs,&real_roots,&img_roots);
    EXPECT_TRUE(1);
}

TEST_F(WellConstraintProjectionsTests, LU_factorisation){

    Eigen::Matrix3d A;
    A << 8, 0, 0,
         0, 6, 6,
         0, 6, 6;
    Eigen::Vector3d b;
    b << 1, 4, 2;
    QList<Eigen::Vector3d> solution_vectors = WellIndexCalculator::WellConstraintProjections::non_inv_solution(A,b);

    for(int i=0; i<solution_vectors.length(); i++){
        std::cout << "solution vector with length "<< sqrt(solution_vectors.at(i)(0)*solution_vectors.at(i)(0)
                                                          +solution_vectors.at(i)(1)*solution_vectors.at(i)(1)
                                                          +solution_vectors.at(i)(2)*solution_vectors.at(i)(2)) <<
                                          " ="<< std::endl << solution_vectors.at(i) << std::endl;
    }

}

TEST_F(WellConstraintProjectionsTests, eigen_eigensolver_test){

    using namespace std;
    using namespace Eigen;

    Eigen::Vector3d xx1;
    Eigen::Vector3d xx2;
    Eigen::Vector3d xx3;
    Eigen::Vector3d xx4;
    xx1 << x_1.x(), x_1.y(), x_1.z();
    xx2 << x_2.x(), x_2.y(), x_2.z();
    xx3 << x_3.x(), x_3.y(), x_3.z();
    xx4 << x_4.x(), x_4.y(), x_4.z();
    QList<Eigen::Vector3d> coords;
    coords.append(xx1);
    coords.append(xx2);
    coords.append(xx3);
    coords.append(xx4);
    MatrixXd A = WellIndexCalculator::WellConstraintProjections::build_A_4p(coords);

    cout << "Here is a random symmetric 5x5 matrix, A:" << endl << A << endl << endl;
    SelfAdjointEigenSolver<MatrixXd> es(A);
    cout << "The eigenvalues of A are:" << endl << es.eigenvalues() << endl;
    cout << "The matrix of eigenvectors, V, is:" << endl << es.eigenvectors() << endl << endl;
    double lambda = es.eigenvalues()[1];
    cout << "Consider the second eigenvalue, lambda = " << lambda << endl;
    VectorXd v = es.eigenvectors().col(1);
    cout << "If v is the corresponding eigenvector, then lambda * v = " << endl << lambda * v << endl;
    cout << "... and A * v = " << endl << A * v << endl << endl;
    MatrixXd D = es.eigenvalues().asDiagonal();
    MatrixXd V = es.eigenvectors();
    cout << "Finally, V * D * V^(-1) = " << endl << V * D * V.inverse() << endl;

    cout << "checking eigenvector dot products, should all be zero" << endl;
    cout <<"v1,v2 = " << es.eigenvectors().col(0).transpose()*es.eigenvectors().col(1) << endl;
    cout <<"v1,v3 = " << es.eigenvectors().col(0).transpose()*es.eigenvectors().col(2) << endl;
    cout <<"v3,v2 = " << es.eigenvectors().col(2).transpose()*es.eigenvectors().col(1) << endl;
}

TEST_F(WellConstraintProjectionsTests, eigen_kkt_solver){

    Eigen::Vector3d xx1;
    Eigen::Vector3d xx2;
    Eigen::Vector3d xx3;
    Eigen::Vector3d xx4;
    xx1 << x_1.x(), x_1.y(), x_1.z();
    xx2 << x_2.x(), x_2.y(), x_2.z();
    xx3 << x_3.x(), x_3.y(), x_3.z();
    xx4 << x_4.x(), x_4.y(), x_4.z();
    QList<Eigen::Vector3d> newcoords;
    newcoords.append(xx1);
    newcoords.append(xx2);
    newcoords.append(xx3);
    newcoords.append(xx4);

    std::cout << "shortest distance" << WellIndexCalculator::WellConstraintProjections::shortest_distance(newcoords) << std::endl;

    Eigen::Matrix3d A = WellIndexCalculator::WellConstraintProjections::build_A_4p(newcoords);
    Eigen::Vector3d b = WellIndexCalculator::WellConstraintProjections::build_b_4p(newcoords, d);
    A = WellConstraintProjections::rm_entries_eps_matrix(A,10e-10);
    std::cout <<"A = " << std::endl << A << std::endl;
    std::cout <<"b = " << std::endl << b << std::endl;
    QList<Eigen::Vector3d> moved_points = WellConstraintProjections::interwell_constraint_projection(newcoords, d);

    std::cout <<"ksi 1 = " << std::endl << xx1 << std::endl;
    std::cout <<"ksi 2 = " << std::endl << xx2 << std::endl;
    std::cout <<"ksi 3 = " << std::endl << xx3 << std::endl;
    std::cout <<"ksi 4 = " << std::endl << xx4 << std::endl;

    std::cout << "solution is " << std::endl;
    for(int i =0; i<moved_points.length(); i++){
        std::cout << "x_" << i << std::endl << moved_points.at(i) << std::endl;
    }
    if (moved_points.length() >0){
        std::cout << "with movement cost = " <<
                WellIndexCalculator::WellConstraintProjections::movement_cost(newcoords, moved_points) << std::endl;
    }

}

TEST_F(WellConstraintProjectionsTests, interwell_multiple_wells_test){

    // Create 3 wells.
    Eigen::Vector3d x1;
    Eigen::Vector3d x2;
    Eigen::Vector3d y1;
    Eigen::Vector3d y2;
    Eigen::Vector3d z1;
    Eigen::Vector3d z2;
    Eigen::Vector3d q1;
    Eigen::Vector3d q2;
    Eigen::Vector3d w1;
    Eigen::Vector3d w2;
    x1 << -4, 1, 1;
    x2 << -1, 0, 0;
    y1 << 0, 1, 3;
    y2 << 0, -1, 0;
    z1 << -3, 1, 0;
    z2 << -2, -1, -1;
    //q1 << 4, -1, -1;
    //q2 << 1, 0, -2;
    //w1 << 3, 1, 0;
    //w2 << -2, 4, -2;
    q1 << -2, -2, 0;
    q2 << -2, 2, 0;
    w1 << 0, 0, 0;
    w2 << 3, 0, 0;

    QList<Eigen::Vector3d> well_1;
    well_1.append(x1);
    well_1.append(x2);
    QList<Eigen::Vector3d> well_2;
    well_2.append(y1);
    well_2.append(y2);
    QList<Eigen::Vector3d> well_3;
    well_3.append(z1);
    well_3.append(z2);
    QList<Eigen::Vector3d> well_4;
    well_4.append(q1);
    well_4.append(q2);
    QList<Eigen::Vector3d> well_5;
    well_5.append(w1);
    well_5.append(w2);

    QList<QList<Eigen::Vector3d>> all_wells;
    //all_wells.append(well_1);
    //all_wells.append(well_2);
    //all_wells.append(well_3);
    all_wells.append(well_4);
    all_wells.append(well_5);

    QList<QList<Eigen::Vector3d>> all_wells_moved = WellIndexCalculator::WellConstraintProjections::interwell_constraint_multiple_wells(all_wells, 4, 10e-4);
    QList<QList<Eigen::Vector3d>> all_wells_length_moved = WellIndexCalculator::WellConstraintProjections::well_length_constraint_multiple_wells(all_wells,10, 5,10e-8);
    QList<QList<Eigen::Vector3d>> all_wells_both_proj = WellIndexCalculator::WellConstraintProjections::both_constraints_multiple_wells(all_wells, 4, 10e-4, 10, 5, 10e-8);

    Eigen::Vector3d temp_comp_vec;
    temp_comp_vec << -2, -2, 1;

    if( q1.isApprox(temp_comp_vec)){
        std::cout << "Vectors are almost the same" << std::endl;
    }
    else std::cout << "Vectors are NOT almost the same" << std::endl;

}


TEST_F(WellConstraintProjectionsTests, threepoint_interwell_error_test){

    // Create wells.
    double d = 300;
    Eigen::Vector3d x1;
    Eigen::Vector3d x2;
    Eigen::Vector3d y1;
    Eigen::Vector3d y2;
    x1 << 162.002, 706.854, 1712;
    x2 << 525.081, 874.706, 1712;
    y1 << 668.55, 580.227, 1712;
    y2 << 916.367, 894.213, 1712;


    QList<Eigen::Vector3d> wells;
    QList<Eigen::Vector3d> moved_wells;
    wells.append(x1);
    wells.append(x2);
    wells.append(y1);
    wells.append(y2);

    moved_wells = WellIndexCalculator::WellConstraintProjections::interwell_constraint_projection(wells, d);
    std::cout << "initial positions" << std::endl;
    for (int i=0; i<4; i++){
        std::cout << wells.at(i) << std::endl;
    }
    std::cout << "moved positions" << std::endl;
    for (int i=0; i<4; i++){
        std::cout << moved_wells.at(i) << std::endl;
    }

}

}
