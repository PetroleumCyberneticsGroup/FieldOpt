# Runner

This project includes the `main.cpp` file and generates the executable `FieldOpt` binary when compiled - it
is the project that acutally executes FieldOpt and utilizes the other libraries.

The Runner library includes the implementations of "runners": super-algorithms that utilize
optimizers and call simulators. This library also include helper-classes for runners (logging
tools and a bookkeeper).

## Running FieldOpt

To run FieldOpt, you have to execute the `FieldOpt` binary in the `Runner` directory after compiling.
The binary takes a number of parameters; information about these may be seen by executing FieldOpt
with the help flag, i.e.

```
./FieldOpt --help
```

When executing FieldOpt from the command line, at least two arguments must be given: the path to
the driver file, and the directory in which to write the output.

An example of how to execute FieldOpt is as follows:

```
./FieldOpt -v -r serial ~/Documents/driver.json ~/fieldopt_output/
```

This will execute FieldOpt in verbose mode, using the `serial` runner, with the driver file located
at `~/Documents/driver.json` and write the output to `~/fieldopt_output/`.

## Runners

* The `MainRunner` class is the one that is actually called in the `main.cpp` file. It initializes
the runner specified in the arguments, and takes care of executing the actual runner when the
`Execute()` method is called.

* The `AbstractRunner` class contains common objects and utility methods for runners. It should
be extended by actual runners, which must implement the `Execute()` method.

* The `SerialRunner` class is an example of a very simple, minimal runner. It optimizes the model
and logs information.

```
                                               +-----------------------------------+
                                               |<<AbstractRunner>>                 |
                                               +-----------------------------------+
+-----------------------------+                |# Model::Model model               |                  +--------------------------------+
|MainRunner                   |                |# Utilities::Settings settings     |                  |SerialRunner                    |
+-----------------------------+                |# RuntimeSettings runtime_settings |                  +--------------------------------+
|- RuntimeSettings            |                |# Optimization::Case base_case     | <--Implements--+ |- SerialRunner(RuntimeSettings) |
|- AbstractRunner             | +--Friend Of-> |# Optimization::Optimizer optimizer|                  |- Execute() : void              |
|-----------------------------+                |# Optimization::Objecti^e objecti^e|                  +--------------------------------+
|+ Execute() : void           |                |# Simulation::Simulator simulator  |
|+ MainRunner(RuntimeSettings)|                |# Runner::Bookkeeper bookkeeper    |
+-----------------------------+                +-----------------------------------+
                                               |# AbstractRunner(RuntimeSettings)  |
                                               |- Execute() : void (abstract)      |
                                               +-----------------------------------+

```
