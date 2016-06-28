#include "well.h"

namespace Model {
    namespace Wells {

        Well::Well(Utilities::Settings::Model settings,
                   int well_number,
                   Properties::VariablePropertyContainer *variable_container,
                   Reservoir::Reservoir *reservoir)
        {
            Utilities::Settings::Model::Well well_settings = settings.wells().at(well_number);

            name_ = well_settings.name;
            type_ = well_settings.type;
            if (well_settings.group.length() >= 1)
                group_ = well_settings.group;
            else group_ = "";

            preferred_phase_ = well_settings.preferred_phase;

            wellbore_radius_ = new Properties::ContinousProperty(well_settings.wellbore_radius);

            controls_ = new QList<Control *>();
            for (int i = 0; i < well_settings.controls.size(); ++i)
                controls_->append(new Control(well_settings.controls[i], well_settings, variable_container));

            trajectory_ = new Wellbore::Trajectory(well_settings, variable_container, reservoir);

            heel_.i = trajectory_->GetWellBlocks()->first()->i();
            heel_.j = trajectory_->GetWellBlocks()->first()->j();
            heel_.k = trajectory_->GetWellBlocks()->first()->k();
        }

        bool Well::IsProducer()
        {
            return type_ == ::Utilities::Settings::Model::WellType::Producer;
        }

        bool Well::IsInjector()
        {
            return type_ == ::Utilities::Settings::Model::WellType::Injector;
        }

        void Well::Update() {
            trajectory_->UpdateWellBlocks();
            heel_.i = trajectory_->GetWellBlocks()->first()->i();
            heel_.j = trajectory_->GetWellBlocks()->first()->j();
            heel_.k = trajectory_->GetWellBlocks()->first()->k();
        }


    }
}
