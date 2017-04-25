/******************************************************************************
   Copyright (C) 2017 Mathias C. Bellout <mathias.bellout@ntnu.no>

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

#include <iostream>
#include "wi_smoothness_test.h"

using namespace std;

namespace Optimization {
namespace Optimizers {

WISmooothnessTest::WISmooothnessTest(
    Settings::Optimizer *settings,
    Case *base_case,
    Model::Properties::VariablePropertyContainer *variables,
    Reservoir::Grid::Grid *grid,
    Logger *logger
)
    : Optimizer(settings, base_case, variables, grid, logger) {
    grid_ = grid;

//    if (variables->BinaryVariableSize() != 0 ||
//        variables->ContinousVariableSize() != 0 ||
//        variables->DiscreteVariableSize() != 3){
//        cout << "Optimizing IJK" << endl;
//
//    } else if  (variables->ContinousVariableSize() != 0 ||
//        variables->DiscreteVariableSize() != 0 ||
//        variables->ContinousVariableSize() != 3){
//        cout << "Optimizing XYZ" << endl;
//    }


}

void WISmooothnessTest::iterate()
{


}


//QString WISmooothnessTest::GetStatusStringHeader() const
//{
//    return QString("%1,%2,%3,%4,%5,%6,%7")
//        .arg("Iteration")
//        .arg("EvaluatedCases")
//        .arg("QueuedCases")
//        .arg("RecentlyEvaluatedCases")
//        .arg("TentativeBestCaseID")
//        .arg("TentativeBestCaseOFValue")
//        .arg("StepLength");
//}

//QString WISmooothnessTest::GetStatusString() const
//{
//    return QString("%1,%2,%3,%4,%5,%6,%7")
//        .arg(iteration_)
//        .arg(nr_evaluated_cases())
//        .arg(nr_queued_cases())
//        .arg(nr_recently_evaluated_cases())
//        .arg(GetTentativeBestCase()->id().toString())
//        .arg(GetTentativeBestCase()->objective_function_value())
//        .arg(step_lengths_[0]);
}

}
}
