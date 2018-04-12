/******************************************************************************
   Created by einar on 4/11/18.
   Copyright (C) 2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
#include "deck_parser.h"
#include <numeric>
#include <boost/lexical_cast.hpp>
#include "Utilities/time.hpp"

namespace Settings {


DeckParser::DeckParser(std::string deck_file) {

    Opm::ParseContext opm_parse_context;
    Opm::Parser opm_parser;

    auto opm_deck_ = opm_parser.parseFile(deck_file, opm_parse_context);
    Opm::EclipseState state( opm_deck_, opm_parse_context );
    Opm::Schedule opm_schedule( opm_deck_,
                                state.getInputGrid(),
                                state.get3DProperties(),
                                state.runspec().phases(),
                                opm_parse_context);

    num_wells_ = opm_schedule.numWells();
    num_groups_ = opm_schedule.numGroups();
    wells_ = opm_schedule.getWells();
    tuning_ = &opm_schedule.getTuning();
    events_ = &opm_schedule.getEvents();
    time_map_ = &opm_schedule.getTimeMap();
    num_timesteps_ = time_map_->numTimesteps();

    initializeTimeVectors();

}

void DeckParser::initializeTimeVectors() {
    time_days_ = std::vector<int>(num_timesteps_);
    time_dates_ = std::vector<std::string>(num_timesteps_);
    time_days_[0] = 0;
    time_dates_[0] = unix_time_to_datestring(time_map_->getStartTime(0));
    for (int i = 1; i < num_timesteps_; ++i) {
        int seconds_from_start = time_map_->getStartTime(i) - time_map_->getStartTime(0);
        int days_from_start = seconds_from_start / 24 / 60 / 60;
        time_days_[i] = days_from_start;
        time_dates_[i] = unix_time_to_datestring(time_map_->getStartTime(i));
    }
}

std::vector<Model::Well> DeckParser::GetWellData() {
    std::vector<Model::Well> well_structs = std::vector<Model::Well>(num_wells_);
    for (int i = 0; i < num_wells_; ++i) {
        well_structs[i] = opmWellToWellStruct(wells_[i]);
    }
    return well_structs;
}

Model::Well DeckParser::opmWellToWellStruct(const Opm::Well *opm_well) {
    Model::Well well_struct;
    well_struct.name = QString::fromStdString(opm_well->name());
    well_struct.type = determineWellType(opm_well);
    well_struct.preferred_phase = determinePreferredPhase(opm_well);
    well_struct.wellbore_radius = determineWellboreRadius(opm_well);
    well_struct.well_blocks = opmToWellBlocks(opm_well->getCompletions());
    well_struct.controls = opmToControlEntries(opm_well);

    return well_struct;
}

Model::WellType DeckParser::determineWellType(const Opm::Well *opm_well) {
    bool is_injector = false;
    bool is_producer = false;

    for (int t = 0; t < num_timesteps_; ++t) {
        if (!is_injector && opm_well->isInjector(t)) {
            if (is_producer) {
                std::cerr << "WARNING: Well " << opm_well->name()
                          << " is detected as an alternating produer/injector well."
                              " This is not currently supported."
                              " Ignoring switch from producer to injector. "
                          << time_dates_[t]
                          << std::endl;
                continue;
            }
            else {
                is_injector = true;
                std::cout << "Well " << opm_well->name() << " set as injector "
                          << time_dates_[t] << std::endl;
            }
        }
        if (!is_producer && opm_well->isProducer(t)) {
            if (is_injector) {
                std::cerr << "WARNING: Well " << opm_well->name()
                          << " is detected as an alternating produer/injector well."
                              " This is not currently supported."
                              " Ignoring switch from injector to producer."
                          << time_dates_[t]
                          << std::endl;
                continue;
            }
            is_producer = true;
            std::cout << "Well " << opm_well->name() << " set as producer "
                      << time_dates_[t] << std::endl;
        }
    }

    if (is_injector && !is_producer) {
        return Model::WellType::Injector;
    }
    else if (!is_injector && is_producer) {
        return Model::WellType::Producer;
    }
    else {
        std::cerr << "WARNING: Deck parser was unable to determine well type." << std::endl;
        return Model::WellType::UNKNOWN_TYPE;
    }
}

Model::PreferredPhase DeckParser::determinePreferredPhase(const Opm::Well *opm_well) {
    Opm::Phase opm_phase = opm_well->getPreferredPhase();
    switch (opm_phase)
    {
        case Opm::Phase::OIL:
            return Model::PreferredPhase::Oil;
            break;
        case Opm::Phase::WATER:
            return Model::PreferredPhase::Water;
            break;
        case Opm::Phase::GAS:
            return Model::PreferredPhase::Gas;
            break;
        default:
            std::cerr << "WARNING: Deck parser was unable to determine the preferred phase"
                "for well " << opm_well->name() << std::endl;
            return Model::PreferredPhase::UNKNOWN_PHASE;
    }
}

double DeckParser::determineWellboreRadius(const Opm::Well *opm_well) {
    auto opm_comps = opm_well->getCompletions();
    double radii_sum = 0;
    for (auto comp : opm_comps) {
        radii_sum += comp.getDiameter() / 2.0;
    }
    double avg_radius = radii_sum / (double)opm_comps.size();
    std::cout << "Wellbore radius set to " << boost::lexical_cast<std::string>(avg_radius)
              << " for well " << opm_well->name() << std::endl;
    return avg_radius;
}

QList<Model::Well::WellBlock> DeckParser::opmToWellBlocks(const Opm::CompletionSet &comp_set) {
    auto well_blocks = QList<Model::Well::WellBlock>();
    for (auto opm_comp : comp_set) {
        Model::Well::Completion comp;
        comp.type = Model::WellCompletionType::Perforation;
        if (opm_comp.getConnectionTransmissibilityFactorAsValueObject().hasValue()) {
            comp.transmissibility_factor = opm_comp.getConnectionTransmissibilityFactor();
        }
        else {
            comp.transmissibility_factor = -1;
        }
        Model::Well::WellBlock wb;
        wb.completion = comp;
        wb.has_completion = true;
        wb.i = opm_comp.getI();
        wb.j = opm_comp.getJ();
        wb.k = opm_comp.getK();
        well_blocks.push_back(wb);
    }
    return well_blocks;
}

QList<Model::Well::ControlEntry> DeckParser::opmToControlEntries(const Opm::Well *opm_well) {
    auto control_entries = QList<Model::Well::ControlEntry>();
    for (int t = 0; t < num_timesteps_; ++t) {
        Model::Well::ControlEntry ce;
        ce.state = Model::WellState::WellOpen;
        ce.control_mode = determineWellControlMode(opm_well, t);
        ce.rate = determineRate(opm_well, t);
        ce.bhp = determineBhp(opm_well, t);
        if (opm_well->isInjector(t)) {
            ce.injection_type = determineInjectorType(opm_well, t);
        }
        ce.time_step = time_days_[t];
    }
    return control_entries;
}

Model::ControlMode DeckParser::determineWellControlMode( const Opm::Well *opm_well, const int timestep ) {
    if (opm_well->isProducer(timestep)) {
        auto opm_wpp = opm_well->getProductionProperties(timestep);
        switch (opm_wpp.controlMode) {
            case Opm::WellProducer::ControlModeEnum::ORAT:
                return Model::ControlMode::RateControl;
                break;
            case Opm::WellProducer::ControlModeEnum::RESV:
                return Model::ControlMode::RateControl;
                break;
            case Opm::WellProducer::ControlModeEnum::LRAT:
                return Model::ControlMode::RateControl;
                break;
            case Opm::WellProducer::ControlModeEnum::BHP:
                return Model::ControlMode::BHPControl;
                break;
            default:
                return Model::ControlMode::UNKNOWN_CONTROL;
                std::cerr << "WARNING: Unable to determine control mode." << std::endl;
        }
    }
    else {
        auto opm_wip = opm_well->getInjectionProperties(timestep);
        switch (opm_wip.controlMode) {
            case Opm::WellInjector::ControlModeEnum::RATE:
                return Model::ControlMode::RateControl;
                break;
            case Opm::WellInjector::ControlModeEnum::RESV:
                return Model::ControlMode::RateControl;
                break;
            case Opm::WellInjector::ControlModeEnum::BHP:
                return Model::ControlMode::BHPControl;
                break;
            default:
                return Model::ControlMode::UNKNOWN_CONTROL;
                std::cerr << "WARNING: Unable to determine control mode."
                    " Unknown control type" << std::endl;
        }
    }
}

double DeckParser::determineRate(const Opm::Well *opm_well, const int timestep) {
    if (opm_well->isInjector(timestep)) {
        auto ips = opm_well->getInjectionProperties(timestep);
        std::vector<double> rates {ips.surfaceInjectionRate, ips.reservoirInjectionRate};
        return *std::max_element(rates.begin(), rates.end());
    }
    else {
        auto pps = opm_well->getProductionProperties(timestep);
        std::vector<double> rates {pps.OilRate, pps.LiquidRate, pps.ResVRate};
        return *std::max_element(rates.begin(), rates.end());
    }
}

double DeckParser::determineBhp(const Opm::Well *opm_well, const int timestep) {
    if (opm_well->isInjector(timestep)) {
        auto ips = opm_well->getInjectionProperties(timestep);
        std::vector<double> bhps {ips.BHPLimit, ips.THPLimit};
        return *std::max_element(bhps.begin(), bhps.end());
    }
    else {
        auto pps = opm_well->getProductionProperties(timestep);
        std::vector<double> bhps {pps.BHPLimit, pps.THPLimit};
        return *std::max_element(bhps.begin(), bhps.end());
    }
}

Model::InjectionType DeckParser::determineInjectorType(const Opm::Well *opm_well, const int timestep) {
    auto ips = opm_well->getInjectionProperties(timestep);
    if (ips.injectorType == Opm::WellInjector::TypeEnum::WATER)
        return Model::InjectionType::WaterInjection;
    else if (ips.injectorType == Opm::WellInjector::TypeEnum::GAS)
        return Model::InjectionType::GasInjection;
    else {
        std::cerr << "WARNING: Unable to detect injection type. Defaulting to water." << std::endl;
        return Model::InjectionType::WaterInjection;
    }
}

}
