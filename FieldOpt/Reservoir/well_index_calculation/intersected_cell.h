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

        QList<Vector3d> points() const;

        Vector3d xvec() const;
        Vector3d yvec() const;
        Vector3d zvec() const;
        double dx() const;
        double dy() const;
        double dz() const;

        const Vector3d & entry_point() const;
        void set_entry_point(const Vector3d &entry_point_);
        const Vector3d & exit_point() const;
        void set_exit_point(const Vector3d &exit_point_);
        double well_index() const;
        void set_well_index(double well_index_);

    private:
        Vector3d entry_point_;
        Vector3d exit_point_;
        double well_index_;
    };
}
}

#endif //FIELDOPT_INTERSECTEDCELL_H
