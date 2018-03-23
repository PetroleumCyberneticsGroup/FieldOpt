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

// ---------------------------------------------------------
#include <iostream>
#include "constraint.h"

// ---------------------------------------------------------
namespace Optimization {
namespace Constraints {

// ---------------------------------------------------------
Constraint::Constraint() {
    logging_enabled_ = false;
    penalty_weight_ = 0.0;
}

// ---------------------------------------------------------
void Constraint::EnableLogging(QString output_directory_path) {
    logging_enabled_ = true;
    constraint_log_path_ = output_directory_path + "/log_constraints.txt";
}

// ---------------------------------------------------------
Eigen::VectorXd
Constraint::GetLowerBounds(QList<QUuid> id_vector) const {
    throw std::runtime_error("Attempted to get bounds from "
                                 "a non-bound cosntraint.");
}

// ---------------------------------------------------------
Eigen::VectorXd
Constraint::GetUpperBounds(QList<QUuid> id_vector) const {
    throw std::runtime_error("Attempted to get bounds from "
                                 "a non-bound cosntraint.");
}

// ---------------------------------------------------------
double Constraint::Penalty(Case *c) {
    return 0.0;
}

// ---------------------------------------------------------
long double Constraint::PenaltyNormalized(Case *c) {
    return normalizer_.normalize(Penalty(c));
}

// ---------------------------------------------------------
void Constraint::InitializeNormalizer(QList<Case *> cases) {
    if (!normalizer_.is_ready()) {
        cout << "WARNING: using default normalization parameter values" << endl;
        normalizer_.set_midpoint(0.0L);
        normalizer_.set_max(1.0L);
        normalizer_.set_steepness(1.0L);
    }
}

}
}

