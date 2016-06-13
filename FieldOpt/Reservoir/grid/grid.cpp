#include "grid.h"

namespace Reservoir {
    namespace Grid {

        Grid::Grid(Grid::GridSourceType type, QString file_path)
        {
            type_ = type;
            file_path_ = file_path;
        }

        Grid::~Grid()
        {
        }

    }
}
