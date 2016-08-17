//
// Created by bellout on 8/4/16.
//
#ifndef FIELDOPT_TEST_RESOURCE_WIC_WIDATA_H
#define FIELDOPT_TEST_RESOURCE_WIC_WIDATA_H

#include <QStringList>
#include <QString>
#include <QDir>
#include <QDirIterator>
#include <QtCore/QString>
#include <QList>
#include <Eigen/Dense>

#include <QFile>
#include <QTextStream>
#include <fstream>

using namespace Eigen;

namespace TestResources {
    namespace WIBenchmark {

        class WIData {
        public:
            // Constructor
            WIData(){};

            // Functions
            void ReadData(QString file_name, QString dir_name, QString dir_list);
            void PrintIJKData(QString file_name);
            void PrintWCFData(QString file_name);

            // Variables:
            Matrix<int, Dynamic, 4> IJK;
            Matrix<double,Dynamic,1> WCF;
            Matrix<double,1,6> XYZ;
            QStringList name;
            QString dir_name;
            QString data_tag;

            bool debug_ = true;

        private:
        };

        void WIData::PrintIJKData(QString file_name) {

            std::ofstream file;
            file.open(file_name.toStdString());

            if (!file.is_open()){
                std::cerr << "Cannot open file '"
                          << file_name.toStdString()
                          << "' for writing." << std::endl;
            }

            file << std::fixed;
            file << IJK;
            file.close();
        }

        void WIData::PrintWCFData(QString file_name) {

            std::ofstream file;
            file.open(file_name.toStdString());

            if (!file.is_open()){
                std::cerr << "Cannot open file '"
                          << file_name.toStdString()
                          << "' for writing." << std::endl;
            }

            file << std::fixed;
            file << WCF;
            file.close();
        }

        void WIData::ReadData(QString file_name,
                              QString dir_name,
                              QString dir_list){

            // READ IJK AND WCF DATA
            QFile file(file_name);
            file.open(QIODevice::ReadOnly|QIODevice::Text);

            QFileInfo file_info(file_name);
            dir_name = file_info.dir().dirName();

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
                    name.append(in_fields[1]);

                    // Read IJK values from current line
                    temp_IJK << in_fields[2].toInt(), in_fields[3].toInt(),
                            in_fields[4].toInt(), in_fields[5].toInt();

                    // Store IJK values
                    Matrix<int, Dynamic, 4> IJK_curr(IJK_stor.rows() + temp_IJK.rows(), 4);
                    IJK_curr << IJK_stor, temp_IJK;
                    IJK_stor = IJK_curr;

                    // Store well conne = 0ction factor values
                    wcf.push_back(in_fields[8].toDouble());
                };
            }

            file.close();

            IJK = IJK_stor;
            WCF = Map<Matrix<double, Dynamic, 1>>(wcf.data(), wcf.size());

            // Debug: check read process for IJK and WCF data is OK
            if (debug_){
                std::cout << "\033[1;31m<DEBUG:START->\033[0m" << std::endl;
                std::cout << "\033[1;31m<DEBUG:END--->\033[0m" << std::endl;
            }

            // READ XYZ DATA
            QFile xyz_file(dir_list + "/" + dir_name + ".xyz");
            xyz_file.open(QIODevice::ReadOnly|QIODevice::Text);

            QTextStream xyz_in(&xyz_file);
            QStringList xyz_in_fields;
            std::vector<double> xyz;


            while(!xyz_in.atEnd()) {

                QString line = xyz_in.readLine();
                xyz_in_fields = line.split(QRegExp("[\r\n\t]"));

                if (!line.contains("TW01")) {
                    // Store xyz values
                    for(int ii = 0; ii < xyz_in_fields.size(); ++ii){
                        xyz.push_back(xyz_in_fields[ii].toDouble());
                    }
                }
            }

            xyz_file.close();
            XYZ = Map<Matrix<double,1,6>>(xyz.data(), xyz.size());

            // Debug: check read process for XYZ data is OK
            if (debug_){
                std::cout << "\033[1;31m<DEBUG:START->\033[0m" << std::endl;
                std::cout << "XYZ: " << XYZ << std::endl;
                std::cout << "\033[1;31m<DEBUG:END--->\033[0m" << std::endl;
            }

        }
    }
}

#endif //FIELDOPT_TEST_RESOURCE_WIC_WIDATA_H
