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
using namespace Eigen;

namespace Optimization {
namespace Optimizers {

WISmoothnessTest::WISmoothnessTest(
    Settings::Optimizer *settings,
    Case *base_case,
    Model::Properties::VariablePropertyContainer *variables,
    Reservoir::Grid::Grid *grid,
    Logger *logger
)
    : Optimizer(settings, base_case, variables, grid, logger) {
    grid_ = grid;
    variables_ = variables;

    // Check routine input has exactly size: 3, type: cont

    cout << variables->DiscreteVariableSize() << endl;
    cout << variables->ContinousVariableSize() << endl;

    if (variables->DiscreteVariableSize() != 0 ||
        variables->ContinousVariableSize() != 3 ) {
        throw runtime_error(
            "WISmoothnessTest: 3 continuous coordinate "
                "variables have not been defined.");
    }
    else {
        cout << "Routine input contains "
            "exactly 3 continuous variables" << endl;
    }

    getXCoordVarID();
    getPerturbations();
}

void WISmoothnessTest::getXCoordVarID() {

    // Get xyz coordinate point
    xyzcoord_ = variables_->GetContinousVariables();

    // Get variable id corresponding to x coord component
    for (auto coord : *xyzcoord_) {
        if (coord->propertyInfo().coord ==
            Model::Properties::Property::Coordinate::x) {
            x_varid = coord->id();
        }
    }
}

Eigen::Matrix<double,Dynamic,1> WISmoothnessTest::getPerturbations() {

    //
    pertx_.setLinSpaced(npointsx_-1,0,nblocksx_*block_sz_);
}

void WISmoothnessTest::iterate()
{
    // Stop the optimizer for iterating more than once
    if (iteration_ > 0) {
        return;
    }



    // Fill up case_handler with x component pertubations
//    auto new_case = new Case(GetTentativeBestCase());
//    new_case->set_real_variable_value(x_varid, perturbation)
//
//    case_handler_->AddNewCase(new_case);

    // "Perturb x coord component" << endl;

//}

//Optimizer::TerminationCondition WISmoothnessTest::IsFinished() {
//    if (iteration_ == 0) return NOT_FINISHED;
//    else if (case_handler_->QueuedCases().size() > 0) return NOT_FINISHED;
//    else return MAX_EVALS_REACHED;
//}

//QString WISmoothnessTest::GetStatusStringHeader() const
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

//QString WISmoothnessTest::GetStatusString() const
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
