#ifndef WELL_H
#define WELL_H

#include "Utilities/settings/settings.h"
#include "Utilities/settings/model.h"
#include "Model/wells/wellbore/completions/completion.h"
#include "Model/wells/wellbore/completions/perforation.h"
#include "Model/properties/variable_property_container.h"
#include "Model/properties/discrete_property.h"
#include "Model/wells/control.h"
#include "Model/wells/wellbore/trajectory.h"
#include "Reservoir/reservoir.h"

#include <QList>

namespace Model {
    namespace Wells {

/*!
 * \brief The Well class represents any well in the model.
 */
        class Well
        {
        public:
            /*!
             * \brief Well Default constructor.
             * \param settings The settings object to create a well from.
             * \param well_number The index of the sepcific well in the Model.Wells list to create a well from.
             * \param variables The variables object to add all new variable variables to.
             */
            Well(Utilities::Settings::Model settings,
                 int well_number,
                 ::Model::Properties::VariablePropertyContainer *variable_container,
                 ::Reservoir::Reservoir *reservoir);

            struct Heel { int i; int j; int k; };

            enum PreferedPhase { Oil, Gas, Water, Liquid };

            QString name() const { return name_; }
            ::Utilities::Settings::Model::WellType type() const { return type_; }
            bool IsProducer();
            bool IsInjector();
            ::Utilities::Settings::Model::PreferedPhase prefered_phase() const { return prefered_phase_; }
            double wellbore_radius() const { return wellbore_radius_->value(); }
            Wellbore::Trajectory *trajectory() { return trajectory_; }
            QList<Control *> *controls() { return controls_; }
            int heel_i() const { return heel_.i; }
            int heel_j() const { return heel_.j; }
            int heel_k() const { return heel_.k; }
            void Update();

        private:
            QString name_;
            ::Utilities::Settings::Model::WellType type_;
            ::Utilities::Settings::Model::PreferedPhase prefered_phase_;
            Properties::ContinousProperty *wellbore_radius_;
            Wellbore::Trajectory *trajectory_;

            Heel heel_;
            QList<Control *> *controls_;
        };

    }
}

#endif // WELL_H
