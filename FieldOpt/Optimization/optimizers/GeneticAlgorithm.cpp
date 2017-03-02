/******************************************************************************
   Created by einar on 2/27/17.
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
#include "GeneticAlgorithm.h"
#include "Utilities/math.hpp"
#include "Utilities/stringhelpers.hpp"
#include <math.h>

namespace Optimization {
namespace Optimizers {

GeneticAlgorithm::GeneticAlgorithm(Settings::Optimizer *settings,
                                   Case *base_case,
                                   Model::Properties::VariablePropertyContainer *variables,
                                   Reservoir::Grid::Grid *grid) : Optimizer(settings, base_case, variables, grid) {
    gen_ = get_random_generator();
    n_ints_ = variables->DiscreteVariableSize();
    n_reas_ = variables->ContinousVariableSize();
    location_parameter_ = 0.0;
    max_generations_ = settings->parameters().max_generations;
    population_size_ = settings->parameters().population_size;
    p_crossover_ = settings->parameters().p_crossover;
    p_mutation_ = settings->parameters().p_mutation;
    scale_real_ = settings->parameters().crossover_scale_real;
    scale_int_ = settings->parameters().crossover_scale_int;
    power_real_ = settings->parameters().mutation_power_real;
    power_int_= settings->parameters().mutation_power_int;
    rea_lower_bounds_ = Eigen::VectorXd::Zero(n_reas_);
    rea_lower_bounds_.fill(settings->parameters().lower_bound_real);
    rea_upper_bounds_ = Eigen::VectorXd::Zero(n_reas_);
    rea_upper_bounds_.fill(settings->parameters().upper_bound_real);
    int_lower_bounds_ = Eigen::VectorXd::Zero(n_ints_);
    int_lower_bounds_.fill(settings->parameters().lower_bound_int);
    int_upper_bounds_ = Eigen::VectorXd::Zero(n_ints_);
    int_upper_bounds_.fill(settings->parameters().upper_bound_int);

    assert(population_size_ > 3); // Because of the tournament size, the population must be at least 3.
    assert(population_size_ % 2 == 1); // Ensure that the population size is an odd number.

    // todo: Generate initial chromosomes and add the cases to the queue
    for (int i = 0; i < population_size_; ++i) {
        auto new_case = new Case(base_case);
        if (n_ints_ > 0) {
            vector<int> rand_ints = random_integers(gen_, int_lower_bounds_[0], int_upper_bounds_[0], n_ints_);
            Eigen::Map<Eigen::VectorXi> rand_int_evec(rand_ints.data(), n_ints_);
            new_case->SetIntegerVarValues(rand_int_evec);
        }
        if (n_reas_ > 0) {
            vector<double> rand_reals = random_doubles(gen_, rea_lower_bounds_[0], rea_upper_bounds_[0], n_reas_);
            Eigen::Map<Eigen::VectorXd> rand_rea_evec(rand_reals.data(), n_reas_);
            new_case->SetRealVarValues(rand_rea_evec);
        }
        population_.push_back(Chromosome(new_case));
        case_handler_->AddNewCase(new_case);
    }
}
Optimizer::TerminationCondition GeneticAlgorithm::IsFinished() {
    bool all_the_same = true;
    for (int i = 0; i < population_size_ - 1; ++i) {
        if (population_[i].ofv() != population_[i+1].ofv())
            all_the_same = false;
    }
    if (all_the_same && iteration_ > 0) {
        cout << "All the same in iteration " << iteration_ << endl;
        iteration_ = max_generations_+1;
    }

    if (iteration_ > max_generations_)
        return TerminationCondition::MAX_EVALS_REACHED;
    else
        return TerminationCondition::NOT_FINISHED;
}
void GeneticAlgorithm::handleEvaluatedCase(Case *c) {
    if (isImprovement(c)) {
        tentative_best_case_ = c;
        cout << "Updated best case in generation " << iteration_ << ": " << c->objective_function_value() << endl;
    }
}
void GeneticAlgorithm::iterate() {
    sortPopulation();
    tournamentSelection();
    laplaceCrossover();
    powerMutation();
    for (auto chrom : population_) {
        chrom.createNewCase();
        case_handler_->AddNewCase(chrom.case_pointer);
    }
    iteration_++;
}
void GeneticAlgorithm::tournamentSelection() {
    vector<Chromosome> mating_pool;
    mating_pool.push_back(population_[0]);
    while (mating_pool.size() < population_size_) {
        vector<Chromosome> pool;
        pool.push_back(population_[random_integer(gen_, 0, population_size_-1)]);
        pool.push_back(population_[random_integer(gen_, 0, population_size_-1)]);
        std::sort(pool.begin(), pool.end(), [&](Chromosome a, Chromosome b) {
          return isBetter(a.case_pointer, b.case_pointer);
        });
        mating_pool.push_back(pool[0]);
    }
    population_ = mating_pool;
}
void GeneticAlgorithm::laplaceCrossover() {
    vector<float> us = random_floats(gen_, population_size_);
    vector<float> rs = random_floats(gen_, population_size_);
    vector<float> ps = random_floats(gen_, population_size_);
    vector<Chromosome> offspring;
    offspring.push_back(population_[0]);

    for (int i = 1; i < population_size_ - 1; i += 2) {
        int p1_idx = random_integer(gen_, 0, population_size_-1);
        int p2_idx = random_integer(gen_, 0, population_size_-1);
        if (ps[i] > p_crossover_) { // Not doing crossover
            offspring.push_back(population_[p1_idx]);
            offspring.push_back(population_[p2_idx]);
            continue;
        } // else doing crossover

        float beta_real, beta_int;
        if (rs[i] <= 0.5) {
            beta_real = location_parameter_ - scale_real_ * log(us[i]);
            beta_int = location_parameter_ - scale_int_ * log(us[i]);
        }
        else {
            beta_real = location_parameter_ + scale_real_ * log(us[i]);
            beta_int = location_parameter_ + scale_int_ * log(us[i]);
        }

        auto p1 = population_[p1_idx]; // Parent 1
        auto p2 = population_[p2_idx]; // Parent 2
        auto o1 = Chromosome(p1.case_pointer); // Offspring 1
        auto o2 = Chromosome(p2.case_pointer); // Offspring 2

        if (n_ints_ > 0) {
            o2.int_vars = p2.int_vars + beta_int * (p1.int_vars - p2.int_vars).cwiseAbs();
            o1.int_vars = p1.int_vars + beta_int * (p1.int_vars - p2.int_vars).cwiseAbs();
        }
        if (n_reas_ > 0) {
            o1.rea_vars = p1.rea_vars + beta_real * (p1.rea_vars - p2.rea_vars).cwiseAbs();
            o2.rea_vars = p2.rea_vars + beta_real * (p1.rea_vars - p2.rea_vars).cwiseAbs();
        }
        offspring.push_back(o1);
        offspring.push_back(o2);
    }
    population_ = offspring;
}
void GeneticAlgorithm::powerMutation() {
    vector<float> ps, rs,ss_int, ss_float;
    ps = random_floats(gen_, population_size_);
    rs = random_floats(gen_, population_size_);
    if (n_ints_ > 0)
        ss_int = random_floats(gen_, population_size_);
    if (n_reas_ > 0)
        ss_float = random_floats(gen_, population_size_);

    for (int i = 1; i < population_size_; ++i) {
        if (rs[i] <= p_mutation_) { // Doing mutation
            if (n_ints_ > 0) {
                ss_int[i] = pow(ss_int[i], power_int_);
                Eigen::VectorXd t_int = Eigen::VectorXd::Zero(n_ints_);
                for (int j = 0; j < n_ints_; ++j) {
                    t_int(j) = (population_[i].int_vars(j) - int_lower_bounds_(j))
                        / (int_upper_bounds_(j) - population_[i].int_vars(j));
                    if (t_int(j) < rs[i])
                        population_[i].int_vars(j) = population_[i].int_vars(j)
                            - ss_int[i] * (population_[i].int_vars(j) - int_lower_bounds_(j));
                    else
                        population_[i].int_vars(j) = population_[i].int_vars(j)
                            + ss_int[i] * (int_upper_bounds_(j) - population_[i].int_vars(j));
                }
            }
            if (n_reas_ > 0) {
                ss_float[i] = pow(ss_float[i], power_real_);
                Eigen::VectorXd t_rea = Eigen::VectorXd::Zero(n_reas_);
                for (int k = 0; k < n_reas_; ++k) {
                    t_rea(k) = (population_[i].rea_vars(k) - rea_lower_bounds_(k))
                        / (rea_upper_bounds_(k) - population_[i].rea_vars(k));
                    if (t_rea(k) < rs[i])
                        population_[i].rea_vars(k) = population_[i].rea_vars(k)
                            - ss_float[i] * (population_[i].rea_vars(k) - rea_lower_bounds_(k));
                    else
                        population_[i].rea_vars(k) = population_[i].rea_vars(k)
                            + ss_float[i] * (rea_upper_bounds_(k) - population_[i].rea_vars(k));
                }
            }
        }
    }
}
void GeneticAlgorithm::sortPopulation() {
    std::sort(population_.begin(), population_.end(), [&](Chromosome a, Chromosome b) {
      return isBetter(a.case_pointer, b.case_pointer);
    });
}
GeneticAlgorithm::Chromosome::Chromosome(Case *c) {
    case_pointer = c;
    if (c->integer_variables().size() > 0)
        int_vars = c->GetIntegerVarVector().cast<double>();
    if (c->real_variables().size() > 0)
        rea_vars = c->GetRealVarVector();
}

void GeneticAlgorithm::Chromosome::truncateInts() {
    auto gen = get_random_generator();
    vector<float> rs = random_floats(gen, int_vars.size());
    for (int i = 0; i < int_vars.size(); ++i) {
        float int_part = floor(int_vars[i]);
        if (rs[i] >= 0.5)
            int_part += 1;
        int_vars[i] = int_part;
    }
}
void GeneticAlgorithm::Chromosome::createNewCase() {
    Case *new_case = new Case(case_pointer);
    if (case_pointer->integer_variables().size() > 0) {
        truncateInts();
        new_case->SetIntegerVarValues(int_vars.cast<int>());
    }
    if (case_pointer->real_variables().size() > 0) {
        new_case->SetRealVarValues(rea_vars);
    }
    case_pointer = new_case;
}
void GeneticAlgorithm::printPopulation() {
    cout << "Population:" << endl;
    cout << "\tchrom\tofv\t\tgenes" << endl;
    for (int i = 0; i < population_size_; ++i) {
        cout << "\t" << i
             << "\t\t" << population_[i].case_pointer->objective_function_value() << endl
             << "\t" << population_[i].rea_vars
             << endl;
    }
}
}
}
