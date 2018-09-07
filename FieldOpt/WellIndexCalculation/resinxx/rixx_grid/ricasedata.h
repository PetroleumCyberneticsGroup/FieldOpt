////////////////////////////////////////////////////////////
//
// Copyright (C) 2011-     Statoil ASA
// Copyright (C) 2013-     Ceetron Solutions AS
// Copyright (C) 2011-2012 Ceetron AS
//
// ResInsight is free software: you can redistribute it
// and/or modify it under the terms of the GNU General
// Public License
// as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any
// later version.
//
// ResInsight is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.
//
// See the GNU General Public License at
// <http://www.gnu.org/licenses/gpl.html>
// for more details.
//
////////////////////////////////////////////////////////////
//
// Modified by M.Bellout on 3/7/18.
//

// ---------------------------------------------------------
#ifndef FIELDOPT_RICASEDATA_H
#define FIELDOPT_RICASEDATA_H

// ---------------------------------------------------------
// STD
#include <vector>
#include <set>

// ---------------------------------------------------------
// Qt
#include <QFileInfo>

// RESINSIGHT: FWK/APPFWK/COMMONCODE\VIZEXT\PROJDATAMOD ----

// RESINSIGHT: FWK/VIZFWK/LIBCORE\LIBGEOMETRY --------------

// ---------------------------------------------------------
// ERT
#include "ert/ecl/ecl_kw_magic.h"
#include "ert/ecl/ecl_nnc_export.h"
//#include "ert/ecl/ecl_nnc_data.h"
//#include "ert/ecl/ecl_nnc_geometry.h"

// ---------------------------------------------------------
#include "rigrid.h"

#include "../rixx_core_geom/cvfObjectx.h"

// ---------------------------------------------------------
using std::vector;
using std::set;

// ---------------------------------------------------------
enum PorosityModelType {
  MATRIX_MODEL,
  FRACTURE_MODEL
};

//==========================================================
// ╦═╗  ╦  ╔═╗  ╔═╗  ╔═╗  ╔═╗  ╔╦╗  ╔═╗  ╔╦╗  ╔═╗
// ╠╦╝  ║  ║    ╠═╣  ╚═╗  ║╣    ║║  ╠═╣   ║   ╠═╣
// ╩╚═  ╩  ╚═╝  ╩ ╩  ╚═╝  ╚═╝  ═╩╝  ╩ ╩   ╩   ╩ ╩
//==========================================================
class RICaseData : public cvf::Object
{
 public:
  // -------------------------------------------------------
  explicit RICaseData(string file_path);

  // destructor is called no matter what after function
  // wicalc_rixx::ComputeWellBlocks, this cannot be overriden
  // by deleting the destructor ~RICaseData() = delete; since
  // it is inherited by cvf::Object!!!
  // ~RICaseData() = delete;
  ~RICaseData();
  // The alternative is to NOT have RICaseData inherit from
  // cvf::Object, but then class RIExtractor complains because
  // it demands that the RICaseData passed to it has cvf::Object
  // members...

  // -------------------------------------------------------
  // Set RIGrid
  RIGrid* mainGrid();
  const RIGrid* mainGrid() const;
  void setMainGrid(RIGrid* mainGrid);

  // -------------------------------------------------------
  // To be removed
  // void allGrids(std::vector<RIGridBase*>* grids);

  // To be removed
  // void
  // allGrids(std::vector<const RIGridBase*>* grids) const;

  // -------------------------------------------------------
  const RIGridBase* grid(size_t index) const;
  RIGridBase* grid(size_t index);
  size_t gridCount() const;

  // -------------------------------------------------------
  // RigCaseCellResultsData*
  // results(RiaDefines::PorosityModelType porosityModel);

  // const RigCaseCellResultsData*
  // results(RiaDefines::PorosityModelType porosityModel) const;

  // -------------------------------------------------------
  // const std::vector<double>*
  // resultValues(RiaDefines::PorosityModelType porosityModel,
//                                            RiaDefines::ResultCatType type,
//                                            const QString& resultName,
//                                            size_t timeStepIndex);

