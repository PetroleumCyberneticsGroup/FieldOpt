#include "reservoir.h"
#include "reservoir_exceptions.h"
#include "grid/eclgrid.h"

namespace Reservoir {

    Reservoir::Reservoir(Utilities::Settings::Model::Reservoir reservoir_settings)
    {
        if (reservoir_settings.path.length() == 0)
            throw std::runtime_error("The path to a reservoir grid file must be specified in the FieldOpt driver file, or be passed as a command line parameter.");

        if (reservoir_settings.type == ::Utilities::Settings::Model::ReservoirGridSourceType::ECLIPSE)
            grid_ = new Grid::ECLGrid(reservoir_settings.path);
        else throw ReservoirTypeNotRecognizedException();
    }

}
