////////////////////////////////////////////////////////////////////
//
// Copyright (C) Statoil ASA
// Copyright (C) Ceetron Solutions AS
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
// <http://www.gnu.org/licenses/gpl.html>
// for more details.
//
////////////////////////////////////////////////////////////////////
//
// Modified by M.Bellout on 3/6/18.
//

#ifndef FIELDOPT_RIFAULTNNC_H
#define FIELDOPT_RIFAULTNNC_H

// -----------------------------------------------------------------
//#pragma once

// -----------------------------------------------------------------
// STD
#include <string>
#include <vector>
#include <map>

// -----------------------------------------------------------------
// Qt
#include <QString>

// RESINSIGHT: FWK/VIZFWK/LIBCORE\LIBGEOMETRY ----------------------
#include "../rixx_core_geom/cvfBase.h"
#include "../rixx_core_geom/cvfVector3.h"
#include "../rixx_core_geom/cvfBoundingBox.h"

// RESINSIGHT: FWK/APPFWK/COMMONCODE\VIZEXT\PROJDATAMOD ------------
#include "../rixx_app_fwk/cvfStructGrid.h"
#include "../rixx_app_fwk/cafFixedArray.h"
#include "../rixx_app_fwk/cvfCellRange.h"

// -----------------------------------------------------------------

//#include "ricell.h"
//#include "rigrid.h"

// -----------------------------------------------------------------
class RIGrid;
class RICell;

// -----------------------------------------------------------------
using std::string;
using std::vector;
using std::map;

// ╦═╗  ╦  ╔═╗  ╔═╗  ╔╗╔  ╔╗╔  ╔═╗  ╔═╗  ╔╦╗  ╦  ╔═╗  ╔╗╔
// ╠╦╝  ║  ║    ║ ║  ║║║  ║║║  ║╣   ║     ║   ║  ║ ║  ║║║
// ╩╚═  ╩  ╚═╝  ╚═╝  ╝╚╝  ╝╚╝  ╚═╝  ╚═╝   ╩   ╩  ╚═╝  ╝╚╝
// =================================================================
class RIConnection
{
 public:
  RIConnection( )
      : m_c1GlobIdx(cvf::UNDEFINED_SIZE_T),
        m_c1Face(cvf::StructGridInterface::NO_FACE),
        m_c2GlobIdx(cvf::UNDEFINED_SIZE_T) {}

  bool hasCommonArea() const {
    return m_polygon.size() > 0;
  }

  size_t m_c1GlobIdx;
  cvf::StructGridInterface::FaceType m_c1Face;
  size_t m_c2GlobIdx;
  vector<cvf::Vec3d> m_polygon;
};

// ╦═╗  ╦  ╔╗╔  ╔╗╔  ╔═╗  ╔╦╗  ╔═╗  ╔╦╗  ╔═╗
// ╠╦╝  ║  ║║║  ║║║  ║     ║║  ╠═╣   ║   ╠═╣
// ╩╚═  ╩  ╝╚╝  ╝╚╝  ╚═╝  ═╩╝  ╩ ╩   ╩   ╩ ╩
// =================================================================
class RINNCData : public cvf::Object
{
 public:
  enum NNCResultType {
    NNC_DYNAMIC,
    NNC_STATIC,
    NNC_GENERATED
  };

  static QString propertyNameFluxWat() { return "FLRWAT"; }
  static QString propertyNameFluxOil() { return "FLROIL"; }
  static QString propertyNameFluxGas() { return "FLRGAS"; }
  static QString propertyNameCombTrans() { return "TRAN"; }
  static QString propertyNameRiCombTrans() { return "riTRAN"; }
  static QString propertyNameRiCombTransByArea() { return "riTRANbyArea"; }
  static QString propertyNameRiCombMult() { return "riMULT"; }

 public:
  RINNCData();

  void processConnections(const RIGrid& mainGrid);

  static cvf::StructGridInterface::FaceType
  calculateCellFaceOverlap(const RICell &c1,
                           const RICell &c2,
                           const RIGrid &mainGrid,
                           vector<size_t>* connectionPolygon,
                           vector<cvf::Vec3d>* connectionIntersections);


  vector<RIConnection>& connections() { return m_connections; }
  const vector<RIConnection>& connections() const { return m_connections; }

  vector<double>& makeStaticConnectionScalarResult(QString nncDataType);
  const vector<double>* staticConnectionScalarResult(size_t scalarResultIndex) const;
  const vector<double>* staticConnectionScalarResultByName(const QString& nncDataType) const;

  vector< vector<double> >& makeDynamicConnectionScalarResult(QString nncDataType, size_t timeStepCount);
  const vector< vector<double> >* dynamicConnectionScalarResult(size_t scalarResultIndex) const;
  const vector<double>* dynamicConnectionScalarResult(size_t scalarResultIndex, size_t timeStep) const;
  const vector< vector<double> >* dynamicConnectionScalarResultByName(const QString& nncDataType) const;
  const vector<double>* dynamicConnectionScalarResultByName(const QString& nncDataType, size_t timeStep) const;

