/******************************************************************************
   Created by einar on 5/23/17.
   Copyright (C) 2017 Einar J.M. Baumann <einar.baumann@gmail.com>

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

#include <gtest/gtest.h>
#include "Optimization/normalizer.h"
#include <cmath>

namespace {
class NormalizerTest : public ::testing::Test {
 public:
  NormalizerTest() : Test() {
      normalizer_.set_max(1.0);
      normalizer_.set_midpoint(0.0);
      normalizer_.set_steepness(1.0);
  }
  virtual ~NormalizerTest() { }

 protected:
  virtual void TearDown() { }
  virtual void SetUp() { }

  Optimization::Normalizer normalizer_;
  std::vector<double> test_values {-100, -1.0, -.5, -.1, 0.0, .001, .25, 1.0, 500};
  std::vector<double> test_values_normed {3.7200759760208356e-44, /* Calculated using a python implementation */
                                          0.2689414213699951,
                                          0.3775406687981454,
                                          0.47502081252106,
                                          0.5,
                                          0.5002499999791666,
                                          0.5621765008857981,
                                          0.7310585786300049,
                                          1.0};
};

TEST_F(NormalizerTest, Initialization) {
    Optimization::Normalizer normalizer;
    EXPECT_FALSE(normalizer.is_ready());
    normalizer.set_max(1.0);
    normalizer.set_midpoint(0.0);
    normalizer.set_steepness(1.0);
    EXPECT_TRUE(normalizer.is_ready());
}

TEST_F(NormalizerTest, ExtremeValues) {
    EXPECT_NO_THROW(normalizer_.normalize(1e200));
    EXPECT_NO_THROW(normalizer_.normalize(-1e200));
    EXPECT_NO_THROW(normalizer_.denormalize(1.0));
    EXPECT_NO_THROW(normalizer_.denormalize(0.0));
    EXPECT_TRUE(std::isinf(normalizer_.denormalize(1.0)));
    EXPECT_TRUE(std::isinf(normalizer_.denormalize(0.0)));
}

TEST_F(NormalizerTest, Normalization) {
    for (int i = 0; i < test_values.size(); ++i) {
        EXPECT_NEAR(normalizer_.normalize(test_values[i]), test_values_normed[i], 1e-10);
    }
}

TEST_F(NormalizerTest, Denormalization) {
    for (int i = 0; i < test_values.size() - 1; ++i) {
        EXPECT_NEAR(normalizer_.denormalize(test_values_normed[i]), test_values[i], 1e-10);
    }
}
}
