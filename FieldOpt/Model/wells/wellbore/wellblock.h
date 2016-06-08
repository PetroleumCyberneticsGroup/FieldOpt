#ifndef WELLBLOCK_H
#define WELLBLOCK_H

#include "Model/properties/discrete_property.h"
#include "Model/wells/wellbore/completions/completion.h"
#include "Model/wells/wellbore/completions/perforation.h"

namespace Model {
namespace Wells {
namespace Wellbore {

/*!
 * \brief The WellBlock class represents a single well block. It contains references to any completion
 * defined within it.
 */
class WellBlock
{
    friend class Trajectory;
public:
    WellBlock(int i, int j, int k);

    virtual ~WellBlock();

/*!
     * \brief The DirectionOfPenetration enum indicates the wells direction of penetration through
     * this block. The W value should be used to indicate that the direction could not be calculated.
     */
    enum DirectionOfPenetration : int {X=1, Y=2, Z=3, W=4};

    void AddCompletion(::Model::Wells::Wellbore::Completions::Completion *completion); //!< Add a completion to this well block.
    bool HasCompletion(); //!< Check if this well block has a completion.
    Completions::Completion *GetCompletion(); //!< Get the completion defined for this block.

    bool HasPerforation(); //!< Check if this well block has a perforation-type completion.
    Completions::Perforation *GetPerforation(); //!< Get the perforation defined for this block.

    int i() const { return i_->value(); }
    int j() const { return j_->value(); }
    int k() const { return k_->value(); }
    DirectionOfPenetration directionOfPenetration() const { return direction_of_penetration_; }

    void setI(const int i) { i_->setValue(i); }
    void setJ(const int j) { j_->setValue(j); }
    void setK(const int k) { k_->setValue(k); }
    void setDirectionOfPenetration(const DirectionOfPenetration dop) { direction_of_penetration_ = dop; }
private:
    Model::Properties::DiscreteProperty *i_;
    Model::Properties::DiscreteProperty *j_;
    Model::Properties::DiscreteProperty *k_;
    Completions::Completion *completion_;
    DirectionOfPenetration direction_of_penetration_; //!< The well's direction of penetration through this block.
};

}
}
}
#endif // WELLBLOCK_H