  // -------------------------------------------------------
  RIActiveCellInfo*
  activeCellInfo(PorosityModelType MATRIX_MODEL);

  const RIActiveCellInfo*
  activeCellInfo(PorosityModelType MATRIX_MODEL) const;

  // -------------------------------------------------------
  void setActiveCellInfo(PorosityModelType MATRIX_MODEL,
                         RIActiveCellInfo* activeCellInfo);

  // -------------------------------------------------------
  // void setActiveFormationNames(
  //    RigFormationNames* activeFormationNames);
//  RigFormationNames* activeFormationNames();

  // -------------------------------------------------------
  // void setSimWellData(
  //    const cvf::Collection<RigSimWellData>& data);

  // -------------------------------------------------------
  // const
  // cvf::Collection<RigSimWellData>& wellResults() const
  // { return m_simWellData; }

  // -------------------------------------------------------
//  std::set<QString> findSortedWellNames() const;

  // const
  // RigSimWellData* findSimWellData(QString wellName) const;

  // const cvf::UByteArray*
  // wellCellsInGrid(size_t gridIndex);

  // const cvf::UIntArray*
  // gridCellToResultWellIndex(size_t gridIndex);

  // const RigCell&
  // cellFromWellResultCell(
  //    const RigWellResultPoint& wellResultCell) const;

  // bool findSharedSourceFace(
  //    cvf::StructGridInterface::FaceType& sharedSourceFace,
//                           const RigWellResultPoint& sourceWellCellResult,
//                           const RigWellResultPoint& otherWellCellResult) const;

  // -------------------------------------------------------
  void computeActiveCellBoundingBoxes();

  // -------------------------------------------------------
  // RiaEclipseUnitTools::UnitSystem unitsType() const
  // { return m_unitsType; }

  // void
  // setUnitsType(RiaEclipseUnitTools::UnitSystem unitsType)
  // { m_unitsType = unitsType; }

  // -------------------------------------------------------
//  std::vector<QString> simulationWellNames() const;

  // bool
  // hasSimulationWell(const QString& simWellName) const;

  // std::vector<const RigWellPath*>
  // simulationWellBranches(const QString& simWellName,
//                                                           bool includeAllCellCenters,
//                                                           bool useAutoDetectionOfBranches);

  // -------------------------------------------------------
  void set_verbosity_vector(const std::vector<int> verb_vector)
  { verb_vector_ = verb_vector; }

  std::vector<int> verb_vector() const
  { return verb_vector_; }

 private:
  // -------------------------------------------------------
  void computeActiveCellsGeometryBoundingBox();
  void computeActiveCellIJKBBox();
//  void computeWellCellsPrGrid();

 private:
  cvf::ref<RIGrid> m_mainGrid;
//  RimEclipseCase* m_ownerCase;

  // -------------------------------------------------------
  RIActiveCellInfo* m_activeCellInfo;
  RIActiveCellInfo* m_fractureActiveCellInfo;

  // -------------------------------------------------------
//  RICaseCellResultsData m_matrixModelResults;
//  RICaseCellResultsData m_fractureModelResults;

  // -------------------------------------------------------
//  cvf::ref<RigFormationNames> m_activeFormationNamesData;

  // -------------------------------------------------------
  // A WellResults object for each well in the reservoir
//  cvf::Collection<RigSimWellData> m_simWellData;

  // -------------------------------------------------------
  // A bool array pr grid with one bool pr cell
  // telling whether the cell is a well cell or not

//  cvf::Collection<cvf::UByteArray> m_wellCellsInGrid;

  // -------------------------------------------------------
  // Array pr grid with index to well pr cell telling
  // which well a cell is in
  // cvf::Collection<cvf::UIntArray>

  // m_gridCellToResultWellIndex;

  // -------------------------------------------------------
  // RiaEclipseUnitTools::UnitSystem m_unitsType;

  // -------------------------------------------------------
  // std::map<std::tuple<QString, bool, bool>,
  // cvf::Collection<RigWellPath>> m_simWellBranchCache;

