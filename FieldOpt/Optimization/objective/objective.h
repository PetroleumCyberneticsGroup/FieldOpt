/***********************************************************
 Created: 22.09.2015 2015 by einar

 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it
 and/or modify it under the terms of the GNU General
 Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at
 your option) any later version.

 FieldOpt is distributed in the hope that it will be
 useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public
 License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#ifndef OBJECTIVE_H
#define OBJECTIVE_H

// ---------------------------------------------------------
// QT / STD
#include <QPair>
#include <QList>
#include <vector>

// ---------------------------------------------------------
#include "Settings/model.h"

// ---------------------------------------------------------
namespace Optimization {
namespace Objective {

using std::vector;

// =========================================================
/*!
 * \brief The Objective class defines an interface for objective functions.
 * It cannot be instantiated on its own.
 */
class Objective
{
 public:
  // -------------------------------------------------------
  /*!
   * \brief value Get the evaluated value of the objective function.
   */
  virtual double value() const = 0;

  // -------------------------------------------------------
  void set_verbosity_vector(const vector<int> verb_vector)
  { verb_vector_ = verb_vector; }

  vector<int> verb_vector() const { return verb_vector_; }

 protected:
  // -------------------------------------------------------
  Objective();
  vector<int> verb_vector_ = vector<int>(11,1);

};

}
}

#endif // OBJECTIVE_H
