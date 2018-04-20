/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 Created: 23.09.2015 2015 by einar

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation, either version
 3 of the License, or (at your option) any later version.

 FieldOpt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty
 of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the GNU
 General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#ifndef CONTINOUS_PROPERTY_H
#define CONTINOUS_PROPERTY_H

// ---------------------------------------------------------
#include "property.h"

// ---------------------------------------------------------
namespace Model {
namespace Properties {

// =========================================================
/*!
 * \brief The ContinousProperty class describes a continous property in the model. The
 * value of the property is held as a floating-point number. Continous properties
 * are typically used for production properties, such as rates and pressures.
 */
class ContinousProperty : public Property
{
 public:
  // ---------------------------------------------------------
  ContinousProperty(double value);

  // ---------------------------------------------------------
  double value() const { return value_; }
  void setValue(double value);

  // ---------------------------------------------------------
  void Add(double d); //!< Add d to the value of this property.

  // ---------------------------------------------------------
  /*!
   * \brief Equals checks whether the value of of this property is equal to
   * the value of another property, optionally within some tolerance.
   * \param other The property to compare this to.
   * \param epsilon Optional tolerance. Default: 0.0
   * \return True if abs(this->value() - other->value()) <= epsilon; otherwise false.
   */
  bool Equals(const ContinousProperty *other, double epsilon=0.0) const;

  QString ToString() const;

 private:
  // ---------------------------------------------------------
  double value_;
};

}
}

#endif // CONTINOUS_PROPERTY_H