  // -------------------------------------------------------
  std::vector<int> verb_vector_ = std::vector<int>(11,0);

 public:
  PorosityModelType PorosityModelTypeMATRIX_;
  PorosityModelType PorosityModelTypeFRAC_;
};

////////////////////////////////////////////////////////////
//
// Copyright (C) 2011-     Statoil ASA
// Copyright (C) 2013-     Ceetron Solutions AS
// Copyright (C) 2011-2012 Ceetron AS
//
// ResInsight is free software: you can redistribute it
// and/or modify it under the terms of the GNU General
// Public License
// as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any
// later version.
//
// ResInsight is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.
//
// See the GNU General Public License at
// <http://www.gnu.org/licenses/gpl.html>
// for more details.
//
////////////////////////////////////////////////////////////
//
// Modified by M.Bellout on 3/7/18.
//

//==========================================================
// ╦═╗  ╦  ╦═╗  ╔═╗  ╔═╗  ╔╦╗  ╔═╗  ╦═╗
// ╠╦╝  ║  ╠╦╝  ║╣   ╠═╣   ║║  ║╣   ╠╦╝
// ╩╚═  ╩  ╩╚═  ╚═╝  ╩ ╩  ═╩╝  ╚═╝  ╩╚═
// ╦  ╔╗╔  ╔╦╗  ╔═╗  ╦═╗  ╔═╗
// ║  ║║║   ║   ║╣   ╠╦╝  ╠╣
// ╩  ╝╚╝   ╩   ╚═╝  ╩╚═  ╚
//==========================================================
// Data interface base class
class RIReaderInterface : public cvf::Object
{
 public:
  // -------------------------------------------------------
  RIReaderInterface() { }
  virtual ~RIReaderInterface() { }

  // -------------------------------------------------------
//  const QString faultIncludeFileAbsolutePathPrefix();

  // -------------------------------------------------------
  virtual bool open(const QString& fileName,
                    RICaseData* eclipseCase) = 0;

  // -------------------------------------------------------
  virtual bool staticResult(const QString& result,
                            PorosityModelType MATRIX,
                            std::vector<double>* values) = 0;

  // -------------------------------------------------------
//  virtual bool dynamicResult(const QString& result,
//                             PorosityModelType MATRIX,
//                             size_t stepIndex,
//                             std::vector<double>* values) = 0;

  // -------------------------------------------------------
  void setFilenamesWithFaults(const vector<QString>& filenames)
  { m_filenamesWithFaults = filenames; }

  // -------------------------------------------------------
  vector<QString> filenamesWithFaults()
  { return m_filenamesWithFaults; }

  // -------------------------------------------------------
  // void setTimeStepFilter(
  //    const std::vector<size_t>& fileTimeStepIndices);

  // -------------------------------------------------------
  // virtual std::set<RiaDefines::PhaseType>
  // availablePhases() const;

 protected:
  // -------------------------------------------------------
  // bool isTimeStepIncludedByFilter(
  //    size_t timeStepIndex) const;

  // -------------------------------------------------------
//  size_t timeStepIndexOnFile(size_t timeStepIndex) const;

 private:
  // -------------------------------------------------------
//  const RIFaultReaderSettings*    readerSettings() const;

 private:
  std::vector<QString> m_filenamesWithFaults;
//  std::vector<size_t> m_fileTimeStepIndices;

};

//==========================================================
// ╦═╗  ╦  ╔═╗  ╔═╗  ╦    ╦═╗  ╔═╗  ╔╦╗  ╦═╗  ╔╦╗
// ╠╦╝  ║  ║╣   ║    ║    ╠╦╝  ╚═╗   ║   ╠╦╝   ║
// ╩╚═  ╩  ╚═╝  ╚═╝  ╩═╝  ╩╚═  ╚═╝   ╩   ╩╚═   ╩
// ╔╦╗  ╔═╗  ═╗ ╦
//  ║║  ╠═╣  ╔╩╦╝
// ═╩╝  ╩ ╩  ╩ ╚═
//==========================================================
// Abstract class for results access
class RIECLRestartDataAccess : public cvf::Object
{
 public:
  // -------------------------------------------------------
  RIECLRestartDataAccess();
  virtual ~RIECLRestartDataAccess();

