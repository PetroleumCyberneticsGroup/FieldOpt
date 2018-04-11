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
#ifndef FIELDOPT_DECKPARSER_H
#define FIELDOPT_DECKPARSER_H

#include <string>

#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/MessageContainer.hpp>
#include <opm/parser/eclipse/Parser/ParseContext.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/EclipseState/SummaryConfig/SummaryConfig.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Schedule.hpp>

namespace Settings {

/*!
 * @brief The DeckParser class uses the simulator deck parsing component
 * in the OPM common library to parse simulation decks.
 *
 * The data contained in this class (after parsing a deck) can be used
 * to initialize parts of FieldOpts Settings::Model object; in particular
 * well paths (in the form of well blocks) and well controls.
 *
 * @note For now, only the Schedule part of the deck is used.
 *
 * @note This should work with ECL and Flow decks. With some tweaks,
 * it should also, in time, work with AD-GPRS decks, as their schedule
 * sections are identical.
 */
class DeckParser {
 public:
  DeckParser(std::string deck_file);


};
}

#endif //FIELDOPT_DECKPARSER_H
