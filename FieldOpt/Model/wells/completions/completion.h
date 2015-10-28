/******************************************************************************
 *
 * completion.h
 *
 * Created: 24.09.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

#ifndef COMPLETION_H
#define COMPLETION_H

#include "Model/variables/integervariable.h"
#include "Model/variables/variablecontainer.h"
#include "Model/variables/variablehandler.h"
#include "Utilities/settings/model.h"

namespace Model {
namespace Wells {
namespace Completions {

/*!
 * \brief The Completion class is the superclass for all completions, e.g. perforations and ICDs.
 *
 * This class specifies members common to all completions, e.g. the location in the reservoir and
 * a unique numerical ID for the completion.
 */
class Completion
{
public:
    Completion(::Utilities::Settings::Model::Completion completion_settings,
               Variables::VariableContainer *variable_container,
               Variables::VariableHandler *variable_handler);

    int id() const { return id_; } //!< Get the unique ID for this completion.
    int i() const { return i_->value(); }
    int j() const { return j_->value(); }
    int k() const { return k_->value(); }
    void setI(int i) { i_->setValue(i); }
    void setJ(int j) { j_->setValue(j); }
    void setK(int k) { k_->setValue(k); }

private:
    int id_; //!< A unique identifier for this completion. The number is generated when reading the model settings.
    Variables::IntegerVariable *i_;
    Variables::IntegerVariable *j_;
    Variables::IntegerVariable *k_;
};

}
}
}

#endif // COMPLETION_H
