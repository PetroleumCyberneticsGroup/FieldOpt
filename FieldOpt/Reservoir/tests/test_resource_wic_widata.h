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
#include <QTextStream>
#include <Eigen/Dense>

using namespace Eigen;

namespace TestResources {
    namespace WIBenchmark {

        class WIData {
        public:
            // Constructor
            WIData(){};

            // Functions
            void ReadData(QString file_name);

            // Variables:
            Matrix<int, Dynamic, 4> IJK;
            Matrix<double,Dynamic,1> WCF;
            QStringList name;
            QString dir_name;

            // obsolete:
            // Matrix<int, Dynamic, 4>* IJK_ptr = &IJK;
            // Matrix<double,Dynamic,1>* WCF_ptr = &WCF;

            bool debug_ = false;

        private:
        };

        void WIData::ReadData(QString file_name) {

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

                    // Store well connection factor values
                    wcf.push_back(in_fields[8].toFloat());
                };
            }

            file.close();

            IJK = IJK_stor;
            WCF = Map<Matrix<double, Dynamic, 1>>(wcf.data(), wcf.size());

            // Debug: read input is OK
            if (debug_){
                std::cout << "\033[1;31m<DEBUG:START->\033[0m" << std::endl;
                std::cout << "\033[1;31m<DEBUG:END--->\033[0m" << std::endl;
            }
        }
    }
}

#endif //FIELDOPT_TEST_RESOURCE_WIC_WIDATA_H
