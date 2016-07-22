#ifndef COMPASSSEARCH_H
#define COMPASSSEARCH_H

#include "Optimization/optimizer.h"

namespace Optimization {
    namespace Optimizers {

/*!
 * \brief The CompassSearch class is an implementation of the Compass Search optimization algorithm
 * described by Torczon, Lewis and Kolda.
 *
 * This algorithm only supports integer and real variables.
 *
 * Reference:
 *
 * Kolda, Tamara G., Robert Michael Lewis, and Virginia Torczon.
 *  "Optimization by direct search: New perspectives on some classical and modern methods."
 *  SIAM review 45.3 (2003): 385-482.
 */
        class CompassSearch : public Optimizer
        {
        public:
            CompassSearch(::Utilities::Settings::Optimizer *settings, Case *base_case,
                          ::Model::Properties::VariablePropertyContainer *variables,
                          Reservoir::Grid::Grid *grid);
            double step_length() const { return step_length_; }

        private:
            double step_length_; //!< The size of the perturbation for each variable.
            double minimum_step_length_; //!< Smallest allowed step length for the optimizer. _This is a termination condition_.

            void step(); //!< Move to a new tentative best case found in the list of recently evaluated cases.
            void contract(); //!< Contract the pattern, i.e. reduce the step length.
            void perturb(); //!< Apply the pattern to generate new cases.

            // Optimizer interface
        public:
            /*!
             * \brief IsFinished Check if the optimization is finished.
             *
             * This algorithm has two termination conditions: max number of objective function evaluations and
             * minimum step length.
             * \return True if the algorithm has finished, otherwise false.
             */
            TerminationCondition IsFinished();

            QString GetStatusStringHeader() const;
            QString GetStatusString() const;

        private:
            void iterate(); //!< Step or contract, perturb, and clear list of recently evaluated cases.
        };

    }}

#endif // COMPASSSEARCH_H
