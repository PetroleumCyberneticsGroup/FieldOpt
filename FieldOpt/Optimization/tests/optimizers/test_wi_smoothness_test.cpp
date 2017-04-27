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

// Test resources (uncomment according to your needs)
//---------------------------------------------------

// Loads settings (json) for optimizers: (currently) ga, apps, cs
// 1. Includes 'Model/tests/test_resource_model.h', which again includes:
//  'Settings/tests/test_resource_settings.hpp', and
//  'Runner/tests/test_resource_runner.hpp'
// 2. Includes 'test_resource_cases.h'
#include "Optimization/tests/test_resource_optimizer.h"

// Loads '../examples/ECLIPSE/HORZWELL/HORZWELL.EGRID'
// and '../examples/ADGPRS/5spot/ECL_5SPOT.EGRID'
#include "Reservoir/tests/test_resource_grids.h"

// Load test function: (currently) Sphere and Rosenbrock:
//#include "Optimization/tests/test_resource_test_functions.h"
//using namespace TestResources::TestFunctions;

//---------------------------------------------------
#include "Optimization/optimizers/wi_smoothness_test.h"
#include <gtest/gtest.h>


using namespace Optimization::Optimizers;
using namespace Model::Properties;

namespace {

class TestWISmoothnessTest : public ::testing::Test,
                             public TestResources::TestResourceOptimizer,
                             public TestResources::TestResourceGrids {
 protected:
  TestWISmoothnessTest() {

      // Define test variables
      ContinousProperty *prod_heel_x_ = new ContinousProperty(360);
      ContinousProperty *prod_heel_y_ = new ContinousProperty(840);
      ContinousProperty *prod_heel_z_ = new ContinousProperty(1712.0);
      ContinousProperty *prod_toe_x_ = new ContinousProperty(360);
      ContinousProperty *prod_toe_y_ = new ContinousProperty(360);
      ContinousProperty *prod_toe_z_ = new ContinousProperty(1712.0);

      prod_heel_x_->setName("SplinePoint#TESTW#heel#x");
      prod_heel_y_->setName("SplinePoint#TESTW#heel#y");
      prod_heel_z_->setName("SplinePoint#TESTW#heel#z");
      prod_toe_x_->setName("SplinePoint#TESTW#toe#x");
      prod_toe_y_->setName("SplinePoint#TESTW#toe#y");
      prod_toe_z_->setName("SplinePoint#TESTW#toe#z");

      varcont_prod_wi_sm_ = new VariablePropertyContainer();
      varcont_prod_wi_sm_->AddVariable(prod_heel_x_);
      varcont_prod_wi_sm_->AddVariable(prod_heel_y_);
      varcont_prod_wi_sm_->AddVariable(prod_heel_z_);
      varcont_prod_wi_sm_->AddVariable(prod_toe_x_);
      varcont_prod_wi_sm_->AddVariable(prod_toe_y_);
      varcont_prod_wi_sm_->AddVariable(prod_toe_z_);

      // Further define test case
      test_case_spline_->set_objective_function_value(0);

      // Set up optimizer
      sm_tester_ = new WISmoothnessTest(
          settings_wi_smth_test_,
          test_case_spline_,
          varcont_prod_wi_sm_,
          grid_5spot_,
          logger_
      );
  }

  virtual ~TestWISmoothnessTest() {}
  virtual void SetUp() {}

  Optimization::Optimizers::WISmoothnessTest *sm_tester_;
  VariablePropertyContainer *varcont_prod_wi_sm_;

};

TEST_F(TestWISmoothnessTest, TestSetVarIDXCoord) {

    auto x_varid = sm_tester_->GetVarIDXCoord();
    auto xvar = varcont_prod_wi_sm_->GetContinousVariable(x_varid);

    cout << "Name of to-be-perturbed component: "
         << xvar->name().toStdString() << endl;
    cout << "Value of to-be-perturbed component: "
         << xvar->value() << " of type ("
         << xvar->type() << ") " << endl;

    EXPECT_EQ(xvar->value(),360);
}

TEST_F(TestWISmoothnessTest, TestSetPerturbations) {

    cout << "Perturbation points:" << endl;
    auto pertx = sm_tester_->GetPerturbations();
    for (int ii=0; ii < pertx.rows(); ++ii) {
        cout << ii << ": " << pertx[ii] << endl;
    }
}

//TEST_F(TestWISmoothnessTest, TestPertVector) {
//}

}

