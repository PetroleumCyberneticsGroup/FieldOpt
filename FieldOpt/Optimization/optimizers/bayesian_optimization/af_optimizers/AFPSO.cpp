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
    c1_ = 2.8;
    c2_ = 1.2;
    inertia_ = 1.2;
    n_particles_ = 200;
    n_neighbourhoods_ = 20;
    n_iterations_ = 500;
    iteration_ = 0;
}
AFPSO::AFPSO(VectorXd lb, VectorXd ub, int rng_seed) : AFPSO() {
    gen_ = get_random_generator(rng_seed*2);
    lb_ = lb;
    ub_ = ub;
    n_dims_ = lb.size();
}
Eigen::VectorXd AFPSO::Optimize(libgp::GaussianProcess *gp, AcquisitionFunction &af, double target) {
    pop_.clear();

    // Generate initial population
    for (int i = 0; i < n_particles_; ++i) {
        pop_.push_back(Particle(lb_, ub_, gen_));
    }

    // Generate neighbourhoods
    assert(n_particles_ % n_neighbourhoods_ == 0);
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

    // Evaluate initial population
    for (int j = 0; j < n_particles_; ++j) {
        pop_[j].fit = af.Evaluate(gp, pop_[j].pos, target);
        pop_[j].fit_best_self = pop_[j].fit;
        pop_[j].fit_best_nbhd = pop_[j].fit;
    }

//    for (int i = 0; i < n_particles_; ++i) {
//        cout << "P" << i << " self: " << pop_[i].fit << " sbst: " << pop_[i].fit_best_self << " nbst: " << pop_[i].fit_best_nbhd << endl;
//    }

    // Main loop (iterations)
    while (iteration_ < n_iterations_) {

        // Inner loop (particles)
        for (int i = 0; i < n_particles_; ++i) {

            // Update best in neighbourhood
            if (pop_[i].fit_best_self < pop_[pop_[i].nbr_1_idx].fit_best_self) {
                if (pop_[i].fit_best_nbhd < pop_[pop_[i].nbr_1_idx].fit_best_self) {
                    pop_[i].pos_best_nbhd = pop_[pop_[i].nbr_1_idx].pos_best_self;
                    pop_[i].fit_best_nbhd = pop_[pop_[i].nbr_1_idx].fit_best_self;
                }
            }
            else if (pop_[i].fit_best_self < pop_[pop_[i].nbr_2_idx].fit_best_self) {
                if (pop_[i].fit_best_nbhd < pop_[pop_[i].nbr_2_idx].fit_best_self) {
                    pop_[i].pos_best_nbhd = pop_[pop_[i].nbr_2_idx].pos_best_self;
                    pop_[i].fit_best_nbhd = pop_[pop_[i].nbr_2_idx].fit_best_self;
                }
            }
            else {
                if (pop_[i].fit_best_nbhd < pop_[i].fit_best_self) {
                    pop_[i].pos_best_nbhd = pop_[i].pos_best_self;
                    pop_[i].fit_best_nbhd = pop_[i].fit_best_self;
                }
            }

            // Update velocity, pos and fitness
            pop_[i].update_velocity(inertia_, c1_, c2_, gen_);
            pop_[i].update_position(lb_, ub_);

            pop_[i].fit = af.Evaluate(gp, pop_[i].pos, target);

            // Check if new best fitness for particle
            if (pop_[i].fit > pop_[i].fit_best_self) {
                pop_[i].fit_best_self = pop_[i].fit;
                pop_[i].pos_best_self = pop_[i].pos;
            }
        }
        iteration_++; // Iteration done
    }
    sort(pop_.begin(), pop_.end(), [](Particle &a, Particle &b) {
      return a.fit_best_nbhd > b.fit_best_nbhd;
    });
//    cout << "f: " << gp->f(pop_[0].pos_best_nbhd.data()) << " v: " << gp->var(pop_[0].pos_best_nbhd.data()) << endl;
    return pop_[0].pos_best_nbhd;
}

AFPSO::Particle::Particle(VectorXd &lb, VectorXd &ub, boost::mt19937 &gen) {
    pos = VectorXd::Zero(lb.size());
    vel = VectorXd::Zero(lb.size());
    for (int i = 0; i < lb.size(); ++i) {
        pos(i) = random_double(gen, lb(i), ub(i));
        double velocity_sign = random_double(gen) <= 0.5 ? 1.0 : -1.0;
        vel(i) = 0.1* velocity_sign * random_double(gen) * (ub(i) - lb(i));
    }
    pos_best_self = pos;
    pos_best_nbhd = pos;
    fit = 0.0;
    fit_best_self = fit;
    fit_best_nbhd = fit;
}
void AFPSO::Particle::update_velocity(double intertia, double c1, double c2, boost::random::mt19937 &gen) {
    for (int i = 0; i < pos.size(); ++i) {
        vel(i) = intertia * vel(i)
            + c1 * random_double(gen) * (pos_best_self(i) - pos(i))
            + c2 * random_double(gen) * (pos_best_nbhd(i) - pos(i));
    }
}
void AFPSO::Particle::update_position(VectorXd &lb, VectorXd &ub) {
    pos = pos + vel;
    check_boundaries(lb, ub);
}
void AFPSO::Particle::check_boundaries(VectorXd &lb, VectorXd &ub) {
    for (int i = 0; i < lb.size(); ++i) {
        if (pos(i) < lb(i)) {
            pos(i) = lb(i);
            vel(i) = 0.0;
        }
        else if (pos(i) > ub(i)) {
            pos(i) = ub(i);
            vel(i) = 0.0;
        }
    }
}

}
}
}
}