  // -------------------------------------------------------
  virtual bool open() = 0;
  virtual void close() = 0;

  // -------------------------------------------------------
  virtual void
  setRestartFiles(const QStringList& fileSet) = 0;

  // -------------------------------------------------------
  virtual void
  setTimeSteps(const std::vector<QDateTime>& timeSteps) {};

  // -------------------------------------------------------
  virtual size_t timeStepCount() = 0;

  // -------------------------------------------------------
  virtual void timeSteps(
      std::vector<QDateTime>* timeSteps,
                         std::vector<double>* daysSinceSimulationStart) = 0;

  // -------------------------------------------------------
  virtual std::vector<int>  reportNumbers() = 0;

  // -------------------------------------------------------
  virtual void resultNames(
      QStringList* resultNames,
                           std::vector<size_t>* resultDataItemCounts) = 0;

  // -------------------------------------------------------
  virtual bool results(const QString& resultName,
                       size_t timeStep,
                       size_t gridCount,
                       std::vector<double>* values) = 0;

  // -------------------------------------------------------
  virtual bool dynamicNNCResults(
      const ecl_grid_type* grid,
                                 size_t timeStep,
                                 std::vector<double>* waterFlux,
                                 std::vector<double>* oilFlux,
                                 std::vector<double>* gasFlux) = 0;

  // -------------------------------------------------------
//  virtual void readWellData(well_info_type * well_info,
//                            bool importCompleteMswData) = 0;

  // -------------------------------------------------------
  virtual int readUnitsType() = 0;

  // -------------------------------------------------------
  // virtual std::set<RiaDefines::PhaseType>
  // availablePhases() const = 0;
};

////////////////////////////////////////////////////////////
//
// Copyright (C) 2011-     Statoil ASA
// Copyright (C) 2013-     Ceetron Solutions AS
// Copyright (C) 2011-2012 Ceetron AS
//
// ResInsight is free software: you can redistribute it
// and/or modify it under the terms of the GNU General
// Public License
// as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any
// later version.
//
// ResInsight is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.
//
// See the GNU General Public License at
// <http://www.gnu.org/licenses/gpl.html>
// for more details.
//
////////////////////////////////////////////////////////////
//
// Modified by M.Bellout on 3/7/18.
//

// ---------------------------------------------------------
typedef struct ecl_grid_struct ecl_grid_type;
typedef struct ecl_file_struct ecl_file_type;
typedef struct well_conn_struct well_conn_type;

// =========================================================
// ╦═╗  ╦  ╦═╗  ╔═╗  ╔═╗  ╔╦╗  ╔═╗  ╦═╗  ╔═╗  ╔═╗  ╦
// ╠╦╝  ║  ╠╦╝  ║╣   ╠═╣   ║║  ║╣   ╠╦╝  ║╣   ║    ║
// ╩╚═  ╩  ╩╚═  ╚═╝  ╩ ╩  ═╩╝  ╚═╝  ╩╚═  ╚═╝  ╚═╝  ╩═╝
// =========================================================
class RIReaderECL : public RIReaderInterface
{
 public:
  // -------------------------------------------------------
  RIReaderECL();
  virtual ~RIReaderECL();

  // -------------------------------------------------------
  bool open(const QString& fileName,
            RICaseData* eclipseCase);

  // -------------------------------------------------------
//  void setHdf5FileName(const QString& fileName);
  void setFileDataAccess(RIECLRestartDataAccess* restartDataAccess);

  // -------------------------------------------------------
  virtual bool openAndReadActiveCellData(const QString& fileName,
                                         const std::vector<QDateTime>& mainCaseTimeSteps,
                                         RICaseData* eclipseCase);

