/******************************************************************************
 *
 * well.h
 *
 * Created: 22.09.2015 2015 by einar
 *
 * This file is part of the FieldOpt project.
 *
 * Copyright (C) 2015-2015 Einar J.M. Baumann <einar.baumann@ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *****************************************************************************/

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
#include "Model/reservoir/reservoir.h"

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
         ::Model::Properties::VariablePropertyHandler *variable_handler,
         ::Model::Reservoir::Reservoir *reservoir);

    struct Heel { Properties::DiscreteProperty *i; Properties::DiscreteProperty *j; Properties::DiscreteProperty *k; };

    enum PreferedPhase { Oil, Gas, Water, Liquid };

    QString name() const { return name_; }
    ::Utilities::Settings::Model::WellType type() const { return type_; }
    bool IsProducer();
    bool IsInjector();
    ::Utilities::Settings::Model::PreferedPhase prefered_phase() const { return prefered_phase_; }
    double wellbore_radius() const { return wellbore_radius_->value(); }
    Wellbore::Trajectory *trajectory() { return trajectory_; }
    QList<Control *> *controls() { return controls_; }
    int heel_i() const { return heel_.i->value(); }
    int heel_j() const { return heel_.j->value(); }
    int heel_k() const { return heel_.k->value(); }

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
