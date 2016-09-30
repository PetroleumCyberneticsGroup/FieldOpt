//
// Created by cutie on 07.09.16.
//

#ifndef FIELDOPT_TRUST_REGION_SEARCH_H
#define FIELDOPT_TRUST_REGION_SEARCH_H

#include "Utilities/math.hpp"

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

            void step(); //!< Move to a new tentative best case found in the list of recently evaluated cases.
            void contract(); //!< Contract/reduce the radius of the region.
            void expand(); //!< increase the radius of the region.
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

            /*!
             * \brief create QList<variables> from a Case
             *
             * Creates a vector which the PolyModel constructor can take as input
             * \return List of Vectors (i.e. positions from given variables) from a Case
             */
            Eigen::VectorXd PointFromCase(Case* c);

            /*!
             * \brief create a Case from a list of variables and a Case prototype
             *
             * Creates a Case type object from a Case prototype (i.e. a case with the same
             * number of variables but where the variable values have been altered.
             * \return A Case generated from a Eigen::VectorXd point
             */
            Case* CaseFromPoint(Eigen::VectorXd point, Case *prototype);

            /*!
             * \brief Update missing function values in the model
             *
             * Sends all unevaluated cases to runner
             * \return A Case generated from a Eigen::VectorXd point
             */
            void UpdateFunctionValues();

            QString GetStatusStringHeader() const;
            QString GetStatusString() const;

        private:
            void iterate(); //!< Step or contract, perturb, and clear list of recently evaluated cases.
        };

    }}


#endif //FIELDOPT_TRUST_REGION_SEARCH_H
