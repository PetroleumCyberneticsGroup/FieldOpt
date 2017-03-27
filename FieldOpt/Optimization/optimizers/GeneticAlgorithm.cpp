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
    population_size_ = std::min(10*variables->ContinousVariableSize(), 100);
    if (population_size_ % 2 != 0) population_size_--;
    p_crossover_ = 0.1;
    lower_bound_ = -10.0;
    upper_bound_ = 10.0;
    decay_rate_ = 4.0;
    mutation_strength_ = 0.25;

    // todo: Generate initial chromosomes and add the cases to the queue
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
    if (iteration_ >= max_generations_) {
        cout << "Generations at termination: " << iteration_ << endl;
        population_ = sortPopulation(population_);
        if (verbosity_level_ > 1) {
            cout << "Final ";
            printPopulation();
        }
        return TerminationCondition::MAX_EVALS_REACHED;
    }
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
    for (int i = 0; i < population_size_; ++i) {
        cout << "\t" << i << "\t";
        printChromosome(population_[i]);
    }
}
void GeneticAlgorithm::printChromosome(Chromosome &chrom) {
    printf("%4.2f\t\t", chrom.ofv());
    for (int i = 0; i < chrom.rea_vars.size(); ++i) {
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
    auto new_case = new Case(tentative_best_case_);
    vector<double> rands = random_doubles(gen_, lower_bound_, upper_bound_,
                                          new_case->GetRealVarVector().size());
    Eigen::VectorXd erands(new_case->GetRealVarVector().size());
    for (int i = 0; i < rands.size(); ++i) {
        erands(i) = rands[i];
    }
    new_case->SetRealVarValues(erands);
    return  new_case;
}
}
}