  // -------------------------------------------------------
  bool staticResult(const QString& result,
                    PorosityModelType MATRIX_MODEL,
                    vector<double>* values);

//  bool dynamicResult(const QString& result,
//                     PorosityModelType MATRIX_MODEL,
//                     size_t stepIndex,
//                     vector<double>* values);

//  void sourSimRlResult(const QString& result,
//                       size_t stepIndex,
//                       std::vector<double>* values);

//  std::vector<QDateTime> allTimeSteps() const;

// -------------------------------------------------------
  static bool transferGeometry(const ecl_grid_type* mainEclGrid,
                               RICaseData* eclipseCase);

  static void transferCoarseningInfo(const ecl_grid_type* eclGrid,
                                     RIGridBase* grid);

//  virtual std::set<RiaDefines::PhaseType> availablePhases() const override;

 private:
  // -------------------------------------------------------
  bool readActiveCellInfo();

//  void buildMetaData();
//  void readWellCells(const ecl_grid_type* mainEclGrid,
//                     bool importCompleteMswData);

  // -------------------------------------------------------
  std::string ertGridName( size_t gridNr );

//  RigWellResultPoint createWellResultPoint(const RigGridBase* grid,
//                                           const well_conn_type* ert_connection,
//                                           int ertBranchId, int ertSegmentId,
//                                           const char* wellName);

//  void importFaults(const QStringList& fileSet,
//                    cvf::Collection<RIFault>* faults);

  // -------------------------------------------------------
  void openInitFile();

  // -------------------------------------------------------
  void extractResultValuesBasedOnPorosityModel(
      PorosityModelType ModelType,
      vector<double>* values,
      const vector<double>& fileValues);

  // -------------------------------------------------------
//  void transferStaticNNCData(const ecl_grid_type* mainEclGrid,
//                             ecl_file_type* init_file,
//                             RIGrid* mainGrid);

  void transferDynamicNNCData(const ecl_grid_type* mainEclGrid,
                              RIGrid* mainGrid);

  // -------------------------------------------------------
  void ensureDynamicResultAccessIsPresent();

//  QStringList validKeywordsForPorosityModel(const QStringList& keywords,
//                                            const std::vector<size_t>& keywordDataItemCounts,
//                                            const RigActiveCellInfo* activeCellInfo,
//                                            const RigActiveCellInfo* fractureActiveCellInfo,
//                                            RiaDefines::PorosityModelType matrixOrFracture,
//                                            size_t timeStepCount) const;

//  std::vector<RigEclipseTimeStepInfo> createFilteredTimeStepInfos();

//  static bool isEclipseAndSoursimTimeStepsEqual(const QDateTime& eclipseDateTime,
//                                                const QDateTime& sourSimDateTime);

 private:
  // -------------------------------------------------------
  // Name of file used to start accessing Eclipse output files
  QString m_fileName;

  // Set of files in filename's path with same base name as filename
  QStringList m_filesWithSameBaseName;

  RICaseData* m_eclipseCase;

  // File access to static results
  ecl_file_type* m_ecl_init_file;

  // File access to dynamic results
  mutable cvf::ref<RIECLRestartDataAccess> m_dynamicResultsAccess;

//  std::unique_ptr<RIHdf5ReaderInterface> m_hdfReaderInterface;
};

////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011-     Statoil ASA
// Copyright (C) 2013-     Ceetron Solutions AS
// Copyright (C) 2011-2012 Ceetron AS
//
// ResInsight is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// ResInsight is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// See the GNU General Public License at
// <http://www.gnu.org/licenses/gpl.html> for more details.
//
////////////////////////////////////////////////////////////////////
//
// Modified by M.Bellout on 3/7/18.
//

//==========================================================
// ╦═╗  ╦  ╔═╗  ╔═╗  ╦    ╔═╗  ╦   ╔╦╗  ╔═╗  ╔═╗  ╦    ╔═╗
// ╠╦╝  ║  ║╣   ║    ║    ╠╣   ║    ║   ║ ║  ║ ║  ║    ╚═╗
// ╩╚═  ╩  ╚═╝  ╚═╝  ╩═╝  ╚    ╩═╝  ╩   ╚═╝  ╚═╝  ╩═╝  ╚═╝
//==========================================================
class RIECLFileTools {
 public:
  // -------------------------------------------------------
  RIECLFileTools();
  virtual ~RIECLFileTools();

