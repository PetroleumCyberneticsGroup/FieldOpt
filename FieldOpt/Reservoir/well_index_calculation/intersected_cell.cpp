#include "intersected_cell.h"

namespace Reservoir {
    namespace WellIndexCalculation {

        QList<Vector3d> IntersectedCell::points() const {
            return QList<Vector3d>({entry_point_, exit_point_});
        }

        Vector3d IntersectedCell::xvec() const {
            return corners()[5] - corners()[4];
        }

        Vector3d IntersectedCell::yvec() const {
            return corners()[6] - corners()[4];
        }

        Vector3d IntersectedCell::zvec() const {
            return corners()[0] - corners()[4];
        }

        double IntersectedCell::dx() const {
            return xvec().norm();
        }

        double IntersectedCell::dy() const {
            return yvec().norm();
        }

        double IntersectedCell::dz() const {
            return zvec().norm();
        }

        const Vector3d &IntersectedCell::entry_point() const {
            return entry_point_;
        }

        void IntersectedCell::set_entry_point(const Vector3d &entry_point) {
            entry_point_ = entry_point;
        }

        const Vector3d &IntersectedCell::exit_point() const {
            return exit_point_;
        }

        void IntersectedCell::set_exit_point(const Vector3d &exit_point) {
            exit_point_ = exit_point;
        }

        double IntersectedCell::well_index() const {
            return well_index_;
        }

        void IntersectedCell::set_well_index(double well_index) {
            well_index_ = well_index;
        }
    }
}
