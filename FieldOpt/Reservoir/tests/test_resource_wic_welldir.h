//
// Created by bellout on 7/30/16.
//
#ifndef FIELDOPT_TEST_RESOURCE_WIC_H
#define FIELDOPT_TEST_RESOURCE_WIC_H

#include "Utilities/file_handling/filehandling.h"
#include <iostream>
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
    namespace WIBenchmark {

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
            bool debug_ = false;
        };


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

            // Debug: check lists are OK
            if (debug_){
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
            }

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
}
#endif //FIELDOPT_TEST_RESOURCE_WIC_H
