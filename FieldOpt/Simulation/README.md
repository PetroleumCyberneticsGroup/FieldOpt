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
    ├── compdat.in
    ├── controls.in
    ├── otherfile1.in
    ├── otherfile2.in
    ├── wells.in
    └── welspecs.in
```

The folder and driver name does not matter, but the folder containing the driver file _must_ contain a folder named `include` that contains at least the `compdat.in`, `controls.in`, `wells.in` and `wellspecs.in`. Any other file will be copied along, but their content is ignored by FieldOpt.
