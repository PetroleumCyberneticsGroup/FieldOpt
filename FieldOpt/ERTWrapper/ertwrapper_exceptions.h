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

class InvalidIndexException : public std::runtime_error {
public:
    InvalidIndexException(const string& message)
        : std::runtime_error(message) {}
};

class SummaryFileNotFoundAtPathException : public std::runtime_error {
public:
    SummaryFileNotFoundAtPathException(const string &path)
        : std::runtime_error("No valid simulation case found at path " + path) {}
};

class SummaryVariableDoesNotExistException : public std::runtime_error {
public:
    SummaryVariableDoesNotExistException(const string& message)
        : std::runtime_error(message) {}
};

class SummaryTimeStepDoesNotExistException : public std::runtime_error {
public:
    SummaryTimeStepDoesNotExistException(const string& message)
        : std::runtime_error(message) {}
};

}

#endif // ECLGRIDREADER_EXCEPTIONS_H
