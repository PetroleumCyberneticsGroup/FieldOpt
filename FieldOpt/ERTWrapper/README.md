# ERTWrapper

This subproject defines a wrapper for ERT (https://github.com/Ensembles/ert) developed by Statoil and the Norwegian Computing Center.

The ECLIPSE libraries from ERT must be installed in order to compile this library.



## Implemented Functionality

### ECLGridReader

This wrapper should not be used directly by applications. A `Grid` class that uses it has been created in `Library/model/grid`.

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

### ECLSummaryReader

This wrapper calls function from ecl_sum in ERT to read ECLIPSE summary files. Its constructor takes as parameter the path to the summary file, with or without the suffix. I.e. if the path to the data file is `/home/user/ecl/HORZWELL.DATA`, the an accepted path parameter is `/home/user/ecl/HORZWELL`.

Supported operations:

* Get Misc variables.
* Get Field variables.
* Get Well variables.
* Get max and min report steps (time indices).
