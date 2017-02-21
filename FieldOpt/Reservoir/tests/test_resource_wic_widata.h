/******************************************************************************
   Copyright (C) 2016-2017 Mathias C. Bellout <mathias.bellout@ntnu.no>

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

#ifndef FIELDOPT_TEST_RESOURCE_WIC_WIDATA_H
#define FIELDOPT_TEST_RESOURCE_WIC_WIDATA_H

#include <QStringList>
#include <QString>
#include <QDir>
#include <QDirIterator>
#include <QtCore/QString>
#include <QList>
#include <QTextStream>
#include <Eigen/Dense>

using namespace Eigen;

namespace TestResources {

class WIData {
 public:
  // Constructor
  WIData(){};

  // Functions
  void ReadData(QString file_name);

  // Variables:
  Matrix<int, Dynamic, 4> IJK;
  Matrix<double,Dynamic,1> WCF;
  QString name;

  // obsolete (keep for ref.):
  // Matrix<int, Dynamic, 4>* IJK_ptr = &IJK;
  // Matrix<double,Dynamic,1>* WCF_ptr = &WCF;

 private:
  bool debug_ = false;
};

void WIData::ReadData(QString file_name) {

    QFile file(file_name);
    file.open(QIODevice::ReadOnly|QIODevice::Text);

    QTextStream in(&file);
    QStringList in_fields;

    Matrix<int, 1, 4> temp_IJK;
    Matrix<int, Dynamic, 4> IJK_stor;
    std::vector<double> wcf;

    while(!in.atEnd()) {

        QString line = in.readLine();

        if (line.contains("OPEN")) {

            // TODO: is there a more robust way to read line such that
            // indices are not wrong if a change in spacing, for example?
            // how to remove the space that the line begins with?
            in_fields = line.split(QRegExp("\\s+"));

            // Read & store well name from current line
            name = in_fields[1];

            // Read IJK values from current line
            temp_IJK << in_fields[2].toInt(), in_fields[3].toInt(),
                in_fields[4].toInt(), in_fields[5].toInt();

            // Store IJK values
            Matrix<int, Dynamic, 4> IJK_curr(IJK_stor.rows() + temp_IJK.rows(), 4);
            IJK_curr << IJK_stor, temp_IJK;
            IJK_stor = IJK_curr;

            // Store well connection factor values
            wcf.push_back(in_fields[8].toFloat());
        };
    }

    file.close();

    IJK = IJK_stor;
    WCF = Map<Matrix<double, Dynamic, 1>>(wcf.data(), wcf.size());

    // Debug: read input is OK
    if (debug_){
    }
}
}

#endif //FIELDOPT_TEST_RESOURCE_WIC_WIDATA_H