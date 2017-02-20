#ifndef FIELDOPT_RESERVOIRBOUNDARY_H
#define FIELDOPT_RESERVOIRBOUNDARY_H

#include "constraint.h"
#include "well_spline_constraint.h"
#include "Reservoir/grid/grid.h"

namespace Optimization {
namespace Constraints {

/*!
 * \brief The ReservoirBoundary class deals with
 * imposing and checking reservoir boundary constraints
 *
 *  The class takes a boundary as input in the form of
 *  i,j and k min,max, i.e. a 'box' in the grid i,j,k
 *  coordinates. It can check wether or not a single
 *  well is inside the given box domain and, if needed,
 *  project the well onto the domain.
 *
 *  \todo Figure out a more effective way to enforce
 *  the box constraints
 *
 *  Suggested teps:
 *  -find the edge cells of the box,
 *  -get the corner points for each of the cells,
 *  -find the corner points of the entire box (assuming then
 *  the box is a parallelogram, which may not be true for the
 *  top and bottom planes)
 *  -print the box data to log for external visualization
 *  -figure out if the current point is inside or outside
 *  the box, e.g., create a BoxEnvelopsPoint function
 *  -if outside, project point onto nearest point on plane
 */
class ReservoirBoundary : public Constraint, WellSplineConstraint
{
 public:
  ReservoirBoundary(const Settings::Optimizer::Constraint &settings,
                    Model::Properties::VariablePropertyContainer *variables,
                    Reservoir::Grid::Grid *grid);

  // Constraint interface
 public:
  bool CaseSatisfiesConstraint(Case *c);
  void SnapCaseToConstraints(Case *c);

 private:
  int imin_, imax_, jmin_, jmax_, kmin_, kmax_;
  QList<int> index_list_;
  Reservoir::Grid::Grid *grid_;
  Well affected_well_;
  QList<int> getListOfCellIndices();

  QList<int> getListOfBoxEdgeCellIndices();
  QList<int> index_list_edge_;
  QList<int> box_edge_cells_;

};
}
}

#endif //FIELDOPT_RESERVOIRBOUNDARY_H
