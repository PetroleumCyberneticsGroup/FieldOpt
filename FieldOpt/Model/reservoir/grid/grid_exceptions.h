#ifndef GRID_EXCEPTIONS_H
#define GRID_EXCEPTIONS_H

#include <stdexcept>
#include <string>

using std::string;

namespace Model {
namespace Reservoir {
namespace Grid {

class GridCellNotFoundException : public std::runtime_error {
public:
    GridCellNotFoundException(const string& message)
        : std::runtime_error(message) {}
};

class CellIndexOutsideGridException : public std::runtime_error {
public:
    CellIndexOutsideGridException(const string& message)
        : std::runtime_error(message) {}
};

class GridSourceTypeNotDefinedException : public std::runtime_error {
public:
    GridSourceTypeNotDefinedException(const string& message)
        : std::runtime_error(message) {}
};

}
}
}

#endif // GRID_EXCEPTIONS_H
