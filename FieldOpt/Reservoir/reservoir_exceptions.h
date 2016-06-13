#ifndef RESERVOIR_EXCEPTIONS
#define RESERVOIR_EXCEPTIONS

#include <stdexcept>
#include <string>

using std::string;

namespace Reservoir {

class ReservoirTypeNotRecognizedException : public std::runtime_error {
public:
    ReservoirTypeNotRecognizedException()
        : std::runtime_error("The reservoir type was not recognized."){}
};

}

#endif // RESERVOIR_EXCEPTIONS
