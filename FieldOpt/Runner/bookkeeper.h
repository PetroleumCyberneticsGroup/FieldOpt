#ifndef BOOKKEEPER_H
#define BOOKKEEPER_H

#include "Utilities/settings/settings.h"
#include "Optimization/case_handler.h"

namespace Runner {

/*!
 * \brief The Bookkeeper class is used to check whether a case has already been evaluated, or
 * is currently being evaluated.
 *
 * If a case has been evaluated, the Bookkeeper can set it's objective function value to
 * the already known value.
 *
 * The Bookkeeper uses the case_handler from the optimizer to keep track of which cases
 * have been evaluated.
 *
 * \todo Handle the case where a case is currently being evaluated; i.e. there exists a case
 * in the "under evaluation" list which is equal to the case being checked, but has a different
 * UUID.
 */
class Bookkeeper
{
public:
    Bookkeeper(Utilities::Settings::Settings *settings, Optimization::CaseHandler *case_handler);

    /*!
     * \brief IsEvaluated Check if a case has already been evaluated. If the set_obj parameter
     * is set to true, the objective value of the case will be set to that of the existing
     * case.
     * \param c The case to check.
     * \param set_obj Automatically set the objective value if it is known.
     * \return True if objective value of these variable values has already been calculated; otherwise false.
     */
    bool IsEvaluated(Optimization::Case *c, bool set_obj=false);

private:
    double tolerance_;
    Optimization::CaseHandler *case_handler_;
};

}

#endif // BOOKKEEPER_H
