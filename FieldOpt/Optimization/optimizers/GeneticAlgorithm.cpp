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
    max_generations_ = settings->parameters().max_generations;
    population_size_ = settings->parameters().population_size;
    p_crossover_ = settings->parameters().p_crossover;
    p_mutation_ = settings->parameters().p_mutation;
    decay_rate_ = 2.0;
    mutation_strength_ = 0.5;

    // todo: Generate initial chromosomes and add the cases to the queue
    for (int i = 0; i < population_size_; ++i) {
        auto new_case = new Case(base_case);
        vector<double> rand_reals = random_doubles(gen_, 0, 1, variables->ContinousVariableSize());
        Eigen::Map<Eigen::VectorXd> rand_rea_evec(rand_reals.data(), variables->ContinousVariableSize());
        new_case->SetRealVarValues(rand_rea_evec);
        population_.push_back(Chromosome(new_case));
        case_handler_->AddNewCase(new_case);
    }
}
Optimizer::TerminationCondition GeneticAlgorithm::IsFinished() {
    if (iteration_ >= max_generations_)
        return TerminationCondition::MAX_EVALS_REACHED;
    else
        return TerminationCondition::NOT_FINISHED;
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
vector<GeneticAlgorithm::Chromosome> GeneticAlgorithm::sortPopulation(vector<Chromosome> population) {
    std::sort(population.begin(), population.end(), [&](Chromosome c1, Chromosome c2) {
        return isBetter(c1.case_pointer, c2.case_pointer);
    });
    return population;
}
}
}
