/***********************************************************
 Copyright (C) 2017
 Mathias C. Bellout <mathias.bellout@ntnu.no>

 Created by bellout on 5/6/18.

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it
 and/or modify it under the terms of the GNU General
 Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at
 your option) any later version.

 FieldOpt is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#include "wicalc_rixx.h"

// ---------------------------------------------------------
using std::cout;
using std::endl;
using std::list;
using std::pair;
using std::string;
using std::fill;
using std::vector;
using std::stringstream;

// ---------------------------------------------------------
#include <memory>
#include <Utilities/verbosity.h>
#include <Utilities/printer.hpp>
#include <boost/lexical_cast.hpp>

// ---------------------------------------------------------
enum CompletionType {
  WELL_PATH
};

// ---------------------------------------------------------
namespace Reservoir {
namespace WellIndexCalculation {

// =========================================================
wicalc_rixx::wicalc_rixx(Grid::Grid *grid,
                         RICaseData *ricasedata) {

  // -------------------------------------------------------
  // cout << "[mod]wicalc_rixx-01.--------- " << endl;
  grid_ = grid;
  // ricasedata_ = ricasedata;

  // Experimental
  // ricasedatap_ = ricasedata;
  // ricasedatac_ = ricasedatap_;

  // ---------------------------------------------------------------
  // std::cout << "[mod]wicalc_rixx-01.--------- " << std::endl;

  RIReaderECL rireaderecl_;
  ricasedata_ = new RICaseData(grid_->GetGridFilePath());
  rireaderecl_.open(QString::fromStdString(grid_->GetGridFilePath()), ricasedata_.p());

  ricasedata_->computeActiveCellBoundingBoxes();
  ricasedata_->mainGrid()->computeCachedData();

  // ---------------------------------------------------------------
  // cout << "[mod]wicalc_rixx-02.--------- " << endl;
  auto grid_count = ricasedata_->mainGrid()->gridCount();
  auto cell_count = ricasedata_->mainGrid()->cellCount();
  auto gcellarray_sz = ricasedata_->mainGrid()->globalCellArray().size();

  // ---------------------------------------------------------------
  intersections_.resize(ricasedata_->mainGrid()->globalCellArray().size());
  fill(intersections_.begin(), intersections_.end(), HUGE_VAL);
  // cout << "[mod]wicalc_rixx-03.--------- " << endl;

  // ---------------------------------------------------------------
  // Dbg
  QDateTime tstart = QDateTime::currentDateTime();
  std::stringstream str; str << "Find cell from coords.";

  str.str(""); str << "grid_->gridCount(): " << grid_count
                   << " -- grid_->cellCount(): " << cell_count
                   << " -- grid_->globalCellArray().size(): "
                   << gcellarray_sz;

}

// -----------------------------------------------------------------
wicalc_rixx::~wicalc_rixx() {
  // cout << "[wic-rixx]deleting vars.----- wicalc_rixx()" << endl;
  // delete ricasedata_;
}

// -----------------------------------------------------------------
void wicalc_rixx::calculateWellPathIntersections(const WellPath& wellPath,
                                                 vector<double> &isc_values) {

  vector<cvf::HexIntersectionInfo> intersections =
      WellPath::findRawHexCellIntersections(ricasedata_->mainGrid(),
                                            wellPath.m_wellPathPoints);

  std::stringstream str;
  if (intersections.size() > 0) {
    str << "Set values to WELL_PATH. -- # of intersections = " << intersections.size()
        << " -- intersection[0].m_hexIndex = " << intersections[0].m_hexIndex;
    print_dbg_msg_wic_ri(__func__, str.str(), 0.0, 0);
  }

  for (auto &intersection : intersections) {

    str.str("");
    str << "intersection.m_hexIndex = "
        << intersection.m_hexIndex
        << " -- values[intersection.m_hexIndex] = "
        << isc_values[intersection.m_hexIndex];
    // print_dbg_msg_wic_ri(__func__, str.str(), 0.0, 0);
    isc_values[intersection.m_hexIndex] = CompletionType::WELL_PATH;
  }
}

// -----------------------------------------------------------------
void
wicalc_rixx::collectIntersectedCells(vector<IntersectedCell> &isc_cells,
                                     vector<WellPathCellIntersectionInfo> isc_info,
                                     WellDefinition well,
                                     WellPath& wellPath) {

  vector<RICompData> completionData;
  Reservoir::Grid::Cell gcell;

  for (auto& cell : isc_info) {

    // -------------------------------------------------------------
    // Get IJK idx
    size_t i, j, k;
    ricasedata_->mainGrid()->ijkFromCellIndex(cell.globCellIndex, &i, &j, &k);

    // Check if cell is active, if not, skip
    bool cellIsActive = activeCellInfo_->isActive(cell.globCellIndex);
    //bool cellIsActiveF = fractureActiveCellInfo_->isActive(cell.globCellIndex);
    if (!cellIsActive) {
      // cout << "Cell is not active" << endl;
      continue;
    }

    // -------------------------------------------------------------
    // Make RI Completion object
    RICompData completion(QString::fromStdString(well.wellname),
                          IJKCellIndex(i, j, k));

    // -------------------------------------------------------------
    // Make FO Cell object + fill values for trans.computation
    gcell = grid_->GetCell(cell.globCellIndex);
    IntersectedCell icell(gcell);

    // -------------------------------------------------------------
    // Calculate direction
    CellDir direction =
        wellPath.calculateDirectionInCell(cell,
                                          icell);

    // -------------------------------------------------------------
    // Calculate transmissibility
    double transmissibility =
        wellPath.calculateTransmissibility(cell.intersectionLengthsInCellCS,
                                           well.skins[0],
                                           well.radii[0],
                                           cell.globCellIndex,
                                           false, icell);

    // -------------------------------------------------------------
    // Deleted in susbsequent versions
    // Store calculated values in RI completion object (for
    // completeness sake, not necessary for our transfer)
    // completion.setTransAndWPImultBackgroundDataFromPerforation(transmissibility,
    //                                                            well.skins[0],
    //                                                            well.radii[0],
    //                                                            direction);
    // completion.addMetadata("Perforation",
    //                        QString("StartMD: %1 - EndMD: %2")
    //                            .arg(wellPath_->m_measuredDepths[0])
    //                            .arg(wellPath_->m_measuredDepths[1])
    //                            + QString(" : ") + QString::number(transmissibility));
    // completionData.push_back(completion);

    // -------------------------------------------------------------
    // Convert start + exit points + calculated lengths in cell for
    // transfer to FO object
    Vector3d start_pt(cell.startPoint.x(),
                      cell.startPoint.y(),
                      cell.startPoint.z());

    Vector3d exit_pt(cell.endPoint.x(),
                     cell.endPoint.y(),
                     cell.endPoint.z());

    Vector3d isc_lengths(cell.intersectionLengthsInCellCS.x(),
                         cell.intersectionLengthsInCellCS.y(),
                         cell.intersectionLengthsInCellCS.z());

    // -------------------------------------------------------------
    // Transfer segment data, incl. wccf, to FO intersected cell
    icell.add_new_segment(start_pt, exit_pt, well.radii[0], well.skins[0]);
    icell.set_cell_well_index_matrix(transmissibility);

    // Add to vector of intersected cells
    isc_cells.push_back(icell);
  }
}

// =========================================================
void
wicalc_rixx::ComputeWellBlocks(
    vector<IntersectedCell> &well_indices,
    WellDefinition &well) {

  // -------------------------------------------------------
  stringstream str;
  cvf::ref<WellPath> wellPath = nullptr;
  cvf::ref<RIExtractor> extractor = nullptr;

  // -------------------------------------------------------------
  // Intersected cells for well
  vector<IntersectedCell> intersected_cells;

  // -------------------------------------------------------------
  // Loop through well segments
  // for (int iSegment = 0; iSegment < wells[iWell].radii.size(); ++iSegment) {
  int iSegment = 0;

  // -----------------------------------------------------------
  // cvf::ref<WellPath> wellPath = new WellPath();
  wellPath = new WellPath();
  vector<WellPathCellIntersectionInfo> intersectionInfos;

  // -----------------------------------------------------------
  // Load measuredepths onto wellPath (= current segment)
  wellPath->m_measuredDepths.push_back(well.heel_md[iSegment]);
  wellPath->m_measuredDepths.push_back(well.toe_md[iSegment]);

  // -----------------------------------------------------------
  // Load wellpathpoints onto wellPath (= current segment)
  cvf::Vec3d cvf_xyzHeel(well.heels[iSegment][0],
                         well.heels[iSegment][1],
                         -well.heels[iSegment][2]);

  cvf::Vec3d cvf_xyzToe(well.toes[iSegment][0],
                        well.toes[iSegment][1],
                        -well.toes[iSegment][2]);

  // -----------------------------------------------------------
  wellPath->m_wellPathPoints.push_back(cvf_xyzHeel);
  wellPath->m_wellPathPoints.push_back(cvf_xyzToe);

  // -----------------------------------------------------------
  // Calculate cells intersected by well path
  calculateWellPathIntersections(*wellPath, intersections_);
  // cout << "[mod]wicalc_rixx-05.--------- calculateWellPathIntersections" << endl;

  // -----------------------------------------------------------
  // Use intersection data to find intersected cell data
  // cvf::ref<RIExtractor> extractor = new RIECLExtractor(ricasedata, *wellPath);
  extractor = new RIECLExtractor(ricasedata_.p(), *wellPath);
  // cout << "[mod]wicalc_rixx-06.--------- cvf::ref<RIExtractor> extractor" << endl;

  // -----------------------------------------------------------
  activeCellInfo_ = ricasedata_->activeCellInfo(MATRIX_MODEL);
  //fractureActiveCellInfo_ = ricasedata_->activeCellInfo(FRACTURE_MODEL);
  // cout << "[mod]wicalc_rixx-07.--------- activeCellInfo_" << endl;

  // -----------------------------------------------------------
  vector<WellPathCellIntersectionInfo>
      intersectedCellInfo = extractor->cellIntersectionInfosAlongWellPath();
  // cout << "[mod]wicalc_rixx-08.--------- intersectedCellInfo" << endl;
    if (VERB_WIC >= 3) {
        for (auto celli : intersectedCellInfo) {
          auto gci = celli.globCellIndex;
          auto emd = celli.endMD;
          auto smd = celli.startMD;
          auto spt = celli.startPoint;
          auto ept = celli.endPoint;
          std::stringstream ss;
          ss << "Global cell index: " << gci << " | StartMD: " << smd << " | EndMD: " << emd;
          ss << "Start point: " << spt.x() << ", " << spt.y() << ", " << spt.y();
          ss << " | End point: "   << ept.x() << ", " << ept.y() << ", " << ept.y();
          Printer::ext_info(ss.str(), "WellIndexCalculation", "wicalc_rixx");
        }
    }

  // -----------------------------------------------------------
  collectIntersectedCells(intersected_cells,
                          intersectedCellInfo,
                          well,
                          *wellPath);

  if (VERB_WIC >= 2) {
    Printer::ext_info("Found " + boost::lexical_cast<std::string>(intersected_cells.size())
                          + " intersected cells.", "WellIndexCalculation, wicalc_rixx");
  }


  // Assign intersected cells to well
  well_indices = intersected_cells;

}
// -----------------------------------------------------------------

}
}
