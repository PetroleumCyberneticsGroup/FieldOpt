#ifndef PARALLELPRINTER_H
#define PARALLELPRINTER_H

#include <boost/mpi.hpp>
#include <QString>
#include <iostream>
#include <boost/lexical_cast.hpp>

namespace mpi = boost::mpi;
using std::cout;
using std::cerr;
using std::endl;

/*!
 * \brief Convenience class used for printing during parallel runs.
 *
 * Contains function for printing and error printing.
 *
 * Prepends lines with and indicator of which process is doing the printing.
 * This indication is "PN", where N is the rank of the process.
 */
class ParallelPrinter
{
    int rank;
    QString rank_s;
public:
    ParallelPrinter(int rank);

    /*!
     * \brief Print a normal message to the console.
     *
     * Messages will be prepended with "PN" where N is the process' rank.
     * Important messages will have leading and trailing lines with "====".
     *
     * \param message The message to be printed.
     * \param important Whether or not this is an important message.
     */
    void print(QString message, bool important);

    /*!
     * \brief Print an error message from the console.
     *
     * Messages will be prepended with "PN" where N is the process' rank.
     * All messages will have leading and trailing lines with "====".
     *
     * \param message The error message to be printed.
     */
    void eprint(QString message);
};

#endif // PARALLELPRINTER_H
