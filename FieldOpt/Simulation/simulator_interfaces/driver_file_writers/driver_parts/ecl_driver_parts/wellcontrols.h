/******************************************************************************
 *
 *
 *
 * Created: 13.11.2015 2015 by einar
 *
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
 * \todo It is likely that this object has to be deleted and recreated whenever the model changes to get the new settings.
 *
 * \todo This only supports liquid rate control. Update settings with more keywords to support more rate types.
 */
class WellControls : public ECLDriverPart
{
public:
    WellControls(QList<Model::Wells::Well *> *wells);

    virtual QString GetPartString();

protected:
    /*!
     * \brief The WellSetting struct is a convenience class representing all settings
     * for a well at a particular time.
     */
    struct WellSetting {
        QString well_name; //!< Name of the well this setting belongs to.
        bool is_injector; //!< True if this setting is for an injector, otherwise false.
        Model::Wells::Control *control; //!< The control entry for this setting.
    };

    /*!
     * \brief The TimeEntry struct is a convenience class representing a time entry
     * along with the well settings for that time.
     */
    struct TimeEntry {
        int time;
        QList<WellSetting *> well_settings;
    };

    QMap<int, TimeEntry *> time_entries_;
    void initializeTimeEntries(QList<Model::Wells::Well *> *wells);

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
    virtual QString createTimeEntry(int time);
    QString createProducerEntry(WellSetting *setting);

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
    QString createInjectorEntry(WellSetting *setting);
};

}
}
}
}
}

#endif // WELLCONTROLS_H
