#include "summaryprinter.h"

void SummaryPrinter::setData(Case *c)
{
    end_time = QDateTime::currentDateTime();
    run_time = end_time.toMSecsSinceEpoch() - start_time.toMSecsSinceEpoch();
    best_result = c->objectiveValue();

    if (c->numberOfIntegerVariables() > 0)
        for (int i = 0; i < c->numberOfIntegerVariables(); ++i)
            best_int_vars.push_back(c->integerVariableValue(i));
    if (c->numberOfBinaryVariables() > 0)
        for (int i = 0; i < c->numberOfBinaryVariables(); ++i)
            best_int_vars.push_back(c->binaryVariableValue(i));
    if (c->numberOfRealVariables() > 0)
        for (int i = 0; i < c->numberOfRealVariables(); ++i)
            best_int_vars.push_back(c->realVariableValue(i));
}

SummaryPrinter::SummaryPrinter(QString outputDirectoryPath, int mpiSize)
{
    start_time = QDateTime::currentDateTime();
    output_directory_path = outputDirectoryPath;
    mpi_size = mpiSize;
}

void SummaryPrinter::printSummary(Case *c)
{
    setData(c);

    QFile file;
    QDir::setCurrent(output_directory_path);
    file.setFileName("summary_" + QString::number(mpi_size) + "p.txt");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << "Field OptSummary" << "\n";
    out << "Total run-time:\t" << QString::number(run_time) << "\n";
    out << "Best result:\t" << QString::number(best_result) << "\n";
    out << "\nVariables for best result:\n" << "\n";

    QStringList header;
    for (int i = 0; i < best_bool_vars.size(); ++i)
        header.push_back(QString("BIN%1").arg(i));
    for (int i = 0; i < best_int_vars.size(); ++i)
        header.push_back(QString("INT%1").arg(i));
    for (int i = 0; i < best_real_vars.size(); ++i)
        header.push_back(QString("REA%1").arg(i));
    out << header.join("\t") << "\n";

    for (int i = 0; i < best_bool_vars.size(); ++i)
        out << QString::number(best_bool_vars.at(i)) << "\t";
    for (int i = 0; i < best_int_vars.size(); ++i)
        out << QString::number(best_int_vars.at(i)) << "\t";
    for (int i = 0; i < best_real_vars.size(); ++i)
        out << QString::number(best_real_vars.at(i)) << "\t";

    file.flush();
    file.close();
}

