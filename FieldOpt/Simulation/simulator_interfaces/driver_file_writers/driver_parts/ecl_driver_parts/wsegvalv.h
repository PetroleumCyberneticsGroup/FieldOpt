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

#ifndef FIELDOPT_WSEGVALV_H
#define FIELDOPT_WSEGVALV_H

#include "ecldriverpart.h"
#include "Model/wells/well.h"

namespace Simulation {
namespace ECLDriverParts {

using namespace Model::Wells;

/*!
 * Generates the WSEGVALV keyword. Must be used once per well.
 *
 * 1. Well name.
 * 2. Segment number.
 * 3. Dimensionless flow coefficient (\$ C_v \$).
 * 4. Cross-section area for flow in the constriction (\$ A_c \$).
 */
class Wsegvalv : public ECLDriverPart {
 public:
  Wsegvalv(Well *well);
  Wsegvalv(QList<Model::Wells::Well *> *wells, int ts);
  Wsegvalv() {}
  QString GetPartString() const override;

 private:
  QString generateEntry(Segment seg, QString wname);

  QStringList entries_;
};

}
}
#endif //FIELDOPT_WSEGVALV_H
