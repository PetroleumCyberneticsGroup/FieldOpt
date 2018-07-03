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

#ifndef FIELDOPT_SEGMENTED_WELL_H
#define FIELDOPT_SEGMENTED_WELL_H

#include "well.h"
#include "Model/wells/wellbore/completions/packer.h"
#include "Model/wells/wellbore/completions/icd.h"

namespace Model {
namespace Wells {

using namespace Wellbore::Completions;

/*!
 * The SegmentedWell class is an extension of the Well class, used
 * to represent wells that are to be simulated using the segmented
 * well model. The primary methods contained within this class are
 * ones used to ease the generation and validation of valid
 * segmentations for wells.
 */
class SegmentedWell : public Well {
 public:
  SegmentedWell(const Settings::Model &settings,
                int well_number,
                Properties::VariablePropertyContainer *variable_container,
                Reservoir::Grid::Grid *grid);

  /*!
   * The Compartment struct models a compartment made up of two packers and an ICD.
   * The ICD is placed at the beginning of the compartment (i.e. closest to the
   * heel of the well).
   */
  struct Compartment {
    Compartment(const double start_md, const double end_md,
                const double start_tvd, const double end_tvd,
                const Settings::Model::Well &well_settings,
                Properties::VariablePropertyContainer *variable_container,
                std::vector<Compartment> &compartments_);
    double GetLength() const;
    double GetTVDDifference() const;
    Packer *start_packer;
    Packer *end_packer;
    ICD *icd;
  };

  /*!
   * The Segment struct contains the data needed by the simulator interface
   * to build the WELSEGS, WSEGVALV and COMPSEGS keywords.
   *
   * Annulus segments should only pass through _one_ well block.
   */
  struct Segment {
    enum SegType { TUBING_SEGMENT, ANNULUS_SEGMENT, ICD_SEGMENT };
    Segment();
    Segment(SegType type, int index, int branch, int outlet,
            double length, double tvd, double diameter, double roughness);
    void AddInlet(int index);
    std::vector<int> GetInlets() const;
    void AddParentBlock(Wellbore::WellBlock *parent_block);
    void AddParentICD(Wellbore::Completions::ICD *parent_icd);
    bool    HasParentBlock() const; //!< Check whether this segment has a parent well block.
    bool    HasParentICD()   const;   //!< Check whether this segment has a parent ICD decvice.
    int     Index()     const { return index_; }
    int     Branch()    const { return branch_; }
    int     Outlet()    const { return outlet_; }
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
    double tvd_change_;                           //!< TVD _change_ along the segment (i.e. zero when perfectly horizontal)
    SegType type_;                         //!< Segment type (tubing, annulus or ICD)
    std::vector<int> inlets_;              //!< List of segment (indices) with this segment as their outlet.
    Wellbore::WellBlock *parent_block_ = 0;      //!< Parent well block for annulus segments.
    Wellbore::Completions::ICD *parent_icd_ = 0; //!< Parent ICD for ICD segments.
  };

  std::vector<Compartment> GetCompartments() const;
  std::vector<Packer *> GetPackers() const;
  std::vector<ICD *> GetICDs() const;
  std::vector<Segment> GetSegments();
  std::vector<Segment> GetTubingSegments();
  std::vector<Segment> GetICDSegments();
  std::vector<Segment> GetAnnulusSegments();


 private:
  Settings::Model::Well well_settings_;
  double tub_diam_;            //!< Tubing (inner) diameter.
  double ann_diam_;            //!< Annular diameter.
  double tub_cross_sect_area_; //!< Tubing cross section area.
  double ann_cross_sect_area_; //!< Annular cross section area.
  double tub_roughness_;       //!< Roughness for tubing segments.
  double ann_roughness_;       //!< Roughness for annulus segments.

  std::vector<Compartment> compartments_;

  std::vector<int> createTubingSegments(std::vector<Segment> &segments) const;
  std::vector<int> createICDSegments(std::vector<Segment> &segments, std::vector<int> &tubing_indexes) const;
  void createAnnulusSegments(std::vector<Segment> &segments, const std::vector<int> &icd_indexes);
};

}
}

#endif //FIELDOPT_SEGMENTED_WELL_H
