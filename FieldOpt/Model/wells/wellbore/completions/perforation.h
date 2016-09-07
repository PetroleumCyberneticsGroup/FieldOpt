#ifndef PERFORATION_H
#define PERFORATION_H

#include "completion.h"
#include "Model/properties/continous_property.h"
#include "Model/properties/variable_property_container.h"

namespace Model {
    namespace Wells {
        namespace Wellbore {
            namespace Completions {

                /*!
                 * \brief The Perforation class represents a well perforation. It contains the potentially variable transmissibility factor.
                 */
                class Perforation : public Completion
                {
                public:
                    Perforation(::Settings::Model::Well::Completion completion_settings,
                                Properties::VariablePropertyContainer *variable_container);

                    Perforation();

                    double transmissibility_factor() const { return transmissibility_factor_->value(); }
                    void setTransmissibility_factor(double value) { transmissibility_factor_->setValue(value); }

                private:
                    Properties::ContinousProperty *transmissibility_factor_;
                };

            }
        }
    }
}

#endif // PERFORATION_H
