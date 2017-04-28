/******************************************************************************
   Copyright (C) 2015-2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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
   along with FieldOpt. If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "wellblock.h"
#include "Model/wells/well_exceptions.h"

namespace Model {
namespace Wells {
namespace Wellbore {

typedef Model::Wells::Wellbore::Completions::Completion Completion;
typedef Completions::Completion::CompletionType CompletionType;

WellBlock::WellBlock(int i, int j, int k)
{
    i_ = new Properties::DiscreteProperty(i);
    j_ = new Properties::DiscreteProperty(j);
    k_ = new Properties::DiscreteProperty(k);
    completion_ = 0;
}

WellBlock::~WellBlock() {
    delete i_;
    delete j_;
    delete k_;
}

void WellBlock::AddCompletion(Completion *completion)
{
    completion_ = completion;
}

bool WellBlock::HasCompletion()
{
    return completion_ != 0;
}

Completion *WellBlock::GetCompletion()
{
    if (completion_ == 0){
        throw Model::Wells::CompletionNotDefinedForWellBlockException(
            i_->value(), j_->value(), k_->value());
    }
    return completion_;
}

bool WellBlock::HasPerforation()
{
    // Check if any completion is defined at all and that it is a perforation
    if (completion_ != 0 &&
        completion_->type() == CompletionType::Perforation) {

        // Attempt cast
        Completions::Perforation *perf =
            static_cast<Completions::Perforation *>(completion_);

        // If cast to perforation was successfull, return true.
        if (perf != NULL)
            return true;
    }
    return false;
}

Completions::Perforation *WellBlock::GetPerforation()
{
    // Check if any completion is defined at all and that it is a perforation
    if (completion_ != 0 &&
        completion_->type() == CompletionType::Perforation) {

        // Attempt cast
        Completions::Perforation *perf =
            static_cast<Completions::Perforation *>(completion_);

        // If cast to perforation was successfull, return true.
        if (perf != NULL)
            return perf;
    }
    throw PerforationNotDefinedForWellBlockException(
        i_->value(), j_->value(), k_->value());
}
}
}
}
