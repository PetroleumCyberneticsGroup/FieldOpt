/******************************************************************************
   Copyright (C) 2015-2018 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#ifndef WELLBLOCK_H
#define WELLBLOCK_H

#include "Model/properties/discrete_property.h"
#include "Model/wells/wellbore/completions/completion.h"
#include "Model/wells/wellbore/completions/perforation.h"
#include <Eigen/Core>

namespace Model {
namespace Wells {
namespace Wellbore {

/*!
 * \brief The WellBlock class represents a single well block. It contains references to any completion
 * defined within it.
 *
 * Note that wellblocks should use 1-indexed IJK-indices, instead of 0-indexing as is used in the rest
 * of FieldOpt and ERT. This is because the IJK-values will be used directly by the simulator interface.
 */
class WellBlock
{
  friend class Trajectory;
 public:
  WellBlock(int i, int j, int k);

  virtual ~WellBlock();

/*!
     * \brief The DirectionOfPenetration enum indicates the wells direction of penetration through
     * this block. The W value should be used to indicate that the direction could not be calculated.
     */
  enum DirectionOfPenetration : int {X=1, Y=2, Z=3, W=4};

  void AddCompletion(::Model::Wells::Wellbore::Completions::Completion *completion); //!< Add a completion to this well block.
  bool HasCompletion(); //!< Check if this well block has a completion.
  Completions::Completion *GetCompletion(); //!< Get the completion defined for this block.

  bool HasPerforation(); //!< Check if this well block has a perforation-type completion.
  Completions::Perforation *GetPerforation(); //!< Get the perforation defined for this block.

  int i() const { return i_->value(); }
  int j() const { return j_->value(); }
  int k() const { return k_->value(); }
  DirectionOfPenetration directionOfPenetration() const { return direction_of_penetration_; }

  void setI(const int i) { i_->setValue(i); }
  void setJ(const int j) { j_->setValue(j); }
  void setK(const int k) { k_->setValue(k); }
  void setDirectionOfPenetration(const DirectionOfPenetration dop) { direction_of_penetration_ = dop; }
  void setEntryPoint(const Eigen::Vector3d entry_point) { entry_point_ = entry_point; }
  void setExitPoint(const Eigen::Vector3d exit_point) { exit_point_ = exit_point; }
  Eigen::Vector3d getEntryPoint() const { return entry_point_; }
  Eigen::Vector3d getExitPoint() const { return exit_point_; }
 private:
  Model::Properties::DiscreteProperty *i_;
  Model::Properties::DiscreteProperty *j_;
  Model::Properties::DiscreteProperty *k_;
  Eigen::Vector3d entry_point_; //!< Entry point for splines through this block.
  Eigen::Vector3d exit_point_;  //!< Exit point for splines through this block.
  Completions::Completion *completion_;
  DirectionOfPenetration direction_of_penetration_; //!< The well's direction of penetration through this block.
};

}
}
}
#endif // WELLBLOCK_H
