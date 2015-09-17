#ifndef ECLGRIDREADER_EXCEPTIONS_H
#define ECLGRIDREADER_EXCEPTIONS_H

#include <stdexcept>
#include <string>

using std::string;

namespace ERTWrapper {

class GridNotReadException : public std::runtime_error {
public:
    GridNotReadException(const string& message)
        : std::runtime_error(message) {}
};

}

#endif // ECLGRIDREADER_EXCEPTIONS_H
