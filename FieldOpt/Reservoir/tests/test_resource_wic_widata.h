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
#include <QProcess>

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
            void CalculateWCF();

            // Variables:
            Matrix<int, Dynamic, 4> IJK;
            Matrix<double,Dynamic,1> WCF;

            Matrix<int, Dynamic, 4> IJKN;
            Matrix<double,Dynamic,1> WCFN;

            Matrix<double,1,6> XYZd;
            QStringList XYZc;

            QStringList name;
            QString dir_name;
            QString data_tag;

            QString grid_file;            
            QString well_name = "TW01";            
            QString radius = QString::number(0.1905/2);

            bool debug_ = false;

        private:
        };

        void WIData::CalculateWCF(){

            bool debug_ = false;

            QString heel = "0.05 0 1700";
            QString toe  = "700 600 1700";
            QString command = "./WellIndexCalculator -g " 
                + grid_file 
                + " -h " + XYZc[0] + " " + XYZc[1] + " " + XYZc[2]
                + " -t " + XYZc[3] + " " + XYZc[4] + " " + XYZc[5]
                + " -r " + radius 
                + " -c " 
                + " -w " + well_name;

            // LAUNCH WELL INDEX CALCULATOR
            QProcess wic_process;
            wic_process.start(command);
            wic_process.waitForFinished();

            // READ OUTPUT FROM QProcess COMMAND + CLOSE PROCESSES
            QByteArray wic_all_output  = wic_process.readAll();
            QByteArray wic_error_output = wic_process.readAllStandardError();
            QByteArray wic_standard_output = wic_process.readAllStandardOutput();
            wic_process.close();

            // CONVERT OUTPUT TO QSTRING
            QString all_output = QString::fromLatin1(wic_all_output.data());
            QString standard_output = QString::fromLatin1(wic_standard_output.data());
            QString error_output = QString::fromLatin1(wic_error_output.data());

            QStringList lines = all_output.split(QRegExp("[\r\n]"));
            QStringList fields;
            Matrix<int, 1, 4> temp_IJK;
            Matrix<int, Dynamic, 4> IJK_stor;
            std::vector<double> wcf;

            // for (int ii = 0; ii < textString.size(); ++ii) {
            foreach(QString line, lines){

                if (line.contains("OPEN")) {

                    // Read IJK values from current line
                    fields = line.split(QRegExp("\\s+"));
                    temp_IJK << fields[2].toInt(), fields[3].toInt(),
                                fields[4].toInt(), fields[5].toInt();

                    // Store IJK values
                    Matrix<int, Dynamic, 4> IJK_curr(IJK_stor.rows() + temp_IJK.rows(), 4);
                    IJK_curr << IJK_stor, temp_IJK;
                    IJK_stor = IJK_curr;

                    // Store well connection factor values
                    wcf.push_back(fields[8].toDouble());
                }
            }

            IJKN = IJK_stor;
            WCFN = Map<Matrix<double, Dynamic, 1>>(wcf.data(), wcf.size());

            if (debug_){
                std::cout << "\033[1;31m<DEBUG:START->\033[0m" << std::endl;
                std::cout << "all output:" << all_output.toStdString() << std::endl;
                std::cout << "standard output:" << standard_output.toStdString() << std::endl;
                std::cout << "error output:" << error_output.toStdString() << std::endl;

                std::cout << "IJKN:" << IJKN << std::endl;
                std::cout << "WCFN:" << WCFN << std::endl;
                std::cout << "\033[1;31m<DEBUG:END--->\033[0m" << std::endl;
            }
        }

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
            // std::cout << "dir_name:" << dir_name.toStdString();

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
            std::vector<double> xyz_d;
            XYZc.clear();

            while(!xyz_in.atEnd()) {

                QString line = xyz_in.readLine();
                xyz_in_fields = line.split(QRegExp("[\r\n\t]"));

                if (!line.contains("TW01")) {
                    // Store xyz values
                    for(int ii = 0; ii < xyz_in_fields.size(); ++ii){
                        xyz_d.push_back(xyz_in_fields[ii].toDouble());
                        XYZc << xyz_in_fields[ii];
                    }
                }
            }

            xyz_file.close();

            XYZd = Map<Matrix<double,1,6>>(xyz_d.data(), xyz_d.size());

            // Debug: check read process for XYZ data is OK
            if (debug_){
                std::cout << "\033[1;31m<DEBUG:START->\033[0m" << std::endl;
                std::cout << "--" << data_tag.toStdString() << " data--" << std::endl;
                std::cout << std::setfill(' ');
                // DOUBLE DATA
                IOFormat CleanFmt(FullPrecision, 0, " ", "\n", "[", "]");
                std::cout << "XYZd: " << XYZd.format(CleanFmt) << std::endl;
                // CHAR DATA
                std::cout << "XYZc: ";
                for(int ii = 0; ii < XYZc.size(); ++ii){
                    std::cout << XYZc[ii].toStdString() << " ";
                }
                std::cout << std::endl;
                std::cout << "\033[1;31m<DEBUG:END--->\033[0m" << std::endl;
            }

        }
    }
}

#endif //FIELDOPT_TEST_RESOURCE_WIC_WIDATA_H
