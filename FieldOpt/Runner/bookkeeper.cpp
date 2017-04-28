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
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "bookkeeper.h"

namespace Runner {

    Bookkeeper::Bookkeeper(Settings::Settings *settings,
                           Optimization::CaseHandler *case_handler)
    {
        tolerance_ = settings->bookkeeper_tolerance();
        case_handler_ = case_handler;
    }

    bool Bookkeeper::IsEvaluated(Optimization::Case *c, bool set_obj)
    {
        for (auto evaluated_c : case_handler_->EvaluatedCases()) {
            if (evaluated_c->Equals(c,tolerance_)) { // Case has been evaluated
                if (set_obj) {
                    c->set_objective_function_value(
                        evaluated_c->objective_function_value()
                    );
                }
                return true;
            }
        }
        return false;
    }

}
