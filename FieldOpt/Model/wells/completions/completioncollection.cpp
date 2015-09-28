/******************************************************************************
 *
 * completioncollection.cpp
 *
 * Created: 24.09.2015 2015 by einar
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

#include "completioncollection.h"
#include "completion_exceptions.h"

namespace Model {
namespace Wells {
namespace Completions {

CompletionCollection::CompletionCollection()
{
    perforations_ = new QList<Perforation *>();
    inflow_control_devices_ = new QList<InflowControlDevice *>();
}

void CompletionCollection::AddPerforation(Perforation *perf)
{
    if (!NewCompletionIsValid(perf)) throw CompletionNotValidException("New perforation is invalid.");
    perforations_->append(perf);
}

void CompletionCollection::AddInflowControlDevice(InflowControlDevice *icd)
{
    if (!NewCompletionIsValid(icd)) throw CompletionNotValidException("New ICD is invalid.");
    inflow_control_devices_->append(icd);
}

bool CompletionCollection::NewCompletionIsValid(Perforation *perf)
{
    return true;
}

bool CompletionCollection::NewCompletionIsValid(InflowControlDevice *icd)
{
    return true;
}

}
}
}
