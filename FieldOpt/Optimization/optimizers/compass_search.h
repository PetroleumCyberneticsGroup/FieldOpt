#ifndef COMPASSSEARCH_H
#define COMPASSSEARCH_H

#include "Optimization/optimizer.h"
#include "GSS.h"

namespace Optimization {
namespace Optimizers {

/*!
 * \brief The CompassSearch class is an implementation of the Compass Search optimization algorithm
 * described by Torczon, Lewis and Kolda. It is an extension of the abstract GSS class.
 *
 * This algorithm only supports integer and real variables, and not both at the same time.
 *
 * Reference:
 *
 * Kolda, Tamara G., Robert Michael Lewis, and Virginia Torczon.
 *  "Optimization by direct search: New perspectives on some classical and modern methods."
 *  SIAM review 45.3 (2003): 385-482.
 */
class CompassSearch : public GSS
{
 public:
  CompassSearch(::Settings::Optimizer *settings,
                Case *base_case,
                ::Model::Properties::VariablePropertyContainer *variables,
                Reservoir::Grid::Grid *grid,
                Logger *logger
  );

  QString GetStatusStringHeader() const;
  QString GetStatusString() const;

 private:
  void iterate(); //!< Step or contract, perturb, and clear list of recently evaluated cases.
  bool is_successful_iteration(); //!< Check if this iteration was successful (i.e. if the current tent. best case was found in this iteration).

 protected:
  void handleEvaluatedCase(Case *c) override;
};

}}

#endif // COMPASSSEARCH_H