  vector< vector<double> >&makeGeneratedConnectionScalarResult(QString nncDataType, size_t timeStepCount);
  const vector< vector<double> >* generatedConnectionScalarResult(size_t scalarResultIndex) const;
  const vector<double>*generatedConnectionScalarResult(size_t scalarResultIndex, size_t timeStep) const;
  vector< vector<double> >* generatedConnectionScalarResult(size_t scalarResultIndex);
  vector<double>*generatedConnectionScalarResult(size_t scalarResultIndex, size_t timeStep);
  const vector< vector<double> >* generatedConnectionScalarResultByName(const QString& nncDataType) const;
  const vector<double>* generatedConnectionScalarResultByName(const QString& nncDataType, size_t timeStep) const;
  vector< vector<double> >* generatedConnectionScalarResultByName(const QString& nncDataType);
  vector<double>* generatedConnectionScalarResultByName(const QString& nncDataType, size_t timeStep);

  vector<QString> availableProperties(NNCResultType resultType) const;

  void setScalarResultIndex(const QString& nncDataType,
                            size_t scalarResultIndex);

  bool hasScalarValues(size_t scalarResultIndex);

 private:
  const QString getNNCDataTypeFromScalarResultIndex(size_t scalarResultIndex) const;
  bool          isNative(QString nncDataType) const;

 private:
  vector<RIConnection> m_connections;
  map<QString, vector< std::vector<double> > > m_connectionResults;
  map<size_t, QString> m_resultIndexToNNCDataType;
};

////////////////////////////////////////////////////////////////////
//
// Copyright (C) Statoil ASA
// Copyright (C) Ceetron Solutions AS
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
// <http://www.gnu.org/licenses/gpl.html>
// for more details.
//
////////////////////////////////////////////////////////////////////
//
// Modified by M.Bellout on 3/6/18.
//

// ╦═╗  ╦  ╔═╗  ╔═╗  ╦ ╦  ╦    ╔╦╗  ╔═╗  ╔═╗  ╦═╗  ╔═╗  ╔═╗  ╦    ╦
// ╠╦╝  ║  ╠╣   ╠═╣  ║ ║  ║     ║   ╚═╗  ╠═╝  ╠╦╝  ║    ║╣   ║    ║
// ╩╚═  ╩  ╚    ╩ ╩  ╚═╝  ╩═╝   ╩   ╚═╝  ╩    ╩╚═  ╚═╝  ╚═╝  ╩═╝  ╩═╝
// =================================================================
class RIFaultsPrCellAccumulator : public cvf::Object
{
 public:
  enum { NO_FAULT = -1, UNKNOWN_FAULT = -2 };

 public:
  explicit RIFaultsPrCellAccumulator(size_t reservoirCellCount) {
    const int  initVals[6] = { NO_FAULT, NO_FAULT, NO_FAULT,
                               NO_FAULT, NO_FAULT, NO_FAULT};
    caf::IntArray6 initVal;
    initVal = initVals;
    m_faultIdxForCellFace.resize(reservoirCellCount, initVal);
  }

  inline int faultIdx(size_t reservoirCellIndex,
                      cvf::StructGridInterface::FaceType face) const {
    return m_faultIdxForCellFace[reservoirCellIndex][face];
  }

  inline void setFaultIdx(size_t reservoirCellIndex,
                          cvf::StructGridInterface::FaceType face,
                          int faultIdx) {
    m_faultIdxForCellFace[reservoirCellIndex][face] = faultIdx;
  }

 private:
  vector< caf::IntArray6 > m_faultIdxForCellFace;
};

////////////////////////////////////////////////////////////////////
//
// Copyright (C) Statoil ASA
// Copyright (C) Ceetron Solutions AS
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
// <http://www.gnu.org/licenses/gpl.html>
// for more details.
//
////////////////////////////////////////////////////////////////////
//
// Modified by M.Bellout on 3/6/18.
//

// ╦═╗  ╦  ╔═╗  ╔═╗  ╦ ╦  ╦    ╔╦╗
// ╠╦╝  ║  ╠╣   ╠═╣  ║ ║  ║     ║
// ╩╚═  ╩  ╚    ╩ ╩  ╚═╝  ╩═╝   ╩
// =================================================================
class RIFault : public cvf::Object
{
 public:

  struct FaultFace {
    FaultFace(size_t nativeReservoirCellIndex,
              cvf::StructGridInterface::FaceType nativeFace,
              size_t oppositeReservoirCellIndex) :
        m_nativeReservoirCellIndex(nativeReservoirCellIndex),
        m_nativeFace(nativeFace),
        m_oppositeReservoirCellIndex(oppositeReservoirCellIndex)
    { }

    size_t m_nativeReservoirCellIndex;
    cvf::StructGridInterface::FaceType m_nativeFace;
    size_t m_oppositeReservoirCellIndex;
  };

 public:
  RIFault();

  void setName(const QString& name);
  QString name() const;

  void
  addCellRangeForFace(cvf::StructGridInterface::FaceType face,
                      const cvf::CellRange& cellRange);
  void
  computeFaultFacesFromCellRanges(const RIGrid* grid);

  void
  accumulateFaultsPrCell(RIFaultsPrCellAccumulator* faultsPrCellAcc,
                         int faultIdx);

  vector<FaultFace>& faultFaces();
  const std::vector<FaultFace>& faultFaces() const;

  vector<size_t>& connectionIndices()
  { return m_connectionIndices; }

  const vector<size_t>& connectionIndices() const
  { return m_connectionIndices; }

 private:
  QString m_name;

  caf::FixedArray<vector<cvf::CellRange>, 6> m_cellRangesForFaces;

  vector<FaultFace> m_faultFaces;
  vector<size_t> m_connectionIndices;

  static cvf::ref<RIFaultsPrCellAccumulator> m_faultsPrCellAcc;
};

#endif //FIELDOPT_NNC_FAULT_H
