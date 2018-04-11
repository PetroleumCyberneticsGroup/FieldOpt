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
#include <ctime>

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

    // Print time steps as dates (YYYY.MM.DD)
//    for (int t = 0; t < num_timesteps_; ++t) {
//        time_t time = time_map_->getStartTime(t);
//        std::tm * ptm = std::localtime(&time);
//        char buffer[12];
//        std::strftime(buffer, 12, "%Y.%m.%d", ptm);
//        std::cout << buffer << std::endl;
//    }
}

}
