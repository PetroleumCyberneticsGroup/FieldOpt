//
// Created by cutie on 07.09.16.
//

#ifndef FIELDOPT_TRUST_REGION_SEARCH_H
#define FIELDOPT_TRUST_REGION_SEARCH_H

#include "trust_region_classes/poly_model.h"
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
        class TrustRegionSearch : public Optimizer
        {
        public:
            TrustRegionSearch(::Settings::Optimizer *settings, Case *base_case,
                          ::Model::Properties::VariablePropertyContainer *variables,
                          Reservoir::Grid::Grid *grid);
            double step_length() const { return radius_; }

        private:
            double radius_; //!< The size of the perturbation for each variable.
            double minimum_radius_; //!< Smallest allowed step length for the optimizer. _This is a termination condition_.
            PolyModel polymodel_;
            Eigen::VectorXd current_model_;

            void step(); //!< Move to a new tentative best case found in the list of recently evaluated cases.
            void contract(); //!< Contract/reduce the radius of the region.


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
            void UpdateModel(); //!< Creates a polynomial model, no matter what state the PolyModel is currently in

        private:
            void iterate(); //!< Step or contract, perturb, and clear list of recently evaluated cases.
            void completeModel();
            void optimizationStep();
        };

    }}


#endif //FIELDOPT_TRUST_REGION_SEARCH_H
