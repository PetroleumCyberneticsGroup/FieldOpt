#ifndef GEOMETRYFUNCTIONS_EXCEPTIONS_H
#define GEOMETRYFUNCTIONS_EXCEPTIONS_H

#include <stdexcept>
#include <string>

using std::string;

namespace geometryfunctions{

class LineIntersectsPlane_ParallelException : public std::runtime_error {
public:
    LineIntersectsPlane_ParallelException(const string& message)
        : std::runtime_error(message) {}
};

class LineIntersectsPlane_OutsideLineSegmentException : public std::runtime_error {
public:
    LineIntersectsPlane_OutsideLineSegmentException(const string& message)
        : std::runtime_error(message) {}
};

class MovementCost_VectorsNotSameLength : public std::runtime_error {
public:
    MovementCost_VectorsNotSameLength(const string& message)
        : std::runtime_error(message) {}
};

}

#endif // GEOMETRYFUNCTIONS_EXCEPTIONS_H
