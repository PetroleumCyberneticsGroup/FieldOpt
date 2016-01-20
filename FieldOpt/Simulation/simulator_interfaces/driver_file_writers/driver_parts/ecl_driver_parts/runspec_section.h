/******************************************************************************
 *
 *
 *
 * Created: 17.11.2015 2015 by einar
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

#ifndef RUNSPEC_SECTION_H
#define RUNSPEC_SECTION_H

#include "ecldriverpart.h"
#include "Model/wells/well.h"
#include <QStringList>

namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace ECLDriverParts {

/*!
 * \brief The runspec class extracts the runspec section from the initial ECL100 driver
 * file and holds it as a string. It also creates the proper WELLDIMS keyword for the case.
 */
class Runspec : public ECLDriverPart
{
public:
    Runspec(QStringList *driver_file_contents, QList<Model::Wells::Well *> *wells);
    QString GetPartString();

private:
    QString runspec_;
};

}
}
}
}
}


#endif // RUNSPEC_SECTION_H
