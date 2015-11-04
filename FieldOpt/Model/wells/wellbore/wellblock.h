/******************************************************************************
 *
 *
 *
 * Created: 29.10.2015 2015 by einar
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

#ifndef WELLBLOCK_H
#define WELLBLOCK_H

#include "Model/variables/integervariable.h"
#include "Model/wells/wellbore/completions/completion.h"
#include "Model/wells/wellbore/completions/perforation.h"

namespace Model {
namespace Wells {
namespace Wellbore {

/*!
 * \brief The WellBlock class represents a single well block. It contains references to any completion
 * defined within it.
 */
class WellBlock
{
public:
    WellBlock(int i, int j, int k);

    void AddCompletion(::Model::Wells::Wellbore::Completions::Completion *completion); //!< Add a completion to this well block.
    bool HasCompletion(); //!< Check if this well block has a completion.
    Completions::Completion *GetCompletion(); //!< Get the completion defined for this block.

    bool HasPerforation(); //!< Check if this well block has a perforation-type completion.
    Completions::Perforation *GetPerforation(); //!< Get the perforation defined for this block.

    int i() const { return i_->value(); }
    int j() const { return j_->value(); }
    int k() const { return k_->value(); }

    void setI(const int i) { i_->setValue(i); }
    void setJ(const int j) { j_->setValue(j); }
    void setK(const int k) { k_->setValue(k); }


private:
    Model::Variables::IntegerVariable *i_;
    Model::Variables::IntegerVariable *j_;
    Model::Variables::IntegerVariable *k_;
    Completions::Completion *completion_;
};

}
}
}
#endif // WELLBLOCK_H
