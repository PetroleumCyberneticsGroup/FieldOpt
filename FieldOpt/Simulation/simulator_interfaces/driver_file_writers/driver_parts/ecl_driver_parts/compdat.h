/******************************************************************************
 *
 *
 *
 * Created: 12.11.2015 2015 by einar
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

#ifndef COMPDAT_H
#define COMPDAT_H

#include "ecldriverpart.h"
#include "Model/wells/well.h"

namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace ECLDriverParts {

/*!
 * \brief The Compdat class Generates the string for the WELSPECS section in the ECLIPSE simulator.
 *
 * The information is taken from both the Model::Wells::Well and Model::Wells::Wellbore::WellBlock classes.
 *
 * \todo It is likely that this object has to be deleted and recreated whenever the model changes to get the new settings.
 *
 * \todo Support WELSPECL.
 *
 * \todo Support ICDs.
 *
 * \todo Support spline-defined wells.
 */
class Compdat : public ECLDriverPart
{
public:
    Compdat(QList<Model::Wells::Well *> *wells);
    QString GetPartString();

private:
    QList<QStringList> createWellEntries(Model::Wells::Well *well);
    QStringList createBlockEntry(QString well_name, double wellbore_radius, Model::Wells::Wellbore::WellBlock *well_block);
};

}
}
}
}
}
#endif // COMPDAT_H
