#ifndef RESULTS_H
#define RESULTS_H

#include <QString>
#include "results_exceptions.h"
#include <vector>

namespace Simulation {
    namespace Results {

/*!
 * \brief The Results class handles access to the results of an evaluated model. It acts
 * as an interface to the utilities doing the actual reading of simulator summary/result
 * files.
 */
        class Results
        {
        public:
            /*!
             * \brief The Property enum Lists the properties that can be retrieved from the results.
             *
             * If results other than these are to be retrieved, they must be added here and
             * functionality to retrieve them must be implemented in the underlying classes.
             */
            enum Property {
                CumulativeOilProduction,
                CumulativeGasProduction,
                CumulativeWaterProduction,
                CumulativeWellOilProduction,
                CumulativeWellGasProduction,
                CumulativeWellWaterProduction,
                Time
            };

            Property GetPropertyKeyFromString(QString prop) {
                if (prop == "CumulativeOilProduction") return CumulativeOilProduction;
                else if (prop == "CumulativeGasProduction") return CumulativeGasProduction;
                else if (prop == "CumulativeWaterProduction") return CumulativeWaterProduction;
                else if (prop == "CumulativeWellOilProduction") return CumulativeWellOilProduction;
                else if (prop == "CumulativeWellGasProduction") return CumulativeWellGasProduction;
                else if (prop == "CumulativeWellWaterProduction") return CumulativeWellWaterProduction;
                else if (prop == "Time") return Time;
                else throw ResultPropertyKeyDoesNotExistException("");
            }

            /*!
             * \brief ReadResults Read the summary data from file.
             * \param file_path The path to the summary file without suffixes.
             */
            virtual void ReadResults(QString file_path) = 0;

            /*!
             * \brief DumpResults Dump the results that have been read. Should be called when the results are no longer valid.
             *
             * Should call setUnavailable().
             */
            virtual void DumpResults() = 0;

            /*!
             * \brief GetFinalValue Gets the value of the given _field_ or _misc_ property at the final time index.
             * \param prop The property to be retrieved.
             */
            virtual double GetValue(Property prop) = 0;

            /*!
             * \brief GetValueVector Get the vector containing all values for the specified property.
             * \param prop The property to be retrieved.
             */
            virtual std::vector<double> GetValueVector(Property prop) = 0;

            /*!
             * \brief GetFinalValue Gets the value of the given property for the given well at the
             * final time index.
             * \param prop The property to be retireved.
             * \param well The well to get the value from.
             */
            virtual double GetValue(Property prop, QString well) = 0;

            /*!
             * \brief GetValue Get the value of the given _field_ or _misc_ property at the given time index.
             * \param prop The property to be retrieved.
             * \param time_index The time index to get the value of the property at.
             */
            virtual double GetValue(Property prop, int time_index) = 0;

            /*!
             * \brief GetValue Get the value of the given property for the given well at the
             * given time index.
             * \param prop The property to be retrieved.
             * \param well The well to get the value from.
             * \param time_index The time index to get the property at.
             * \return
             */
            virtual double GetValue(Property prop, QString well, int time_index) = 0;

            /*!
             * \brief available Indicates whether results are available.
             *
             * The constructor sets the underlying value to false. setAvailable() should be called
             * when results are available to indicate it.
             */
            bool isAvailable() const { return available_; }

        protected:
            /*!
             * \brief Results Default constructor. A Results object is not useful on its own; One of the
             * subclasses' constructor must be called.
             */
            Results() { available_ = false; }

            /*!
             * \brief setAvailable Sets the availability to true. This shuld be called when results
             * are available.
             */
            void setAvailable() { available_ = true; }

            /*!
             * \brief setUnavailable Sets the availability to false. Should be called when results
             * are unavailable.
             */
            void setUnavailable() { available_ = false; }


        private:
            bool available_;
        };

    }}

#endif // RESULTS_H
