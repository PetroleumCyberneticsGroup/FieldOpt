/******************************************************************************
 *
 * completion.h
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

#ifndef COMPLETION_H
#define COMPLETION_H

#include "Settings/model.h"

namespace Model {
namespace Wells {
namespace Wellbore {
namespace Completions {

/*!
 * \brief The Completion class is the superclass for all completions, e.g. perforations and ICDs.
 *
 * This class specifies members common to all completions, e.g. a unique numerical ID for the completion.
 * Completion objects add themselves to their parent well block.
 */
class Completion
{
public:

    enum CompletionType : int { Perforation=1001 };
    CompletionType type() const { return type_; } //!< Get the type of completion, e.g. Perforation.

protected:
    Completion(::Settings::Model::Well::Completion completion_settings);
    Completion(CompletionType type);

private:
    CompletionType type_;
};

}
}
}
}

#endif // COMPLETION_H
