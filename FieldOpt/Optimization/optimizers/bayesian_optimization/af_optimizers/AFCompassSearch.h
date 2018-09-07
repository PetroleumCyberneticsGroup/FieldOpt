/******************************************************************************
   Created by einar on 6/8/17.
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
#ifndef FIELDOPT_AFCOMPASSSEARCH_H
#define FIELDOPT_AFCOMPASSSEARCH_H

#include "AFOptimizer.h"
#include <Eigen/Core>
#include <boost/random.hpp>

using namespace Eigen;
using namespace std;

namespace Optimization {
namespace Optimizers {
namespace BayesianOptimization {
namespace AFOptimizers {

class AFCompassSearch : public AFOptimizer {
 public:
  AFCompassSearch();
  AFCompassSearch(const VectorXd &lb, const VectorXd &ub, int rng_seed=0);
  Eigen::VectorXd Optimize(libgp::GaussianProcess *gp, AcquisitionFunction &af, double target) override;

 private:
  VectorXd lb_; //!< Lower bounds for the variables.
  VectorXd ub_; //!< Upper bounds for the variables.
  VectorXd step_lengths_;
  VectorXd min_step_lengths_;
  vector<VectorXi> directions_;

  boost::random::mt19937 gen_;
  Eigen::VectorXd generateRandomVector();
};

}
}
}
}
#endif //FIELDOPT_AFCOMPASSSEARCH_H
