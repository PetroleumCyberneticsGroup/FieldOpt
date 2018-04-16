//
// Created by joakim on 16.04.18.
//
#ifndef FIELDOPT_EIGEN_UTIL_H
#define FIELDOPT_EIGEN_UTIL_H

#include <Eigen/Core>
#include <Eigen/Dense>


inline void eigen_tail(Eigen::VectorXd &lhs, const Eigen::VectorXd &rhs, int a) {
  int d = lhs.rows() - a;
  for (int i = 0; i < a; ++i) {
    lhs[i + d] = rhs[i];
  }
}

inline void eigen_head(Eigen::VectorXd &lhs, const Eigen::VectorXd &rhs, int a) {
  for (int i = 0; i < a; ++i) {
    lhs[i] = rhs[i];
  }
}

inline void eigen_col(Eigen::MatrixXd &lhs, const Eigen::VectorXd &rhs, int a) {
  for (int i = 0; i < lhs.rows(); ++i) {
    lhs(i, a) = rhs[i];
  }
}

inline void eigen_row(Eigen::MatrixXd &lhs, const  Eigen::VectorXd &rhs, int a) {
  for (int i = 0; i < lhs.cols(); ++i) {
    lhs(a, i) = rhs[i];
  }
}

inline void eigen_block(Eigen::MatrixXd &lhs, const Eigen::MatrixXd &rhs, int startRow, int startCol) {
  for (int i = 0; i < rhs.rows(); ++i) {
    for (int j = 0; j < rhs.cols(); ++j) {
      lhs(startRow + i, startCol + j) = rhs(i, j);
    }
  }
}


#endif //FIELDOPT_EIGEN_UTIL_H