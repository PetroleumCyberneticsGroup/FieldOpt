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

#ifndef WELLCONTROLS_H
#define WELLCONTROLS_H

#include <QMap>

#include "ecldriverpart.h"
#include "Model/wells/well.h"

namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace ECLDriverParts {

/*!
 * \brief The WellControls class generates the string for the well controls, using the
 * WCONPROD, WCONINJE and TIME keywords.
 *
 * The information is primarily taken from the Model::Wells::Control class.
 *
 * \todo This only supports liquid rate control. Update settings with more keywords to support more rate types.
 *
 * \note This class contains some messy code that should be replaced in time. For now, the new code
 * (the code for specific time steps) is only used by the ECLIPSE interface. All interfaces should
 * be updated to use this code.
 */
class WellControls : public ECLDriverPart
{
 public:
  WellControls(const QList<Model::Wells::Well *> *wells, const QList<int> &control_times);

  /*!
   * Create control entries for a new time step. This includes the time progression keyword
   * from this timestep up to the next timestep.
   * @param control_times Vector of all timesteps, needed for time progression keywords.
   * @param timestep Timestep for which to generate entries.
   */
  WellControls(const QList<Model::Wells::Well *> *wells, const QList<int> &control_times, const int &timestep);

  WellControls(){}

  virtual QString GetPartString() const;

 protected:

  /*!
   * Flag to be used to indicate if this instance is used to generate
   * entries for one specific timestep. It will cause GetPartString
   * to call the getTimestepPartString() method instead of using
   * its old mode of operation.
   */
  bool specific_timestep_;

  /*!
   * Get the part string when this instance only specifies controls for a particular
   * timestep.
   */
  QString getTimestepPartString() const;

  /*!
   * Find the next timestep after the one provided as an argument. Returns -1 if the one
   * provided is the last timestep.
   */
  int nextTimestep(const int &timestep, const QList<int> &control_times) const;

  /*!
   * \brief The WellSetting struct is a convenience class representing all settings
   * for a well at a particular time.
   */
  struct WellSetting {
    WellSetting(const QString &well_name, const bool &is_injector, const Model::Wells::Control &control);
    QString well_name; //!< Name of the well this setting belongs to.
    bool is_injector; //!< True if this setting is for an injector, otherwise false.
    Model::Wells::Control control; //!< The control entry for this setting.
  };

  /*!
   * \brief The TimeEntry struct is a convenience class representing a time entry
   * along with the well settings for that time.
   */
  struct TimeEntry {
    int time;
    bool has_well_setting = false;
    QList<WellSetting> well_settings;
  };

  QMap<int, TimeEntry> time_entries_;
  void initializeTimeEntries(const QList<Model::Wells::Well *> *wells, const QList<int> &control_times);
  void initializeTimeEntries(const QList<Model::Wells::Well *> *wells, const QList<int> &control_times, const int &timestep);

  /*!
   * \brief createTimeEntry Creates a string on the form
   * \code
   * TIME
   *    25 /
   *
   * \endcode
   * Including the trailing blank line.
   * \param time The time step to be inserted.
   * \return
   */
  virtual QString createTimeEntry(const double &time, const double &prev_time) const;
  QString createProducerEntry(const WellSetting &setting) const;

  /*!
   * \brief createInjectorEntry Creates a string on the form
   * \code
   * WCONPROD
   *    PROD OPEN LRAT 2* 5000 1000 /
   * /
   *
   * \endcode
   * Including the trailing blank line
   * \param setting The setting to create an entry from.
   * \return
   */
  QString createInjectorEntry(const WellSetting &setting) const;
};

}
}
}
}
}

#endif // WELLCONTROLS_H