  // -------------------------------------------------------
  static QString
  firstFileNameOfType(const QStringList& fileSet,
                                     ecl_file_enum fileType);

  // -------------------------------------------------------
  static QStringList
  filterFileNamesOfType(const QStringList& fileSet,
                                           ecl_file_enum fileType);

  // -------------------------------------------------------
  static bool
  findSiblingFilesWithSameBaseName(const QString& fileName,
                                               QStringList* fileSet);

  // -------------------------------------------------------
  static bool
  keywordData(ecl_file_type* ecl_file,
                          const QString& keyword,
                          size_t fileKeywordOccurrence,
                          std::vector<double>* values);

  // -------------------------------------------------------
  static bool keywordData(ecl_file_type* ecl_file,
                          const QString& keyword,
                          size_t fileKeywordOccurrence,
                          std::vector<int>* values);

  // -------------------------------------------------------
  static cvf::ref<RIECLRestartDataAccess>
  createDynamicResultAccess(const QString& fileName);

};

////////////////////////////////////////////////////////////
//
// Copyright (C) 2011-     Statoil ASA
// Copyright (C) 2013-     Ceetron Solutions AS
// Copyright (C) 2011-2012 Ceetron AS
//
// ResInsight is free software: you can redistribute it
// and/or modify it under the terms of the GNU General
// Public License
// as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any
// later version.
//
// ResInsight is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.
//
// See the GNU General Public License at
// <http://www.gnu.org/licenses/gpl.html>
// for more details.
//
////////////////////////////////////////////////////////////
//
// Modified by M.Bellout on 3/8/18.
//

// =========================================================
// ╦═╗  ╦  ╦╔═  ╔═╗  ╦ ╦  ╦ ╦  ╔═╗  ╦═╗  ╔╦╗  ╦    ╔═╗  ╔═╗
// ╠╦╝  ║  ╠╩╗  ║╣   ╚╦╝  ║║║  ║ ║  ╠╦╝   ║║  ║    ║ ║  ║
// ╩╚═  ╩  ╩ ╩  ╚═╝   ╩   ╚╩╝  ╚═╝  ╩╚═  ═╩╝  ╩═╝  ╚═╝  ╚═╝
// =========================================================
class RIKeywordLocation {

 public:
  // -------------------------------------------------------
  RIKeywordLocation(const std::string& keyword,
                     size_t itemCount,
                     int indexWithinReportStep)
      : m_keyword(keyword),
        m_itemCount(itemCount),
        m_indexWithinReportStep(indexWithinReportStep) {}

  // -------------------------------------------------------
  std::string keyword() const { return m_keyword; }
  size_t itemCount() const { return m_itemCount; }

  // -------------------------------------------------------
  int indexWithinReportStep() const
  { return m_indexWithinReportStep; }

 private:
  // -------------------------------------------------------
  std::string m_keyword;
  size_t m_itemCount;
  int m_indexWithinReportStep;
};

// =========================================================
// ╦═╗  ╦  ╦═╗  ╔═╗  ╔╦╗  ╦═╗  ╔╦╗  ╦═╗  ╔═╗  ╦═╗  ╔╦╗
// ╠╦╝  ║  ╠╦╝  ╚═╗   ║   ╠╦╝   ║   ╠╦╝  ╠═╝  ╠╦╝   ║
// ╩╚═  ╩  ╩╚═  ╚═╝   ╩   ╩╚═   ╩   ╩╚═  ╩    ╩╚═   ╩
// ╦╔═  ╦ ╦  ╦═╗  ╔╦╗  ╔═╗
// ╠╩╗  ║║║  ╠╦╝   ║║  ╚═╗
// ╩ ╩  ╚╩╝  ╩╚═  ═╩╝  ╚═╝
// =========================================================
class RIRestartReportKeywords {

 public:
  // -------------------------------------------------------
  RIRestartReportKeywords();

  // -------------------------------------------------------
  void appendKeyword(const std::string& keyword,
                     size_t itemCount, int globalIndex);

