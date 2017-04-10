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
                                   Reservoir::Grid::Grid *grid,
                                   Logger *logger
)
    : Optimizer(settings, base_case, variables, grid, logger) {
    n_vars_ = variables->ContinousVariableSize();
    gen_ = get_random_generator();
    max_generations_ = settings->parameters().max_generations;
    if (settings->parameters().population_size < 0)
        population_size_ = std::min(10*n_vars_, 100);
    else population_size_ = settings->parameters().population_size;
    if (population_size_ % 2 != 0) population_size_--; // Make sure its an even number

    p_crossover_ = settings->parameters().p_crossover;
    decay_rate_ = settings->parameters().decay_rate;
    mutation_strength_ = settings->parameters().mutation_strength;

    if (constraint_handler_->HasBoundaryConstraints()) {
        lower_bound_ = constraint_handler_->GetLowerBounds(base_case->GetRealVarIdVector());
        upper_bound_ = constraint_handler_->GetUpperBounds(base_case->GetRealVarIdVector());
        if (verbosity_level_ > 1) {
            cout << "Using bounds from constraints: " << endl;
            cout << vec_to_str(vector<double>(lower_bound_.data(), lower_bound_.data() + lower_bound_.size()));
            cout << endl;
            cout << vec_to_str(vector<double>(upper_bound_.data(), upper_bound_.data() + upper_bound_.size()));
            cout << endl;
        }
    }
    else {
        lower_bound_.resize(n_vars_);
        upper_bound_.resize(n_vars_);
        lower_bound_.fill(settings->parameters().lower_bound);
        upper_bound_.fill(settings->parameters().upper_bound);
    }

    for (int i = 0; i < population_size_; ++i) {
        auto new_case = generateRandomCase();
        population_.push_back(Chromosome(new_case));
        case_handler_->AddNewCase(new_case);
    }
    if (verbosity_level_ > 1) {
        cout << "Initial ";
        printPopulation();
    }
}
Optimizer::TerminationCondition GeneticAlgorithm::IsFinished() {
    TerminationCondition tc = NOT_FINISHED;
    if (iteration_ >= max_generations_)
        tc = MAX_ITERATIONS_REACHED;
    else if (case_handler_->NumberSimulated() > max_evaluations_)
        tc = MAX_EVALS_REACHED;

    if (tc != NOT_FINISHED) {
        cout << "Generations at termination: " << iteration_ << endl;
        population_ = sortPopulation(population_);
        logger_->AddEntry(new Summary(this, tc));
    }
    return tc;
}
GeneticAlgorithm::Chromosome::Chromosome(Case *c) {
    case_pointer = c;
    rea_vars = c->GetRealVarVector();
}
void GeneticAlgorithm::Chromosome::createNewCase() {
    Case *new_case = new Case(case_pointer);
    new_case->SetRealVarValues(rea_vars);
    case_pointer = new_case;
}
void GeneticAlgorithm::printPopulation(vector<Chromosome> population) const {
    if (population.size() == 0)
        population = population_;
    cout << "Population:" << endl;
    for (int i = 0; i < population.size(); ++i) {
        cout << "\t" << i << "\t";
        printChromosome(population[i]);
    }
}
void GeneticAlgorithm::printChromosome(Chromosome &chrom) const {
    printf("%4.2f\t\t", chrom.ofv());
    for (int i = 0; i < n_vars_; ++i) {
        printf("%2.4f\t", chrom.rea_vars(i));
    }
    cout << endl;
}
vector<GeneticAlgorithm::Chromosome> GeneticAlgorithm::sortPopulation(vector<Chromosome> population) {
    std::sort(population.begin(), population.end(), [&](Chromosome c1, Chromosome c2) {
      return isBetter(c1.case_pointer, c2.case_pointer);
    });
    return population;
}
Case *GeneticAlgorithm::generateRandomCase() {
    auto new_case = new Case(GetTentativeBestCase());

    Eigen::VectorXd erands(n_vars_);
    for (int i = 0; i < n_vars_; ++i) {
        erands(i) = random_doubles(gen_, lower_bound_(i), upper_bound_(i), 1)[0];
    }
    new_case->SetRealVarValues(erands);
    return  new_case;
}
}
}
