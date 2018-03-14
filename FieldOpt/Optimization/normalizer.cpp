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

#include "normalizer.h"
#include <stdexcept>
#include <cmath>

using namespace std;

namespace Optimization {
Normalizer::Normalizer() {
  midpoint_ = -99;
  steepness_ = -99;
  max_ = -99;
}

bool Normalizer::is_ready() const {
  if (midpoint_ == -99 || steepness_ == -99 || max_ == -99)
    return false;
  else
    return true;
}

long double Normalizer::normalize(const double val) const {
  if (!is_ready())
    throw runtime_error("Attempting to use an untrained normalizer.");
  long double nval = max_ / (1.0 +
      exp( (-1.0L) * steepness_ * (val - midpoint_) )
  );
  return nval;
}

double Normalizer::denormalize(const long double nval) const {
  if (!is_ready())
    throw runtime_error("Attempting to use an untrained normalizer.");
  double val = midpoint_ - log(max_ / nval - 1) / steepness_;
  return val;
}

}