  // -------------------------------------------------------
  std::vector<std::string> keywordsWithItemCountFactorOf(
      const std::vector<size_t>& factorCandidates);

  // -------------------------------------------------------
  std::vector<std::pair<std::string, size_t> >
  keywordsWithAggregatedItemCount();

 private:
  // -------------------------------------------------------
  std::vector<RIKeywordLocation>
  objectsForKeyword(const std::string& keyword);

  std::set<std::string> uniqueKeywords();

 private:
  // -------------------------------------------------------
  std::vector<RIKeywordLocation> m_keywordNameAndItemCount;
};

// =========================================================
// ╦═╗  ╦  ╦═╗  ╔═╗  ╔╦╗  ╦═╗  ╔╦╗  ╦═╗  ╔═╗  ╦═╗
// ╠╦╝  ║  ╠╦╝  ╚═╗   ║   ╠╦╝   ║   ╠╦╝  ╠═╝  ╠╦╝
// ╩╚═  ╩  ╩╚═  ╚═╝   ╩   ╩╚═   ╩   ╩╚═  ╩    ╩╚═
// ╔╦╗  ╔═╗  ╔╦╗  ╔═╗  ╔═╗
//  ║   ╚═╗   ║   ║╣   ╠═╝
//  ╩   ╚═╝   ╩   ╚═╝  ╩
// =========================================================
class RIRestartReportStep {

 public:
  // -------------------------------------------------------
  //int globalIndex;
  QDateTime dateTime;

  RIRestartReportKeywords m_keywords;
};

////////////////////////////////////////////////////////////
//
// Copyright (C) 2011-     Statoil ASA
// Copyright (C) 2013-     Ceetron Solutions AS
// Copyright (C) 2011-2012 Ceetron AS
//
// ResInsight is free software: you can redistribute it
// and/or modify it under the terms of the GNU General
// Public License
// as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any
// later version.
//
// ResInsight is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.
//
// See the GNU General Public License at
// <http://www.gnu.org/licenses/gpl.html>
// for more details.
//
////////////////////////////////////////////////////////////
//
// Modified by M.Bellout on 3/8/18.
//

// =========================================================
// ╦═╗  ╦  ╔═╗  ╔═╗  ╦    ╦ ╦  ╔╗╔  ╦═╗  ╔═╗  ╔╦╗  ╦═╗  ╔╦╗
// ╠╦╝  ║  ║╣   ║    ║    ║ ║  ║║║  ╠╦╝  ╚═╗   ║   ╠╦╝   ║
// ╩╚═  ╩  ╚═╝  ╚═╝  ╩═╝  ╚═╝  ╝╚╝  ╩╚═  ╚═╝   ╩   ╩╚═   ╩
// ╔═╗  ╦  ╦    ╔═╗  ╔═╗  ═╗ ╦
// ╠╣   ║  ║    ║╣   ╠═╣  ╔╩╦╝
// ╚    ╩  ╩═╝  ╚═╝  ╩ ╩  ╩ ╚═
// =========================================================
// Class for access to results from a unified restart file
class RIECLUnifiedRestartFileAccess :
    public RIECLRestartDataAccess {

 public:
  // -------------------------------------------------------
  RIECLUnifiedRestartFileAccess();
  virtual ~RIECLUnifiedRestartFileAccess();

//  void setRestartFiles(const QStringList& fileSet);
//  bool open();
//  void close();
//
//  size_t timeStepCount();
//  void   timeSteps(std::vector<QDateTime>* timeSteps,
//                   std::vector<double>* daysSinceSimulationStart) override;
//  std::vector<int> reportNumbers();
//
//  void resultNames(QStringList* resultNames,
//                   std::vector<size_t>* resultDataItemCounts);
//  bool results(const QString& resultName,
//               size_t timeStep,
//               size_t gridCount,
//               std::vector<double>* values);
//
//  bool dynamicNNCResults(const ecl_grid_type* grid,
//                         size_t timeStep,
//                         std::vector<double>* waterFlux,
//                         std::vector<double>* oilFlux,
//                         std::vector<double>* gasFlux) override;
//
////  virtual void readWellData(well_info_type * well_info,
////                            bool importCompleteMswData);
//  virtual int readUnitsType();
//
////  virtual std::set<RiaDefines::PhaseType> availablePhases() const override;
//
// private:
//  bool openFile();
//  bool useResultIndexFile() const;
//  void extractTimestepsFromEclipse();
//
// private:
//  QString m_filename;
  ecl_file_type* m_ecl_file;
//
//  std::vector<QDateTime> m_timeSteps;
//  std::vector<double> m_daysSinceSimulationStart;
//  std::vector<int> m_reportNr;

//  std::set<RiaDefines::PhaseType> m_availablePhases;
};

