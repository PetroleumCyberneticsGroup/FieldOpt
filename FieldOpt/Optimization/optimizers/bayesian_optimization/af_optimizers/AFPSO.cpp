/******************************************************************************
   Created by einar on 6/6/17.
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
#include "AFPSO.h"
#include "Utilities/math.hpp"
#include <algorithm>

namespace Optimization {
namespace Optimizers {
namespace BayesianOptimization {
namespace AFOptimizers {

AFPSO::AFPSO() {
    c1_ = 1.193;
    c2_ = 1.193;
    inertia_ = 0.721;
    n_particles_ = 50;
    n_neighbourhoods_ = 10;
    n_iters_ = 20;
    gen_ = get_random_generator();
    n_iterations_ = 50;
    iteration_ = 0;
}
AFPSO::AFPSO(VectorXd lb, VectorXd ub) : AFPSO() {
    lb_ = lb;
    ub_ = ub;
    n_dims_ = lb.size();
}
VectorXd AFPSO::Optimize(libgp::GaussianProcess *gp, AcquisitionFunction &af) {
    pop_.clear();

    // Generate initial population
    for (int i = 0; i < n_particles_; ++i) {
        pop_.push_back(Particle());
    }

    // Evaluate initial population
    for (int j = 0; j < n_particles_; ++j) {
        pop_[j].fit = af.Evaluate(gp, pop_[j].pos);
    }

    // Generate neighbourhoods
    assert(n_particles_ & n_neighbourhoods_ == 0);
    int part_per_nbhd = n_particles_ / n_neighbourhoods_;
    for (int k = 0; k < n_neighbourhoods_; ++k) {
        int p = k*part_per_nbhd;
        pop_[p].nbr_1_idx = p + part_per_nbhd - 1;
        pop_[p].nbr_2_idx = p + 1;
        for (int i = 1; i < part_per_nbhd-1; ++i) {
            pop_[p+i].nbr_1_idx = p + i - 1;
            pop_[p+i].nbr_2_idx = p + i + 1;
        }
        pop_[p+part_per_nbhd-1].nbr_1_idx = p+part_per_nbhd - 2;
        pop_[p+part_per_nbhd-1].nbr_2_idx = p;
    }

    // Main loop (iterations)
    while (iteration_ < n_iterations_) {

        // Inner loop (particles)
        for (int i = 0; i < n_particles_; ++i) {

            // Update best in neighbourhood
            if (pop_[i].fit < pop_[pop_[i].nbr_1_idx].fit) {
                pop_[i].pos_best_nbhd = pop_[pop_[i].nbr_1_idx].pos;
                pop_[i].fit_best_nbhd = pop_[pop_[i].nbr_1_idx].fit;
            }
            else if (pop_[i].fit < pop_[pop_[i].nbr_2_idx].fit) {
                pop_[i].pos_best_nbhd = pop_[pop_[i].nbr_2_idx].pos;
                pop_[i].fit_best_nbhd = pop_[pop_[i].nbr_2_idx].fit;
            }

            // Update velocity, pos and fitness
            pop_[i].update_velocity();
            pop_[i].update_position();
            pop_[i].fit = af.Evaluate(gp, pop_[i].pos);

            // Check if new best fitness for particle
            if (pop_[i].fit > pop_[i].fit_best_self) {
                pop_[i].fit_best_self = pop_[i].fit;
                pop_[i].pos_best_self = pop_[i].pos;
            }
        }
        // Iteration done
        iteration_++;
    }
    sort(pop_.begin(), pop_.end(), [](Particle &a, Particle &b) {
      return a.fit_best_nbhd > b.fit_best_nbhd;
    });
    return pop_[0].pos_best_nbhd;
}

AFPSO::Particle::Particle() {
    pos = VectorXd::Zero(lb_.size());
    for (int i = 0; i < lb_.size(); ++i) {
        pos(i) = random_double(gen_, lb_(i), ub_(i));
    }
    double velocity_sign = random_double(gen_) <= 0.5 ? 1.0 : -1.0;
    vel = velocity_sign * random_double(gen_) * (ub_ - lb_);
    pos_best_self = pos;
    pos_best_nbhd = pos;
    fit = 0.0;
    fit_best_self = fit;
}
void AFPSO::Particle::update_velocity() {
    for (int i = 0; i < lb_.size(); ++i) {
        vel(i) = inertia_ * vel(i)
            + c1_ * random_double(gen_) * (pos_best_self(i) - pos(i))
            + c2_ * random_double(gen_) * (pos_best_nbhd(i) - pos(i));
    }
}
void AFPSO::Particle::update_position() {
    pos = pos + vel;
    check_boundaries();
}
void AFPSO::Particle::check_boundaries() {
    for (int i = 0; i < lb_.size(); ++i) {
        if (pos(i) < lb_(i)) {
            pos(i) = lb_(i);
            vel(i) = 0.0;
        }
        else if (pos(i) > ub_(i)) {
            pos(i) = ub_(i);
            vel(i) = 0.0;
        }
    }
}

}
}
}
}
