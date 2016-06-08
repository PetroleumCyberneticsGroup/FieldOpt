#include "perforation.h"

namespace Model {
    namespace Wells {
        namespace Wellbore {
            namespace Completions {

                Perforation::Perforation(Utilities::Settings::Model::Well::Completion completion_settings,
                                         Properties::VariablePropertyContainer *variable_container)
                        : Completion(completion_settings)
                {
                    transmissibility_factor_ = new Properties::ContinousProperty(completion_settings.transmissibility_factor);
                    if (completion_settings.is_variable) {
                        transmissibility_factor_->setName(completion_settings.name);
                        variable_container->AddVariable(transmissibility_factor_);
                    }
                }

                Perforation::Perforation()
                        : Completion(CompletionType::Perforation)
                {
                    transmissibility_factor_ = new Properties::ContinousProperty(0.0);
                }






            }
        }
    }
}
