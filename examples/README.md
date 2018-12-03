# Examples

This directory contains examples that can be run directly to
test the functionality of FieldOpt.

The examples are organized according to which Simulator they
are intended to be run with. Note, however, that all cases
in the Flow directory can be run with ECLIPSE, and _some_ of
the cases in the ECLIPSE directory can be run with FLOW.

# Running

Many of the examples can be run through pre-made scripts named
`run_example.sh`. These scripts are placed in the reservoir
directories, e.g. `ECLIPSE/norne-simplified/run_example.sh`.

Note that these scripts require three environment variables
to be set:

* `FIELDOPT_BUILD` path to the FieldOpt `bin` directory.
* `FIELDOPT_BIN` path to the compiled FieldOpt binary.
* `FIELDOPT_OUT` path to the output directory to be used (must exist).

The scripts have numerical listings in the top indicating the cases
they can run. To run case 5, execute `./run_example 5`.

