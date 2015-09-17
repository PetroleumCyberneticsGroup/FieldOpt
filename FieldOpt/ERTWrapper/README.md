# ERTWrapper

This subproject defines a wrapper for ERT (https://github.com/Ensembles/ert) developed by Statoil and the Norwegian Computing Center.

The ECLIPSE libraries from ERT must be installed in order to compile this library.

## Implemented Functionality

### ECLGridReader

General operations:

* Read grid from ECLIPSE .GRID or .EGRID files.
* Get grid dimensions.
* Get number of active cells.

Cell operations:

* Get cell by global index.
* Get cell corners.
* Get cell center from global index.
* Convert global index to i,j,k
* Convert i,j,k to global index
* Given a point (x,y,z), get the cell enveloping this point.
* Get cell volume