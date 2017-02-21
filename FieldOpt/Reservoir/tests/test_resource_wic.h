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

#ifndef FIELDOPT_TEST_RESOURCE_WIC_H
#define FIELDOPT_TEST_RESOURCE_WIC_H

#include <QStringList>
#include <QString>
#include <QDir>
#include <QDirIterator>
#include <QtCore/QString>
#include <QList>
#include <Eigen/Dense>
#include <QTextStream>

using namespace Eigen;

namespace TestResources {

class WellDir {
 public:
  // Constructor
  WellDir(){};

  // Functions
  QList<QStringList> GetWellDir();
  QStringList MakeDirList(QString data_dir);
  QStringList AddFilesToList(QStringList temp_dir, QString ext);
  void printWellDirList(QStringList temp, QString temp_str);

 private:
  // Variables: folder path
  QString well_data_dir_ = "../wic-benchmark-wells";
  // QString* well_data_dir_ptr = &well_data_dir_; // obsolete
};

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

  Matrix<int, Dynamic, 4>* IJK_ptr = &IJK;
  Matrix<double,Dynamic,1>* WCF_ptr = &WCF;

 private:

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

//		return WIData;

    //        debug
    //        for( int ii=0; ii < in_fields.length(); ++ii ) {
    //            std::cout << ii << ":" << in_fields[ii].toStdString() << std::endl;
    //        }
}

QList<QStringList> WellDir::GetWellDir() {

    // Check directory exists
    if (!Utilities::FileHandling::DirectoryExists(well_data_dir_))
        throw std::runtime_error("Well dir " + well_data_dir_.toStdString() + " not found.");

    // Make list of well dirs
    QStringList dir_list_ = MakeDirList(well_data_dir_);

    // Make overall well_list_ list + add dir name list to it
    QList<QStringList> well_list_;
    well_list_.append(dir_list_);

    // Make list of rms/pcg well files + add lists to well_list_:
    well_list_.append(AddFilesToList(dir_list_, QString("*RMS.DATA")));
    well_list_.append(AddFilesToList(dir_list_, QString("*PCG.DATA")));

    // Check lists are OK
    std::cout << "well_data_path_: " << well_data_dir_.toStdString() << std::endl;
    std::cout << "size of well_list_: " << well_list_.size() << std::endl;

    QStringList temp = well_list_[0];
    QString temp_str = "well dir list";
    printWellDirList(temp, temp_str);

    temp = well_list_[1];
    temp_str = "rms_list";
    printWellDirList(temp, temp_str);

    temp = well_list_[2];
    temp_str = "pcg_list";
    printWellDirList(temp, temp_str);

    return well_list_;
}

QStringList WellDir::MakeDirList(QString data_dir){
    // Make list of well dirs:
    QDir dir(data_dir);
    dir.setSorting(QDir::Name);
    dir.setNameFilters(QStringList()<<"tw*");
    QStringList dir_list_;
        foreach (QString dir_name_, dir.entryList()){
            QString dir_str_ = dir.absolutePath() + "/" + dir_name_;
            dir_list_.append(dir_str_);
        }
    return dir_list_;
}

QStringList WellDir::AddFilesToList(QStringList temp_dir, QString ext){

    QStringList temp_list_;
        foreach (QString dir_name_, temp_dir){
            QDir temp_dir(dir_name_);
            QStringList file_name_ = temp_dir.entryList(QStringList() << ext,
                                                        QDir::AllEntries |
                                                            QDir::Files |
                                                            QDir::CaseSensitive |
                                                            QDir::NoDotAndDotDot);
            // Check if only one file
            if (file_name_.length()>1)
                throw std::runtime_error("Too many " + ext.toStdString() + " files in well folder!");

            QString file_str_ = dir_name_ + "/" + file_name_[0];
            temp_list_.append(file_str_);
        }
    return temp_list_;
}

void WellDir::printWellDirList(QStringList temp, QString temp_str){
    std::cout << "printing: " << temp_str.toStdString()
              << ", size: " << temp.size() << std::endl;
    for( int ii=0; ii < temp.length(); ++ii ){
        std::cout << temp_str.toStdString() << ": " << temp[ii].toStdString() << std::endl;
    }
}

}
#endif //FIELDOPT_TEST_RESOURCE_WIC_H