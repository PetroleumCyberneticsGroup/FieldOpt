/******************************************************************************
   Created by einar on 8/24/18.
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
#ifndef FIELDOPT_VERBOSITY_H
#define FIELDOPT_VERBOSITY_H

/*!
 * @brief This file contains verbosity level settings for all modules in FieldOpt.
 * FieldOpt has to be recompiled after changing these settings.
 *
 * Level descriptions (they are additive):
 *     0 - Silent (only errors)
 *     1 - Only warnings and rudimentary progression (once pr. iteration).
 *     2 - More progression priting.
 *     3 - Debug-level printing. It is not recommended to use this level on more than one module at a time.
 */

//                    Module
#define VERB_MOD 1 // Model
#define VERB_OPT 2 // Optimization
#define VERB_WIC 1 // WellIndexCalculation
#define VERB_SIM 1 // Simulation
#define VERB_RUN 1 // Runner
#define VERB_RES 1 // Reservoir
#define VERB_SET 1 // Settings

#endif //FIELDOPT_VERBOSITY_H
