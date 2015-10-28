/******************************************************************************
 *
 *
 *
 * Created: 28.10.2015 2015 by einar
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

#ifndef FILEHANDLING_H
#define FILEHANDLING_H

#include <QString>
#include <QFileInfo>

namespace Utilities {
namespace FileHandling {

/*!
 * \brief FileExists Checks whether or not a file exists at the specified path.
 * \param file_path Path to a file that may or may not exist.
 * \return True if a file exists at the specified path, otherwise false.
 */
bool FileExists(QString file_path);

/*!
 * \brief DirectoryExists Checks whether or not a folder exists at the specified path.
 * \param folder_path Path to a folder that may or may not exist.
 * \return True if a folder exists at the specified path, otherwise false.
 */
bool DirectoryExists(QString folder_path);

}
}

#endif // FILEHANDLING_H
