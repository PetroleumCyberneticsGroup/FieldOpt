/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation, either version
 3 of the License, or (at your option) any later version.

 FieldOpt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty
 of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the
 GNU General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#ifndef MODEL_H
#define MODEL_H

// ---------------------------------------------------------
// QT
#include <QString>
#include <QList>

// ---------------------------------------------------------
#include "Reservoir/grid/eclgrid.h"
#include "properties/variable_property_container.h"
#include "wells/well.h"
#include "Settings/model.h"
#include "Optimization/case.h"
#include "Model/wells/wellbore/wellblock.h"
#include "Runner/loggable.hpp"
#include "Runner/logger.h"
#include "Utilities/colors.hpp"
//#include "wells/well_group.h"

// ---------------------------------------------------------
class Logger;

// ---------------------------------------------------------
namespace Model {
namespace WellGroups {
class WellGroup;
}
}

// ---------------------------------------------------------
namespace Model {
class ModelSynchronizationObject;

// ---------------------------------------------------------
/*!
 * \brief The Model class represents the reservoir model as
 * a whole, including wells and any related variables, and
 * the reservoir grid.
 */
class Model : public Loggable
{
  // -------------------------------------------------------
  friend class ModelSynchronizationObject;

 public:
  // -------------------------------------------------------
  Model(::Settings::Model* settings, Logger *logger);

  // -------------------------------------------------------
  LogTarget GetLogTarget() override;
  map<string, string> GetState() override;
  QUuid GetId() override;
  map<string, vector<double>> GetValues() override;

  // -------------------------------------------------------
  /*!
   * \brief reservoir Get the reservoir (i.e. grid).
   */
  Reservoir::Grid::Grid *grid() const
  { return grid_; }

  // -------------------------------------------------------
  /*!
   * \brief variables Get the set of variable properties
   * of all types.
   */
  Properties::VariablePropertyContainer *variables() const
  { return variable_container_; }

  // -------------------------------------------------------
  // QList<Wells::Well *> *wells_in_group(int gnr) const
  // { return well_groups_->at(gnr)->wells_; }

  QList<WellGroups::WellGroup *> *well_groups() const
  { return well_groups_; }

  // -------------------------------------------------------
  /*!
   * \brief wells Get a list of all the wells in the model.
   */
  QList<Wells::Well *> *wells() const {
    return wells_;
  }

  // -------------------------------------------------------
  /*!
   * \brief ApplyCase Applies the variable values
   * from a case to the variables in the model.
   * \param c Case to apply the variable values of.
   */
  void ApplyCase(Optimization::Case *c,
                 int rank=0);

  // -------------------------------------------------------
  /*!
   * @brief Get the UUId of last case applied to the Model.
   * @return
   */
  QUuid GetCurrentCaseId() const
  { return current_case_id_; }

  // -------------------------------------------------------
  void SetCompdatString(const QString compdat)
  { compdat_ = compdat; };

  // -------------------------------------------------------
  void SetResult(const std::string key,
                 std::vector<double> vec);

  // -------------------------------------------------------
  /*!
   * @brief Should be called at the end of the optimization
   * run. Writes the last case to the extended log.
   */
  void Finalize();

  // -------------------------------------------------------
  // Drilling sequence for all wells
  struct Drilling {
    Settings::Model::DrillingMode mode;

    // Main orderings
    vector<pair<string, pair<int, int>>> name_vs_order;
    map<string, int> name_vs_num;
    multimap<string, double> name_vs_time;

    // Transformation map
    multimap<int, pair<int, string>> mp_wells_into_groups;

    // Resulting vecotrs
    vector<vector<pair<int, string>>> wseq_grpd_sorted_name;

    // Resulting aux vectors
    vector<int> drill_groups_;
    vector<pair<string, double>> wseq_grpd_sorted_vs_time;

  };

  // -------------------------------------------------------
  /*!
   * @brief
   */
  void SetDrillingSeq();
  void GetDrillingStr();

  void UpdateNamevsTimeMap();
  void SetDrillTimeVec();

  // void UpdateControlTimes();

 private:
  // -------------------------------------------------------
  Reservoir::Grid::Grid *grid_;
  Properties::VariablePropertyContainer *variable_container_;

  QList<Wells::Well *> *wells_;
  QList<WellGroups::WellGroup *> *well_groups_;
  Drilling *drillseq_;

  // -------------------------------------------------------
  /*!
   * \brief Verify the model. Throws an exception if it is not.
   */
  void verify();

  void verifyWells();
  void verifyWellTrajectory(Wells::Well *w);
  void verifyWellBlock(Wells::Wellbore::WellBlock *wb);

  // -------------------------------------------------------
  Logger *logger_;
  QUuid current_case_id_;
  Settings::Model* settings_;

  // -------------------------------------------------------
  /*!
   * \brief Compdat list generated from the list of well
   * blocks corresponding to the current case. This is
   * set by the simulator library.
   */
  QString compdat_;

  // -------------------------------------------------------
  /*!
   * \brief The results of the last simulation (i.e.
   * the one performed with the current case).
   */
  std::map<std::string, std::vector<double>> results_;

  // -------------------------------------------------------
//  void CreateWells();
  void CreateWellGroups();


  // -------------------------------------------------------
  class Summary : public Loggable {
   public:
    Summary(Model *model) { model_  = model; }
    LogTarget GetLogTarget() override;
    map<string, string> GetState() override;
    map<string, WellDescription> GetWellDescriptions() override;
    QUuid GetId() override;
    map<string, vector<double>> GetValues() override;
   private:
    Model *model_;
  };
};

}

#endif // MODEL_H
