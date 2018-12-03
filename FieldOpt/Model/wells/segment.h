/******************************************************************************
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

#ifndef FIELDOPT_SEGMENT_H
#define FIELDOPT_SEGMENT_H

#include "wellbore/wellblock.h"
#include "wellbore/completions/icd.h"
#include <vector>

namespace Model {
namespace Wells {

/*!
 * The Segment struct contains the data needed by the simulator interface
 * to build the WELSEGS, WSEGVALV and COMPSEGS keywords. It is used by
 * segmented wells.
 *
 * Annulus segments should only pass through _one_ well block.
 */
class Segment {
 public:
  enum SegType { TUBING_SEGMENT, ANNULUS_SEGMENT, ICD_SEGMENT };
  Segment();
  Segment(SegType type, int index, int branch, int outlet,
          double length, double tvd, double diameter, double roughness,
          double outlet_md
  );
  void AddInlet(int index);
  std::vector<int> GetInlets() const;
  void AddParentBlock(Wellbore::WellBlock *parent_block);
  void AddParentICD(Wellbore::Completions::ICD *parent_icd);
  bool    HasParentBlock() const; //!< Check whether this segment has a parent well block.
  bool    HasParentICD()   const;   //!< Check whether this segment has a parent ICD decvice.
  Wellbore::WellBlock *ParentBlock() { return parent_block_; }
  Wellbore::Completions::ICD *ParentICD() { return parent_icd_; }
  int     Index()     const { return index_; }
  int     Branch()    const { return branch_; }
  int     Outlet()    const { return outlet_; }
  int     OutletMD()  const { return md_; }
  double  TVDChange() const { return tvd_change_; }
  double  Length()    const { return length_; }
  double  Diameter()  const { return diameter_; }
  double  Roughness() const { return roughness_; }
  SegType Type()      const { return type_; }
  std::string ToString();

 private:
  int index_;                            //!< Segment index
  int branch_;                           //!< Branch index
  int outlet_;                           //!< Index of outlet segment.
  double length_, diameter_, roughness_; //!< Physical segment properties
  double md_;                            //!< Measured depth to start of segment
  double tvd_change_;                    //!< TVD _change_ along the segment (i.e. zero when perfectly horizontal)
  SegType type_;                         //!< Segment type (tubing, annulus or ICD)
  std::vector<int> inlets_;              //!< List of segment (indices) with this segment as their outlet.
  Wellbore::WellBlock *parent_block_ = 0;      //!< Parent well block for annulus segments.
  Wellbore::Completions::ICD *parent_icd_ = 0; //!< Parent ICD for ICD segments.
};

}
}
#endif //FIELDOPT_SEGMENT_H
