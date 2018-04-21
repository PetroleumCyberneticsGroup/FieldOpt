
/***********************************************************
 Copyright (C) 2015-2017
 Einar J.M. Baumann <einar.baumann@gmail.com>

 Created: 22.09.2015 2015 by einar

 This file is part of the FieldOpt project.

 FieldOpt is free software: you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation, either version
 3 of the License, or (at your option) any later version.

 FieldOpt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty
 of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the
 GNU General Public License along with FieldOpt.
 If not, see <http://www.gnu.org/licenses/>.
***********************************************************/

// ---------------------------------------------------------
#ifndef PROPERTY_H
#define PROPERTY_H
// ---------------------------------------------------------

#include "property_exceptions.h"
#include <QString>
#include <QUuid>

// ---------------------------------------------------------
namespace Model {
namespace Properties {

// =========================================================
/*!
 * \brief Property class is an abstract class implemented
 * by specific property types, i.e. integer, real and binary.
 * It holds and to some extends describes the value of a
 * property in the model.
 */
class Property
{
 public:
  // ---------------------------------------------------------
  enum Type { Discrete, Continous, Binary }; //!< The underlying datatype of the property's value.

  // ---------------------------------------------------------
  Type type() const { return type_; } //!< Get the underlying datatype of the property's value.
  QString name() const { return name_;} //!< Get the name set for the variable. Returns an empty string if name has not been set.
  void setName(QString name) { name_ = name; } //!< Set the name of the variable.

  bool IsLocked() const { return locked_; } //!< Check if the property is locked.
  void Lock() { locked_ = true; } //!< Set the property to locked. The value of a locked property cannot be changed.
  void Unlock() { locked_ = false; } //!< Unlock a property.

  /*!
   * \brief //!< Set the property to variable.
   *
   * This will also populate the info data structure for the property.
   * The property name is used to create the Info datastructure, thus the name of the property
   * must be set before setting it to variable.
   */
  void SetVariable();
  bool isVariable() { return is_variable_; } //!< Check if the property is variable.

  QUuid id() const { return id_; } //!< Unique ID for the property.

  /*!
   * @brief Update the UUID for this object. This is used when synchronizing models between process instances when
   * using MPI.
   * @param new_id The new ID for this object.
   */
  void UpdateId(QUuid new_id);

  /*!
   * \brief The type of property represented. This type decides which
   * field in the Info datastructure is set.
   */
  enum PropertyType : int {BHP=2001, Rate=2002, SplinePoint=2003,
    WellBlock=2004, Transmissibility=2005, PseudoContVert=2006
  };

  /*!
   * \brief For SplinePoint type properties, this is used to indicate
   * whether  the property is for the heel or the toe of the well.
   */
  enum SplineEnd : int {Heel=3001, Toe=3002};

  /*!
   * \brief For SplinePoint and WellBlock type variables, this indicates
   * which coordinate the property is for.
   */
  enum Coordinate : int {x=4001, y=4002, z=4003, i=4004, j=4005, k=4006};

  /*!
   * \brief The PropertyInfo struct contains information about what the
   * property represents.
   * This struct is only populated if the property is set variable.
   */
  struct PropertyInfo {
    bool is_set_ = false; //!< Indicates whether the info has been set or not.
    PropertyType prop_type; //!< The type of property this is (part) of.
    QString parent_well_name; //!< The name of the well this property belongs to.
    int index; //!< The number of the well block this property belongs to, or the time step of a control.
    SplineEnd spline_end;
    Coordinate coord;
  };

  PropertyInfo propertyInfo() const; //!< Get an Info struc containing information about the variable. This is only set if the property is variable.

 protected:
  Property(Type type) { type_ = type; locked_ = false; is_variable_ = false; name_ = "";}

 private:
  QUuid id_ = QUuid::createUuid();
  Type type_;
  bool locked_;
  bool is_variable_;
  QString name_;
  PropertyInfo info_;

  void set_property_info();
  PropertyType get_prop_type_name(const QString prop_name) const;
  QString get_well_name(const QString prop_name) const;
  SplineEnd get_spline_end(const QString prop_name) const;
  int get_prop_index(const QString prop_name) const;
  Coordinate get_prop_coord(const QString prop_name) const;
};

}
}

#endif // PROPERTY_H
