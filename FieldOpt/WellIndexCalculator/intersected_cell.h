/******************************************************************************
   Copyright (C) 2015-2016 Hilmar M. Magnusson <hilmarmag@gmail.com>
   Modified by Einar J.M. Baumann (2016) <einar.baumann@gmail.com>

   This file and the WellIndexCalculator as a whole is part of the
   FieldOpt project. However, unlike the rest of FieldOpt, the
   WellIndexCalculator is provided under the GNU Lesser General Public
   License.

   WellIndexCalculator is free software: you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation, either version 3 of
   the License, or (at your option) any later version.

   WellIndexCalculator is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with WellIndexCalculator.  If not, see
   <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef FIELDOPT_INTERSECTEDCELL_H
#define FIELDOPT_INTERSECTEDCELL_H

#include "Reservoir/grid/cell.h"

namespace Reservoir {
namespace WellIndexCalculation {
    using namespace Eigen;
    /*!
     * \brief The IntersectedCell struct holds information about an intersected cell.
     */
    class IntersectedCell : public Grid::Cell {
    public:
        IntersectedCell() {}
        IntersectedCell(const Grid::Cell &cell) : Grid::Cell(cell) {};

        std::vector<Vector3d> points() const;

        Vector3d xvec() const;
        Vector3d yvec() const;
        Vector3d zvec() const;
        double dx() const;
        double dy() const;
        double dz() const;

        const Vector3d & entry_point() const;
        void set_entry_point(const Vector3d &entry_point);
        const Vector3d & exit_point() const;
        void set_exit_point(const Vector3d &exit_point);
        double well_index() const;
        void set_well_index(double well_index);

    private:
        Vector3d entry_point_;
        Vector3d exit_point_;
        double well_index_;
    };
}
}

#endif //FIELDOPT_INTERSECTEDCELL_H
