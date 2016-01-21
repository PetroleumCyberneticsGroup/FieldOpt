/******************************************************************************
 *
 * weightedsum.cpp
 *
 * Created: 07.10.2015 2015 by einar
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

#include "weightedsum.h"

namespace Optimization {
namespace Objective {

WeightedSum::WeightedSum(Utilities::Settings::Optimizer *settings, Simulation::Results::Results *results)
{
    results_ = results;
    components_ = new QList<WeightedSum::Component *>();
    for (int i = 0; i < settings->objective().weighted_sum.size(); ++i) {
        WeightedSum::Component *comp = new WeightedSum::Component();
        comp->property = results_->GetPropertyKeyFromString(settings->objective().weighted_sum.at(i).property);
        comp->coefficient = settings->objective().weighted_sum.at(i).coefficient;
        comp->time_step = settings->objective().weighted_sum.at(i).time_step;
        if (settings->objective().weighted_sum.at(i).is_well_prop) {
            comp->is_well_property = true;
            comp->well = settings->objective().weighted_sum.at(i).well;
        }
        else comp->is_well_property = false;
        components_->append(comp);
    }
}

double WeightedSum::value() const
{
    double value = 0;
    for (int i = 0; i < components_->size(); ++i) {
        value += components_->at(i)->resolveValue(results_);
    }
    return value;
}

double WeightedSum::Component::resolveValue(Simulation::Results::Results *results)
{
    if (is_well_property) {
        if (time_step < 0) { // Final time step well property
            return coefficient * results->GetValue(property, well);
        }
        else { // Non-final time step well property
            return coefficient * results->GetValue(property, well, time_step);
        }
    }
    else {
        if (time_step < 0) { // Final time step field/misc property
            return coefficient * results->GetValue(property);
        }
        else { // Non-final time step field/misc property
            return coefficient * results->GetValue(property, time_step);
        }
    }
}

}
}
