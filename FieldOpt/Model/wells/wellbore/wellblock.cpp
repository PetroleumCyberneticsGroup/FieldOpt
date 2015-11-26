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

#include "wellblock.h"
#include "Model/wells/well_exceptions.h"

namespace Model {
namespace Wells {
namespace Wellbore {

WellBlock::WellBlock(int i, int j, int k, int id)
    : id_(id)
{
    i_ = new Properties::DiscreteProperty(i);
    j_ = new Properties::DiscreteProperty(j);
    k_ = new Properties::DiscreteProperty(k);
    completion_ = 0;
}

void WellBlock::AddCompletion(Model::Wells::Wellbore::Completions::Completion *completion)
{
    completion_ = completion;
}

bool WellBlock::HasCompletion()
{
    if (completion_ == 0)
        return false;
    else
        return true;
}

Model::Wells::Wellbore::Completions::Completion *WellBlock::GetCompletion()
{
    if (completion_ == 0)
        throw Model::Wells::CompletionNotDefinedForWellBlockException(i_->value(), j_->value(), k_->value());
    return completion_;
}

bool WellBlock::HasPerforation()
{
    if (completion_ != 0 && completion_->type() == Completions::Completion::CompletionType::Perforation) { // Check if any completion is defined at all and that it is a perforation
        Completions::Perforation *perf = static_cast<Completions::Perforation *>(completion_); // Attempt cast
        if (perf != NULL) // If cast to perforation was successfull, return true.
            return true;
    }
    return false;
}

Completions::Perforation *WellBlock::GetPerforation()
{
    if (completion_ != 0 && completion_->type() == Completions::Completion::CompletionType::Perforation) { // Check if any completion is defined at all and that it is a perforation
        Completions::Perforation *perf = static_cast<Completions::Perforation *>(completion_); // Attempt cast
        if (perf != NULL) // If cast to perforation was successfull, return true.
            return perf;
    }
    throw PerforationNotDefinedForWellBlockException(i_->value(), j_->value(), k_->value());
}

}
}
}
