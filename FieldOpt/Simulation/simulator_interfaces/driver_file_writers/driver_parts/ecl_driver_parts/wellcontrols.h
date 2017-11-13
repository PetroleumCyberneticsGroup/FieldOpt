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
                        WellControls(QList<Model::Wells::Well *> *wells, QList<int> control_times);

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
                            bool has_well_setting = false;
                            QList<WellSetting *> well_settings;
                        };

                        QMap<int, TimeEntry *> time_entries_;
                        void initializeTimeEntries(QList<Model::Wells::Well *> *wells, QList<int> control_times);

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
                        virtual QString createTimeEntry(double time, double prev_time);
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
