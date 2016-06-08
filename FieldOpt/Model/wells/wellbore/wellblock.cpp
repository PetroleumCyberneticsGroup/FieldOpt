#include "wellblock.h"
#include "Model/wells/well_exceptions.h"

namespace Model {
    namespace Wells {
        namespace Wellbore {

            WellBlock::WellBlock(int i, int j, int k)
            {
                i_ = new Properties::DiscreteProperty(i);
                j_ = new Properties::DiscreteProperty(j);
                k_ = new Properties::DiscreteProperty(k);
                completion_ = 0;
            }

            WellBlock::~WellBlock() {
                delete i_;
                delete j_;
                delete k_;
            }

            void WellBlock::AddCompletion(Model::Wells::Wellbore::Completions::Completion *completion)
            {
                completion_ = completion;
            }

            bool WellBlock::HasCompletion()
            {
                return completion_ != 0;
            }

            Model::Wells::Wellbore::Completions::Completion *WellBlock::GetCompletion()
            {
                if (completion_ == 0)
                    throw Model::Wells::CompletionNotDefinedForWellBlockException(i_->value(), j_->value(), k_->value());
                return completion_;
            }

            bool WellBlock::HasPerforation()
            {
                if (completion_ != 0 && completion_->type() == Completions::Completion::CompletionType::Perforation) { // Check if any completion is defined at all and that it is a perforation
                    Completions::Perforation *perf = static_cast<Completions::Perforation *>(completion_); // Attempt cast
                    if (perf != NULL) // If cast to perforation was successfull, return true.
                        return true;
                }
                return false;
            }

            Completions::Perforation *WellBlock::GetPerforation()
            {
                if (completion_ != 0 && completion_->type() == Completions::Completion::CompletionType::Perforation) { // Check if any completion is defined at all and that it is a perforation
                    Completions::Perforation *perf = static_cast<Completions::Perforation *>(completion_); // Attempt cast
                    if (perf != NULL) // If cast to perforation was successfull, return true.
                        return perf;
                }
                throw PerforationNotDefinedForWellBlockException(i_->value(), j_->value(), k_->value());
            }
        }
    }
}
