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
            "continuous variables (i.e., one spline well)" << endl;
    }

    SetVarIDXCoord();
    SetPerturbations();
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
                "coordinate of the toe" << endl;
            break;
        }
    }
}

Eigen::Matrix<double,Dynamic,1>
WISmoothnessTest::SetPerturbations() {
    pertx_.setLinSpaced(npointsx_-1,0,nblocksx_*block_sz_);
}

void WISmoothnessTest::iterate()
{
    // Stop the optimizer for iterating more than once
    if (iteration_ > 0) { return; }

    // Add one new case for each perturbation point
    auto xvar = variables_->GetContinousVariable(x_varid_);
    for (int ii=0; ii < pertx_.rows(); ++ii) {

        auto new_case = new Case(GetTentativeBestCase());
        variables_->SetContinousVariableValue(
            x_varid_,
            xvar->value() + pertx_[ii]
        );
        case_handler_->AddNewCase(new_case);
    }
}

}
}
