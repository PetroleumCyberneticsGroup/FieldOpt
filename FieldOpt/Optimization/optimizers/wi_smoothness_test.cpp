/******************************************************************************
   Copyright (C) 2017 M.Bellout (2017)
   <mathias.bellout@ntnu.no, chakibbb@gmail.com>

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

WISmoothnessTest::WISmoothnessTest(
    Settings::Optimizer *settings,
    Case *base_case,
    Model::Properties::VariablePropertyContainer *variables,
    Reservoir::Grid::Grid *grid,
    Logger *logger
)
    : Optimizer(settings, base_case, variables, grid, logger) {
    variables_ = variables;

    // Check routine input has exactly size: 6, type: cont
    if (variables->DiscreteVariableSize() != 0 ||
        variables->ContinousVariableSize() != 6 ) {
        throw runtime_error(
            "WISmoothnessTest: en entire well, i.e., toe "
                "and heel, needs to be defined in json driver.");
    }
    else {
        cout << "Input to procedure contains exactly 6 "
            "continuous variables (i.e., one spline well)." << endl;
    }

    SetVarIDXCoord();

    long npointsx = 96*5; // num of points incl'zero' point
    long block_sz = 24; // block size (m)
    long nblocksx = 20;  // number of blocks
    SetPerturbations(npointsx, block_sz, nblocksx);
}

void WISmoothnessTest::SetVarIDXCoord() {

    // Get xyz coordinate point
    xyzcoord_ = variables_->GetContinousVariables();
    bool is_xcoord, is_toe;

    // Get variable id corresponding to x coord component
    for (auto coord : *xyzcoord_) {
        is_xcoord = coord->propertyInfo().coord ==
            Model::Properties::Property::Coordinate::x;

        is_toe = coord->propertyInfo().spline_end ==
            Model::Properties::Property::SplineEnd::Toe;

        if (is_xcoord && is_toe) {
            x_varid_ = coord->id();
            cout << "This procedure perturbs only the x "
                "coordinate of the toe." << endl;
            break;
        }
    }
}

Eigen::Matrix<double,Dynamic,1>
WISmoothnessTest::SetPerturbations(long npointsx,
                                   long nblocksx,
                                   long block_sz) {
    pertx_.setLinSpaced(npointsx-1,0,nblocksx*block_sz);
}

void WISmoothnessTest::iterate()
{
    // Stop the optimizer from iterating more than once
    cout << endl << ">>>>> Current iteration: "
         << iteration_ << " <<<<<<" << endl;
    if (iteration_ > 0) {
        return;
    }

    // Add one new case for each perturbation point
    auto xvar = variables_->GetContinousVariable(x_varid_);
    for (int ii=0; ii < pertx_.rows(); ++ii) {

        auto new_case = new Case(GetTentativeBestCase());
        new_case->set_real_variable_value(
            x_varid_,
            xvar->value() + pertx_[ii]
        );
        case_handler_->AddNewCase(new_case);
    }
    iteration_++;
}

Optimizer::TerminationCondition
WISmoothnessTest::IsFinished() {
    if (iteration_ == 0) {
        return NOT_FINISHED;
    }
    else if (case_handler_->QueuedCases().size() > 0) {
        return NOT_FINISHED;
    }
    else return MAX_EVALS_REACHED;
}

//void WISmoothnessTest::handleEvaluatedCase(Case *c) {
//    if (isImprovement(c))
//        updateTentativeBestCase(c);
//}

QString WISmoothnessTest::GetStatusStringHeader() const
{
    return QString("%1,%2,%3,%4,%5,%6,%7")
        .arg("Iteration")
        .arg("Iteration")
        .arg("EvaluatedCases")
        .arg("QueuedCases")
        .arg("RecentlyEvaluatedCases")
        .arg("TentativeBestCaseID")
        .arg("TentativeBestCaseOFValue");
}

QString WISmoothnessTest::GetStatusString() const
{
    return QString("%1,%2,%3,%4,%5,%6,%7")
        .arg(iteration_)
        .arg(iteration_)
        .arg(nr_evaluated_cases())
        .arg(nr_queued_cases())
        .arg(nr_recently_evaluated_cases())
        .arg(GetTentativeBestCase()->id().toString())
        .arg(GetTentativeBestCase()->objective_function_value());
}

}
}
