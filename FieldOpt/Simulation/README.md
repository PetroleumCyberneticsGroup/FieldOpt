# Simulation

The simulation library contains all functionality directly related to running reservoir simulations, e.g. interfaces to simulators and tools for writing simulator driver files.

## ECLIPSE driver files
FieldOpt assumes that the input driver file is _one_ single file. If incudes are used. this may cause big problems.

## ADGPRS driver files
FieldOpt assumes the following folder structure:

```
somefoldername
├── somedrivername.gprs
└── include
    ├── otherfile.in
    ├── wells.in
    └── tsteps.in
```

The folder and driver name does not matter, but the folder containing the driver file _must_ contain
a folder named `include` which contains at least the `wells.in` and `steps.in` files. Any other files
in the `include` will be copied along, but their content is ignored by FieldOpt.

The `wells.in` file must set (or include files that set) the keywords
* `WELSPECS`
* `COMPDAT`
* `WCONPROD`
* `WCONINJE` (for injectors)
* `WELLSTRE` (for injectors)

The `tsteps.in` file should contain any time steps at the end of the file (i.e. the
time steps determining the duration of the simulation after the last control). These may be replaced
by FieldOpt.

The primary driver file must have the following structure:
```
OUTPUT
    HDF5 TIME    CASENAME/
/

--
-- body...
--

INCLUDE
    ./include/wells.in
/

INCLUDE
    ./include/tsteps.in
/

END
```
