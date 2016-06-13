#ifndef MODEL_H
#define MODEL_H

#include <QString>
#include <QList>

#include "Reservoir/reservoir.h"
#include "properties/variable_property_container.h"
#include "wells/well.h"
#include "Utilities/settings/model.h"
#include "Optimization/case.h"
#include "Model/wells/wellbore/wellblock.h"

namespace Model {

    /*!
     * \brief The Model class represents the reservoir model as a whole, including wells and
     * any related variables, and the reservoir grid.
     */
    class Model
    {
    public:
        Model(::Utilities::Settings::Model settings);

        /*!
         * \brief reservoir Get the reservoir (i.e. grid).
         */
        Reservoir::Reservoir *reservoir() const { return reservoir_; }

        /*!
         * \brief variables Get the set of variable properties of all types.
         */
        Properties::VariablePropertyContainer *variables() const { return variable_container_; }

        /*!
         * \brief wells Get a list of all the wells in the model.
         */
        QList<Wells::Well *> *wells() const { return wells_; }

        /*!
         * \brief ApplyCase Applies the variable values from a case to the variables in the model.
         * \param c Case to apply the variable values of.
         */
        void ApplyCase(Optimization::Case *c);

    private:
        Reservoir::Reservoir *reservoir_;
        Properties::VariablePropertyContainer *variable_container_;
        QList<Wells::Well *> *wells_;

        void verify(); //!< Verify the model. Throws an exception if it is not.
        void verifyWells();
        void verifyWellTrajectory(Wells::Well *w);
        void verifyWellBlock(Wells::Wellbore::WellBlock *wb);
    };

}

#endif // MODEL_H
