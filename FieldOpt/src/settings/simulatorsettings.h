#ifndef SIMULATORSETTINGS_H
#define SIMULATORSETTINGS_H

#include <QString>

enum SelectedSimulator {SIM_NOT_SET, MRST, GPRS, VLP};

/*!
 * \brief Class containing general settings for the reservoir simulator.
 */
class SimulatorSettings
{
private:
    SelectedSimulator simulator;  //!< The simulator specified in the driver file.

public:
    SimulatorSettings();

    SelectedSimulator getSimulator() const;
    void setSimulator(const SelectedSimulator &value);

    QString toString() const;
};

#endif // SIMULATORSETTINGS_H