////////////////////////////////////////////////////////////
//
// Copyright (C) 2011-     Statoil ASA
// Copyright (C) 2013-     Ceetron Solutions AS
// Copyright (C) 2011-2012 Ceetron AS
//
// ResInsight is free software: you can redistribute it
// and/or modify it under the terms of the GNU General
// Public License
// as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any
// later version.
//
// ResInsight is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.
//
// See the GNU General Public License at
// <http://www.gnu.org/licenses/gpl.html>
// for more details.
//
////////////////////////////////////////////////////////////
//
// Modified by M.Bellout on 3/8/18.
//

//==========================================================
// ╦═╗  ╦  ╔═╗  ╔═╗  ╦    ╦═╗  ╔═╗  ╔╦╗  ╦═╗  ╔╦╗
// ╠╦╝  ║  ║╣   ║    ║    ╠╦╝  ╚═╗   ║   ╠╦╝   ║
// ╩╚═  ╩  ╚═╝  ╚═╝  ╩═╝  ╩╚═  ╚═╝   ╩   ╩╚═   ╩
// ╔═╗  ╦  ╦    ╔═╗  ╔═╗  ╔═╗  ╔╦╗  ╔═╗  ═╗ ╦
// ╠╣   ║  ║    ║╣   ╚═╗  ║╣    ║   ╠═╣  ╔╩╦╝
// ╚    ╩  ╩═╝  ╚═╝  ╚═╝  ╚═╝   ╩   ╩ ╩  ╩ ╚═
//==========================================================
// Class for access to results from a set of restart files
class RIECLRestartFilesetAccess :
    public RIECLRestartDataAccess
{
 public:
  // -------------------------------------------------------
//  RIECLRestartFilesetAccess();
//  virtual ~RIECLRestartFilesetAccess();
//
//  bool                        open();
//  void                        setRestartFiles(const QStringList& fileSet);
//  void                        close();
//
//  void                        setTimeSteps(const std::vector<QDateTime>& timeSteps);
//  size_t                      timeStepCount();
//  void                        timeSteps(std::vector<QDateTime>* timeSteps, std::vector<double>* daysSinceSimulationStart) override;
//  std::vector<int>            reportNumbers();
//
//  void                        resultNames(QStringList* resultNames, std::vector<size_t>* resultDataItemCounts);
//  bool                        results(const QString& resultName, size_t timeStep, size_t gridCount, std::vector<double>* values);
//
//  bool                        dynamicNNCResults(const ecl_grid_type* grid, size_t timeStep, std::vector<double>* waterFlux, std::vector<double>* oilFlux, std::vector<double>* gasFlux) override;
//
////  virtual void                readWellData(well_info_type* well_info, bool importCompleteMswData);
//  virtual int                 readUnitsType();
//
////  virtual std::set<RiaDefines::PhaseType> availablePhases() const override;
//
// private:
//  void                        openTimeStep(size_t timeStep);
//
// private:
//  QStringList                     m_fileNames;
//  std::vector<QDateTime>          m_timeSteps;
//  std::vector<double>             m_daysSinceSimulationStart;
//
//  std::vector< ecl_file_type* >   m_ecl_files;
////  std::set<RiaDefines::PhaseType> m_availablePhases;
};


#endif //FIELDOPT_RICASEDATA_H
