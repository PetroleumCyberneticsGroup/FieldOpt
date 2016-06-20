#ifndef GEOMETRYFUNCTIONS_H
#define GEOMETRYFUNCTIONS_H
#include "Reservoir/grid/cell.h"
#include "Reservoir/grid/grid.h"
#include "Reservoir/grid/eclgrid.h"
#include "Reservoir/grid/grid_exceptions.h"
#include "Reservoir/well_index_calculation/intersected_cell.h"
#include <QList>
#include <QPair>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>

namespace WellIndexCalculation {

/*!
 * \brief WellIndexCalculation is a toolbox for doing geometric calculations on points and lines within a Grid
 *
 * We want to collect all geometric functions necessary to determine which Cells are intercepted by a line
 * and thereafter use this to compute the Well Index for every Cell.
 */

    namespace GeometryFunctions {
        using namespace Eigen;


        /*!
         * \brief The IntersectedCell struct holds information about an intersected cell.
         */
//        struct IntersectedCell {
//            IntersectedCell() {}
//            IntersectedCell(Reservoir::Grid::Cell c, Vector3d entryp) {
//                cell = c;
//                entry_point = entryp;
//            }
//            Reservoir::Grid::Cell cell; //!< The intersected cell/
//            Vector3d entry_point; //!< The point where the well spline enters the cell, or the spline start point if it starts inside the cell.
//            Vector3d exit_point; //!< The point where the well exits the cell, or the spline end point if it ends inside the cell.
//            double well_index;
//
//            QList<Vector3d> points() {return QList<Vector3d>({entry_point, exit_point});}
//            Vector3d xvec() {return cell.corners()[5] - cell.corners()[4];}
//            Vector3d yvec() {return cell.corners()[6] - cell.corners()[4];}
//            Vector3d zvec() {return cell.corners()[0] - cell.corners()[4];}
//            double dx() {return xvec().norm();}
//            double dy() {return yvec().norm();}
//            double dz() {return zvec().norm();}
//            double kx() {return cell.permx();}
//            double ky() {return cell.permy();}
//            double kz() {return cell.permz();}
//        };


    };

}

#endif // GEOMETRYFUNCTIONS_H
