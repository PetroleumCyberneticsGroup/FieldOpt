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
#include <vector>

#include <QList>

#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/MessageContainer.hpp>
#include <opm/parser/eclipse/Parser/ParseContext.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/EclipseState/SummaryConfig/SummaryConfig.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Schedule.hpp>
#include "Settings/model.h"

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
 *
 * @todo Add support for multisegment wells.
 */
class DeckParser {
 public:
  DeckParser(std::string deck_file);

  std::vector<Model::Well> GetWellData();

 private:
  size_t num_wells_;
  size_t num_groups_;
  size_t num_timesteps_;

  std::vector< const Opm::Well * > wells_;
  const Opm::Tuning *tuning_;
  const Opm::Events *events_;
  const Opm::TimeMap *time_map_;
  std::vector<int> time_days_;
  std::vector<std::string> time_dates_;

  void initializeTimeVectors();

  /*!
   * @brief Convert a parsed OPM well strucure to a FieldOpt
   * Settings well struct.
   */
  Model::Well opmWellToWellStruct(const Opm::Well *opm_well);

  /*!
   * @brief Determine whether a well is a producer or an injector.
   *
   * Note that this does not support wells that alternate between
   * producting and injecting. It will be set as whatever the well
   * is first set to be in the deck; if it switches later a warning
   * will be printed, but it will otherwise be ignored.
   */
  Model::WellType determineWellType(const Opm::Well *opm_well);

  /*!
   * @brief Determine the preferred phase for a well (water/oil/gas/liquid)
   */
  Model::PreferredPhase determinePreferredPhase(const Opm::Well *opm_well);

  /*!
   * @brief Determine the wellbore radius for the well.
   *
   * Note that this will just take the average of the wellbore radius
   * for all the completions found for the well. This is because
   * FieldOpt does not currently support wells with variable
   * wellbore radius.
   */
  double determineWellboreRadius(const Opm::Well *opm_well);

  /*!
   * @brief Convert an OPM completion set to a list of FieldOpt settings
   * WellBlocks.
   */
  QList<Model::Well::WellBlock> opmToWellBlocks(const Opm::CompletionSet &comp_set);

  /*!
   * @brief Convert OPMs representation of well controls to a list of
   * FieldOpt Settings ControlEntries.
   */
  QList<Model::Well::ControlEntry> opmToControlEntries(const Opm::Well *opm_well);

  /*!
   * @brief Determine the control mode for a well at a certain timestep
   * using the WellProductionProperties for that well at that time from
   * the OPM parser.
   */
  Model::ControlMode determineWellControlMode(const Opm::Well *opm_well, const int timestep);

  /*!
   * @brief Determine the target/limit rate for a well at a timestep.
   * @note I'm really not sure if this is correctly implemented.
   */
  double determineRate(const Opm::Well *opm_well, const int timestep);

  /*!
   * @brief Determine the target/limit bhp for a well at a timestep.
   * @note I'm really not sure if this is correctly implemented.
   */
  double determineBhp(const Opm::Well *opm_well, const int timestep);

  /*!
   * @brief Determine the injector type (gas/water).
   */
  Model::InjectionType determineInjectorType(const Opm::Well *opm_well, const int timestep);
};
}

#endif //FIELDOPT_DECKPARSER_H
