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
//    bool all_the_same = true;
//    for (int i = 0; i < population_size_ - 1; ++i) {
//        if (population_[i].ofv() != population_[i+1].ofv())
//            all_the_same = false;
//    }
//    if (all_the_same && iteration_ > 0) {
//        cout << "All the same in iteration " << iteration_ << endl;
//        iteration_ = max_generations_+1;
//    }

    if (iteration_ >= max_generations_)
        return TerminationCondition::MAX_EVALS_REACHED;
    else
        return TerminationCondition::NOT_FINISHED;
}
void GeneticAlgorithm::handleEvaluatedCase(Case *c) {
    if (isImprovement(c)) {
        tentative_best_case_ = c;
        cout << "Updated best case in generation " << iteration_ << ": " << c->objective_function_value() << endl;
    }
    auto chrom = Chromosome(c);
    int idx = tournamentSelection(true);
    population_[idx] = chrom;
}
void GeneticAlgorithm::iterate() {
//    sortPopulation();
    cout << "=== ITERATION START" << endl;
    printPopulation();
    int p1_idx = tournamentSelection();
    int p2_idx = tournamentSelection();
    cout << "Selected: " << p1_idx << ", " << p2_idx << endl;
    printChromosome(population_[p1_idx]);
    printChromosome(population_[p2_idx]);
    vector<Chromosome> offspring = laplaceCrossover(p1_idx, p2_idx);
    cout << "After crossover:" << endl;
    printChromosome(offspring[0]);
    printChromosome(offspring[1]);
    auto o1 = powerMutation(offspring[0]);
    auto o2 = powerMutation(offspring[1]);
    o1.createNewCase();
    o2.createNewCase();
    case_handler_->AddNewCase(o1.case_pointer);
    case_handler_->AddNewCase(o2.case_pointer);
    iteration_++;
}
int GeneticAlgorithm::tournamentSelection(bool inverse) {
    auto indices = random_integers(gen_, 0, population_size_-1, 3);
    sort(indices.begin(), indices.end(), [&](int a, int b) {
        return isBetter(population_[a].case_pointer, population_[b].case_pointer);
    });
    if (inverse)
        reverse(indices.begin(), indices.end());
    return indices[0];
}
void GeneticAlgorithm::sortPopulation() {
    std::sort(population_.begin(), population_.end(), [&](Chromosome a, Chromosome b) {
      return isBetter(a.case_pointer, b.case_pointer);
    });
}
vector<GeneticAlgorithm::Chromosome> GeneticAlgorithm::laplaceCrossover(const int p1_idx, const int p2_idx) {
    if (random_double(gen_) > p_crossover_) { // Not doing crossover
        vector<Chromosome> offspring = {population_[p1_idx], population_[p2_idx]};
        return offspring;
    } // else, doing crossover

    double beta_real, beta_int;
    double u = random_double(gen_);

    if (random_double(gen_) <= 0.5) {
        beta_real = location_parameter_ - scale_real_ * log(u);
        beta_int = location_parameter_ - scale_int_ * log(u);
    }
    else {
        beta_real = location_parameter_ + scale_real_ * log(u);
        beta_int = location_parameter_ + scale_int_ * log(u);
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

    vector<Chromosome> offspring = {o1, o2};
    return offspring;
}
GeneticAlgorithm::Chromosome GeneticAlgorithm::powerMutation(Chromosome chr) {
    double p, r, s_int, s_real;
    p = random_double(gen_);
    r = random_double(gen_);
    s_int = pow(random_double(gen_), power_int_);
    s_real = pow(random_double(gen_), power_real_);

    if (p <= p_mutation_) { // Doing mutation
        if (n_ints_ > 0) {
            Eigen::VectorXd t_int = Eigen::VectorXd::Zero(n_ints_);
            for (int j = 0; j < n_ints_; ++j) {
                t_int(j) = (chr.int_vars(j) - int_lower_bounds_(j))
                    / (int_upper_bounds_(j) - chr.int_vars(j));
                if (t_int(j) < r)
                    chr.int_vars(j) = chr.int_vars(j) - s_int * (chr.int_vars(j) - int_lower_bounds_(j));
                else
                    chr.int_vars(j) = chr.int_vars(j) + s_int * (int_upper_bounds_(j) - chr.int_vars(j));
            }
        }
        if (n_reas_ > 0) {
            Eigen::VectorXd t_rea = Eigen::VectorXd::Zero(n_reas_);
            for (int k = 0; k < n_reas_; ++k) {
                t_rea(k) = (chr.rea_vars(k) - rea_lower_bounds_(k))
                    / (rea_upper_bounds_(k) - chr.rea_vars(k));
                if (t_rea(k) < r)
                    chr.rea_vars(k) = chr.rea_vars(k) - s_real * (chr.rea_vars(k) - rea_lower_bounds_(k));
                else
                    chr.rea_vars(k) = chr.rea_vars(k) + s_real * (rea_upper_bounds_(k) - chr.rea_vars(k));
            }
        }
    }
    return chr;

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
        cout << "\t" << i;
        printChromosome(population_[i]);
    }
}
void GeneticAlgorithm::printChromosome(Chromosome &chrom) {
    cout << "\t\t" << chrom.case_pointer->objective_function_value() << endl
         << "\t" << chrom.rea_vars
         << endl;
}
}
}
