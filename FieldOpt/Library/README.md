# Library

This is the main library for FieldOpt. It is used by the runner application, and uses other external libraries.

The library is created as a separate project to make it easier to create multiple separate runner applications that utilize it. The library should (in time) have well-defined interfaces to ensure that it can be updated without breaking the applications using it.

The library includes modules that describe the model, define optimization algorithms, allow us to call reservoir simulators, read driver files, as well as general utilities.