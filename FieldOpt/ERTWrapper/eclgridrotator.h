/******************************************************************************
   Copyright (C) 2017 Mathias Bellout [Created on 20170729]
   <mathias.bellout@ntnu.no, chakibbb@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef FIELDOPT_ECLGRIDROTATOR_H
#define FIELDOPT_ECLGRIDROTATOR_H

// FIELDOPT
#include "eclgridreader.h"
#include "Utilities/colors.hpp"

// BOOST
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/range/value_type.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

// EIGEN
#include <Eigen/Dense>

// STANDARD
#include <string>
#include <iostream>

using namespace ERTWrapper::ECLGrid;
using namespace std;
using namespace Eigen;

namespace ERTWrapper {
namespace ECLGrid {

class ECLGridRotator {

 public:

  ECLGridRotator();
  ~ECLGridRotator();

  ECLGridReader * ecl_grid_reader_;

  /*!
   * \brief
   * \param
   * \return
   */
  void GetParametersFromJSON(int argc, const char **argv);
  void GetParametersFromJSON();

  string param_file_;
  string output_file_;
  string grid_file_;
  string OUTPUT_FILE_PATH;
  string ECL_GRID_FILE_PATH;
  string ROTATION_MODE;
  MatrixXd RX_RY_RZ;
  Matrix<double,3,1> RCENTER;
  Matrix<double,3,1> ROFFSET;

  /*! JSON OBJECT STRINGS */
  string OUTPUTFILE = "OUTPUT_FILE";
  string GRIDPATH = "ECL_GRID_FILE_PATH";
  string ROTANGLE = "RX_RY_RZ_ROTATION_ANGLES";
  string ROTMODE = "RX_RY_RZ_ROTATION_MODE";
  string ROTCENTER = "RX_RY_RZ_ROTATION_CENTER";
  string ROTOFFSET = "RX_RY_RZ_ROTATION_OFFSET";

  /*! Rotation matrices */
  Matrix<double,3,3> Rx_;
  Matrix<double,3,3> Ry_;
  Matrix<double,3,3> Rz_;

  /*!
   * \brief
   * \param
   * \return
   */
  void SetRotationMatrix(MatrixXd rx_ry_rz, bool dbg=false);

  /*!
   * \brief
   * \param
   * \return
   */
  void RotateCOORD();

  /*!
   * \brief
   * \param
   * \return
   */
  void PrintCoordZcornData();

  /*!
   * \brief
   * \param
   * \return
   */
  void RotateZCORN(bool rotateGrid = false);

};

}
}

#endif //FIELDOPT_ECLGRIDROTATOR_H