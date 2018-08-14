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
#ifndef WELSPECS_H
#define WELSPECS_H

#include "ecldriverpart.h"
#include "Model/wells/well.h"
#include <QList>

namespace Simulation {
namespace ECLDriverParts {

/*!
 * \brief The Welspecs class Generates the string for the WELSPECS section in the ECLIPSE simulator.
 *
 * The information is taken from the Model::Wells::Well class.
 */
class Welspecs : public ECLDriverPart
{
 public:
  /*!
   * Generate a WELSPECS entry for all wells at that are activated at any timestep.
   *
   * Note that this will result in a non-viable deck if not all variable wells are activated
   * at the first time step.
   * @param wells
   */
  Welspecs(QList<Model::Wells::Well *> *wells);

  Welspecs(){}

  /*!
   * Generate the WELSPECS entry data for all wells that are first activated
   * at a certain timestep.
   * @param wells List of all wells.
   * @param timestep Timestep to generate data for.
   */
  Welspecs(QList<Model::Wells::Well *> *wells, int timestep);

  QString GetPartString() const;

 private:
  QStringList createWellEntry(::Model::Wells::Well *well);
};

}
}

#endif // WELSPECS_H
