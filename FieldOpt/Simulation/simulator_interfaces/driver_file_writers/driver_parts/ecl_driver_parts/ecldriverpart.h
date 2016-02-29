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

#ifndef ECLDRIVERPART_H
#define ECLDRIVERPART_H

#include <QStringList>
#include <QString>
#include "../driverpart.h"

namespace Simulation {
namespace SimulatorInterfaces {
namespace DriverFileWriters {
namespace DriverParts {
namespace ECLDriverParts {
/*!
 * \brief The ECLDriverPart class Is the parent class for any part of an ECL100 driver file,
 * e.g. the WELSPECS section. Because the driver files are so similar, this class is also used
 * by ADGPRS driver parts.
 */
class ECLDriverPart : public Simulation::SimulatorInterfaces::DriverFileWriters::DriverParts::DriverPart
{
public:

protected:
    ECLDriverPart() {}

    /*!
     * \brief base_entry_line_ Represents a basic line entry in the eclipse file.
     *
     * For example, in the WELSPECS section, each entry line consists of 17 fields.
     * The base entry line list for the WELSPECS section would thus be 17 instances
     * of the '1*' string, indicating that all 17 values should be defaulted.
     */
    QStringList base_entry_line_;

    /*!
     * \brief initializeBaseEntryLine Initializes the base_entry_line_ member to a set number of elements,
     * all containing the string '1*'.
     * \param n Number of elements to create.
     */
    void initializeBaseEntryLine(int n);

    QList<QStringList> entries_; //!< List containing the actual entries
    QString head_; //!< Anything that should be printed before the content, e.g. the keyword ('WELSPECS' etc.).
    QString foot_; //!< Anything that should be printed after the content, e.g. a terminator ('/')
};

}
}
}
}
}

#endif // ECLDRIVERPART_H
