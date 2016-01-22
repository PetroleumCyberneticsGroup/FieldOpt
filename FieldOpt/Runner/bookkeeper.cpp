#include "bookkeeper.h"

namespace Runner {

Bookkeeper::Bookkeeper(Utilities::Settings::Settings *settings, Optimization::CaseHandler *case_handler)
{
    tolerance_ = settings->bookkeeper_tolerance();
    case_handler_ = case_handler;
}

bool Bookkeeper::IsEvaluated(Optimization::Case *c, bool set_obj)
{
    foreach (auto evaluated_c, case_handler_->EvaluatedCases()) {
        if (evaluated_c->Equals(c)) { // Case has been evaluated
            if (set_obj) c->set_objective_function_value(evaluated_c->objective_function_value());
            return true;
        }
    }
    return false;
}

}
