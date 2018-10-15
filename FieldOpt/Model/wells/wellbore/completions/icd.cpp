/******************************************************************************
   Copyright (C) 2015-2018 Einar J.M. Baumann <einar.baumann@gmail.com>

   This file is part of the FieldOpt project.

   FieldOpt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   FieldOpt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <Utilities/printer.hpp>
#include <Utilities/verbosity.h>
#include "icd.h"

namespace Model {
namespace Wells {
namespace Wellbore {
namespace Completions {

ICD::ICD(const Settings::Model::Well::Completion &completion_settings,
         Properties::VariablePropertyContainer *variable_container) : SegmentedCompletion(completion_settings,
                                                                                          variable_container) {
    flow_coefficient_ = completion_settings.valve_flow_coeff;
    valve_size_ = new Properties::ContinousProperty(completion_settings.valve_size);
    valve_size_->setName(completion_settings.name);

    min_valve_size_ = completion_settings.min_valve_size;
    max_valve_size_ = completion_settings.max_valve_size;
    assert(min_valve_size_ < max_valve_size_);
    assert(min_valve_size_ >= 0.0);

    time_step_ = completion_settings.time_step;
    segment_idx_ = completion_settings.segment_index;
    device_name_ = completion_settings.device_name;


    if (completion_settings.variable_strength == true || completion_settings.is_variable == true) {
        variable_container->AddVariable(valve_size_);
    }

}
double ICD::setting() const {
    if (valveSize() < min_valve_size_ || valveSize() > max_valve_size_) {
        Printer::ext_warn("Valve size " + Printer::num2str(valveSize()) + "outside bounds, throwing exception.",
            "Model", "ICD");
        throw std::runtime_error("Valve size outside bounds.");
    }

    double setting = (valveSize() - min_valve_size_) / (max_valve_size_ - min_valve_size_);

    if (VERB_MOD >= 1) {
        std::stringstream ss;
        ss << "Computed setting " << setting << " from valve size " << valveSize()
           << ". [min, max] = [ " << min_valve_size_ << " , " << max_valve_size_ << " ]";
        Printer::ext_info(ss.str(), "Model", "ICD");
    }

    return setting;
}

}
}
}
}
