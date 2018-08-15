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

#include <iostream>
#include "wellcontrols.h"

namespace Simulation {
namespace ECLDriverParts {

WellControls::WellControls(const QList<Model::Wells::Well *> *wells,
                           const QList<int> &control_times)
{
    specific_timestep_ = false;
    initializeBaseEntryLine(12);
    initializeTimeEntries(wells, control_times);
}

WellControls::WellControls(const QList<Model::Wells::Well *> *wells,
                           const QList<int> &control_times,
                           const int &timestep) {
    specific_timestep_ = true;
    initializeBaseEntryLine(12);
    initializeTimeEntries(wells, control_times, timestep);
}

WellControls::WellSetting::WellSetting(const QString &well_name,
                                       const bool &is_injector,
                                       const Model::Wells::Control &control)
{
    this->well_name = well_name;
    this->is_injector = is_injector;
    this->control = Model::Wells::Control(control);
}

QString WellControls::getTimestepPartString() const {
    // When this method is used, the time entries map should only contain two entries: one for the current
    // timestep that includes well data, and one for the next that should only include the time to be used
    // for time progression keywords.
    QString part = "";

    // Add control entries
    if (time_entries_.first().has_well_setting) {
        for (auto well_setting : time_entries_.first().well_settings) {
            if (well_setting.is_injector) {
                QString injector_entry = createInjectorEntry(well_setting);
                part.append(injector_entry);
            }
            else {
                part.append(createProducerEntry(well_setting));
            }
        }
    }

    if (time_entries_.size() == 2) {
        part.append(createTimeEntry(time_entries_.last().time, time_entries_.first().time));
    }
    return part;
}

void WellControls::initializeTimeEntries(const QList<Model::Wells::Well *> *wells,
                                         const QList<int> &control_times,
                                         const int &timestep) {
    time_entries_ = QMap<int, TimeEntry>();
    TimeEntry time_entry;
    time_entry.time = timestep;
    time_entry.well_settings = QList<WellSetting>();
    time_entry.has_well_setting = false;
    for (auto well : *wells) {
        for (auto control : *well->controls()) {
            if (control->time_step() == timestep) {
                time_entry.has_well_setting = true;
                WellSetting well_setting = WellSetting(well->name(), well->IsInjector(), *control);
                time_entry.well_settings.append(well_setting);
            }
        }
    }
    time_entries_.insert(timestep, time_entry);

    // Insert next time entry to be used for time progression keywords
    if (nextTimestep(timestep, control_times) > 0) {
        TimeEntry next_entry;
        next_entry.has_well_setting = false;
        next_entry.time = nextTimestep(timestep, control_times);
        time_entries_.insert(next_entry.time, next_entry);
    }
}

int WellControls::nextTimestep(const int &timestep, const QList<int> &control_times) const {
    int current_index = control_times.indexOf(timestep);
    if (current_index + 1 < control_times.size()) {
        return control_times[current_index + 1];
    }
    else {
        return -1;
    }
}

QString WellControls::GetPartString() const
{
    if (specific_timestep_) {
        return getTimestepPartString();
    }
    else {
        QString part = "";
        double prev_time = 0;
        for (double time : time_entries_.keys()) {
            part.append(createTimeEntry(time, prev_time));
            prev_time = time;
            if (time_entries_[time].has_well_setting) {
                for (WellSetting setting : time_entries_.value(time).well_settings) {
                    if (setting.is_injector)
                        part.append(createInjectorEntry(setting));
                    else
                        part.append(createProducerEntry(setting));
                }
            }
        }
        return part;
    }
}

void WellControls::initializeTimeEntries(const QList<Model::Wells::Well *> *wells, const QList<int> &control_times)
{
    time_entries_ = QMap<int, TimeEntry>();
    for (int i = 0; i < wells->size(); ++i) {
        if (wells->at(i)->trajectory()->GetWellBlocks()->size() > 0) {
            for (int j = 0; j < wells->at(i)->controls()->size(); ++j) {
                int current_time_step = wells->at(i)->controls()->at(j)->time_step();
                if (time_entries_.keys().contains(current_time_step)) {
                    // Adding to existing time step
                    auto extended_well_settings = time_entries_.value(current_time_step).well_settings;
                    extended_well_settings.append(WellSetting(wells->at(i)->name(),
                                                              wells->at(i)->IsInjector(),
                                                              *wells->at(i)->controls()->at(j)));
                    time_entries_[current_time_step].well_settings = extended_well_settings;
                }
                else {
                    // Adding new time step
                    TimeEntry time_entry = TimeEntry();
                    time_entry.time = current_time_step;
                    time_entry.well_settings = QList<WellSetting>({WellSetting(wells->at(i)->name(),
                                                                   wells->at(i)->IsInjector(),
                                                                   *wells->at(i)->controls()->at(j))});
                    time_entry.has_well_setting = true;
                    time_entries_.insert(time_entry.time, time_entry);
                }

            }
        }
    }
    for (int t = 0; t < control_times.size(); ++t) {
        if (!time_entries_.contains(control_times[t])) { // Adding a control time without a well setting
            auto control_time_entry = TimeEntry();
            control_time_entry.time = control_times[t];
            time_entries_.insert(control_time_entry.time, control_time_entry);
        }
    }
}

QString WellControls::createTimeEntry(const double &time, const double &prev_time) const {
    if (time == 0) {
        return QString(""); // A Time entry should not be created for the initial step
    }
    double delta_time = time - prev_time; // The amount of time to advance
    return QString("TSTEP\n 2*%1 /\n\n").arg(delta_time/2.0);
}

QString WellControls::createProducerEntry(const WellControls::WellSetting &setting) const
{
    QStringList producer_entry_line = GetBaseEntryLine(9);
    producer_entry_line[0] = setting.well_name;

    if (setting.control.open()) producer_entry_line[1] = "OPEN";
    else producer_entry_line[1] = "SHUT";

    switch (setting.control.mode()) {
        case ::Settings::Model::ControlMode::BHPControl:
            producer_entry_line[2] = "BHP";
            producer_entry_line[8] = QString::number(setting.control.bhp());
            break;
        case ::Settings::Model::ControlMode::RateControl:
            producer_entry_line[2] = "LRAT";
            producer_entry_line[6] = QString::number(setting.control.rate());
            break;
        default:
            throw std::runtime_error("Producer control mode not recognized.");
    }
    return "WCONPROD\n   " + producer_entry_line.join(" ") + "/\n/\n\n";
}

QString WellControls::createInjectorEntry(const WellControls::WellSetting &setting) const
{
    QStringList injector_entry_line = GetBaseEntryLine(7);
    injector_entry_line[0] = setting.well_name;

    switch (setting.control.injection_fluid()) {
        case ::Settings::Model::InjectionType::WaterInjection:
            injector_entry_line[1] = "WATER";
            break;
        case ::Settings::Model::InjectionType::GasInjection:
            injector_entry_line[1] = "GAS";
            break;
        default:
            throw std::runtime_error("Injector type not recognized.");
    }

    if (setting.control.open()) injector_entry_line[2] = "OPEN";
    else injector_entry_line[2] = "SHUT";

    switch (setting.control.mode()) {
        case ::Settings::Model::ControlMode::BHPControl:
            injector_entry_line[3] = "BHP";
            injector_entry_line[6] = QString::number(setting.control.bhp());
            break;
        case ::Settings::Model::ControlMode::RateControl:
            injector_entry_line[3] = "RATE";
            injector_entry_line[4] = QString::number(setting.control.rate());
            break;
        default:
            throw std::runtime_error("Producer control mode not recognized.");
    }
    return "WCONINJE\n   " + injector_entry_line.join(" ") + "/\n/\n\n";
}



}
}
