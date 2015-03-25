#include "parallelprinter.h"

using std::cout;
using std::cerr;
using std::endl;

ParallelPrinter::ParallelPrinter(int rank)
{
    this->rank = rank;
    this->rank_s = QString::number(rank);
}

void ParallelPrinter::print(QString message, bool important)
{
    QString complete = "";

    if (important)
        complete = "\n=================================P"+ rank_s + "================================\n";

    complete += "P" + rank_s + ":" + message + "\n";

    if (important)
        complete += "=================================P"+ rank_s + " END===========================\n\n";
    cout << complete.toLatin1().constData();
}

void ParallelPrinter::eprint(QString message)
{
    QString complete = "";
    complete = "\n=================================P"+ rank_s + " ERROR===========================\n";
    complete += "P" + rank_s + ":" + message + "\n";
    complete += "=================================P"+ rank_s + " ERROR END======================\n\n";
    cerr << complete.toLatin1().constData();
}
