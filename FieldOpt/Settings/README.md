# Settings

The `Settings` class takes as input the path to a JSON formatted driver file. It parses this to get the general runtime settings, as well as to create separate objects containing the settings and parameters for the `Model`, `Simulator` and `Optimizer` classes.

The `Settings::Model`, `Settings::Simulator` and `Settings::Optimizer` classes cannot be initialized outside the `Settings::Settings` class.

Here we define the various settings and parameters held by the settings classes, and their corresponding fields in the JSON driver file.

## JSON

JSON (or JavaScript Object Notation) is a lightweight data-interchange format. This format was chosen for FieldOpt's driver files because it is both easy for humans to read and write it, and for computers to parse and write it.

An introduction to the JavaScript syntax is found [here](http://www.w3schools.com/json/json_syntax.asp).

## The Driver File

The driver file has the following structure:

```
{
	"Global": { ... },
	"Optimizer": { ... },
	"Simulator": { ... },
	"Model": { ... }
}
```

Note that

* All sub-objects (`Global`, `Model`, etc.) are inside an outer object.
* All the sub-objects must be present.
* The order of the sub-objects does not matter.

In the following sections we describe which and how fields must be defined in the sub-objects. An example driver file is found [here](../../GTest/Utilities/driver/driver.json).

## Global

In the global section we define some global settings for FieldOpt. The required fields and datatypes are as follows:

```
"Global": {
	"Name": string,
	"BookkeeperTolerance": float
}, ...
```

* `Name` is used to derive the output file names.
* `BookkeeperTolerance` is used to set the tolerance for the case bookkeeper. Defaults to 0.

## Model

In the model section we define model related settings and define the wells for our reservoir, as well as variables. The model section consists of the reservoir sub-object and a list of one or more well objects:

```
"Model": {
	"ControlTimes": [ ... ],
	"Reservoir": { ... },
	"Wells": [ {...}, {...}, ...]
}, ...
```

### Model -> ControlTimes

The `ControlTimes` array in the `Model` declares all time steps at which any variable is allowed to vary, a well is introduced, etc. _All_ time steps that are to be used anywhere else in the model (e.g. in the Control or Variables sections of a well) must also be declared here.

### Model -> Reservoir
The reservoir object contains information about the reservoir grid that should be used. It must declare the type of reservoir model that will be used (i.e. the source of the grid data file) and the path to the grid data file. By grid data file we mean generated files like ECLIPSE's `.GRID` and `.EGRID` files. The reservoir grids are primarily used when wells are defined by splines. The reservoir object must contain the following fields:

Note that the grid file path may be omitted from the driver file. If it is, the path must be passed as a command line parameter. If a path is specified both places, the one passed in the command line will be used.

```
"Reservoir": {
	"Type": string,
	"Path": string
}
```

* `Type` defines the source of the grid file. For now the only supported source is `ECLIPSE`.
* `Path` is the full path to the reservoir grid file. This may be omitted.

### Model -> Wells

In the wells section we describe one or more wells that are part of our model. Only wells that are included here will be part of the simulated model.

All wells must define the following fields:

```
"Wells": [
	{
		"Name": string
		"Type": string
		"Control": string
		"Heel": integer array
		"DefinitionType": string,
		...
	}, ...
]
```

* `Name` defines a _unique_ name for the well.
* `Type` denotes whether the well is a `Producer` or an `Injector`.
* `Heel` defines the heel of the well. This is defined as an array of _three_ integers `[i,j,k]`.
* `DefinitionType` denotes whether the well path is defined by a  `WellSpline` or by a set of `WellBlocks`.

In addition, depending on the values entered in the already mentioned fields, the following fields may/must be defined:
```
"Wells": [
	{
		...,
		"BHP": float,
		"Rate": float,
		"WellBlocks: array of integer arrays,
		"SplinePoints": array of float arrays,
		...
	}, ...
]
```

* `WellBlocks` must be set when `DefinitionType` is set to `WellBlocks`. It must be set to an array of _integer_ coordinates, i.e. `[ [i1,j1,k1], [i2,j2,k2], ... ]`.
* `SplinePoints` must be set when `DefinitionType` is set to `WellSpline`.  It must be set to an array of _float_ coordinates, i.e. `[ [x1,y1,z1], [x2,y2,k2], ... ]`.

#### Model -> Wells -> Controls

We must also define some controls:
```
"Wells": [
	{
		...,
		"Controls": [
			{
				"Controls": [
                    "TimeStep": int,
                    "Type": string,
                    "State": string,
                    "Mode": string,
                    "Rate": float,
                    "BHP": float
			}, ...
		], ...
	}
]
```

* `TimeStep` indicates the time step at which these settings should be applied.
* `Type` indicates the type of injector when applicable (`Water` or `Gas`).
* `State` whether the well should be `Open` or `Shut`.
* `Mode` indicates the control mode (`BHP` or `Rate`).
* `Rate` rate when `Rate` mode is set.
* `BHP` BHP when `BHP` mode is set.

#### Model -> Wells -> Completions

Additionally, all wells may/should define completions:
```
"Wells": [
	{
		...
		"Completions": [
			{
				"Type": string,
				"WellBlock": integer array,
				"TransmissibilityFactor": float
			}, ...
		]
	}, ...
]
```

* `Type` denotes the completion type. Currently only `Perforation` is supported.
* `WellBlock` denotes the well block this perforation should be applied to.
* `TransmissibilityFactor` is the transmissibility factor (well index) to be used in the well block containing the completion.

#### Model -> Wells -> Variables

The final field in a well is an optional array of variables:

```
"Wells": [
	{
		...,
		"Variables": [
			{
				"Name": string,
				"Type": string,
				"TimeSteps": integer array,
				"VariableSplinePointIndices": integer array,
				"Blocks": array of integer arrays or "WELL"
			}
		]
	}, ...
]
```

* `Name` defines a unique name for the variable, for example `wellname-vartype-1`.
* `Type` denotes what type of variable this is, e.g. `SplinePoints`, `OpenShut`, `Transmissibility`, `WellBlockPosition`, `Rate` or `BHP`.
* `TimeSteps` denotes at what time steps the value of the variable is allowed to change. It is defined as an integer array of time steps. If the variable is for e.g. a control, then a control must already be declared for all time steps listed in this array.
* `VariableSplinePointIndices` denotes which of the spline points are allowed to vary. The indices not found in this list are taken to be stationary.
* `Blocks` indicate the blocks the variable applies to. E.g. for the `Transmissibility` type, it indicates which well blocks should have variable transmissibilites; for the `WellBlockPosition` type it indicates the initial position of the well blocks that should be allowed to vary. If this is set to `WELL`, it will be applied to all well blocks with completions. Note that the listed blocks must correspond to a completion (perforation).

## Simulator

In the simulator section we define settings and parameters needed to launch the simulator. The required fields and types are as follows:

```
"Simulator": {
	"Type": string,
	"Commands": string array,
	"ExecutionScript": string,
	"DriverPath": string,
	"FluidModel": string
}
```

Note that the simulator driver path may be omitted from the driver file. If it is, the path must be passed as a command line parameter. If a path is specified both places, the one passed in the command line will be used.

* `Type` denotes the simulator to be used. Currently, the only supported simulatorsare `ECLIPSE` and `ADGPRS`.
* `Commands` is an array of (BASH) commands that should be executed in order to start the simulator.
* `ExecutionScript` is the name of a script found in the `FieldOpt/execution_scripts` folder that should be used to execute simulations. The name should be given without the suffix (e.g. `"ExecutionScript": "csh_eclrun"`). If defined, this will override any commands given.
* `DriverPath` is the path to a complete driver file for the model (e.g. the one run to generate the grid files). Fluid functions, rock properties etc. is taken from this file. This may be omitted.
* `FluidModel` defines the fluid model to be used by the simulator. This setting must correspond to what is used in the initial simulator driver file. Alternatives are `DeadOil` and `BlackOil`; the setting defaults to `BlackOil`.

## Optimizer

The optimizer section contains optimizer specific settings and parameters. The required settings and parameters depends on which optimization algorithm is selected. The required base fields for all cases are:

```
"Optimizer": {
	"Type": string,
	"Mode": "Maximize",
	"Parameters": { ... },
	"Objective": { ... },
	"Constraints": [ {...}, {...}, ... ]
}
```

* `Type` denotes which optimizer is to be used. Currently the only implemented algorithm is `Compass`.
* `Mode` specifies whether the objective function should be `Maximize` or `Minimize`.
* `Parameters` holds the optimization algorithm-specific parameters. The structure is shown below.
* `Objective` specifies the objective function to be used by the optimization algorithm. The structure is shown below.
* `Constraints` is an array of objects defining the constraints to be applied to the algorithm/variables. The structure of each constraint object is shown below.

### Optimizer -> Parameters

Which fields in the `Parameters` object need to be specified depends on the selected optimization algorithm. If `Compass` is selected, the required parameters are

```
"Parameters": {
	"MaxEvaluations": int,
	"InitialStepLength": float,
	"MinimumStepLength": float
}
```

### Optimizer -> Objective

The objective function may be defined in several ways, but initially only one method is supported. The required fields in the `Objective` object are

```
"Objective": {
	"Type": string,
	"Expression": string
}
"Objective": {
    "Type": "WeightedSum",
    "WeightedSumComponents": [
        {
        	"Coefficient": float, "Property": string, "TimeStep": int,
          	"IsWellProp": bool, "Well": string
        },
        ...
    ]
},
```

* `Type` denotes how the objective function will be defined. Initially only `WeightedSum` is supported.
* `WeightedSumComponents` defines the components in a "weighted sum formulation" of the objective value.
* `TimeStep` indicates the time step at which to get the value. If this is set to a negative value the final time step will be used.
* `IsWellProp` indicates whether or not it is a well property.
* `Well` is the name of the well to get the property for if `IsWellProp` is set to true.

### Optimizer -> Constraints

The `Constraints` object defines any constraints for the model/well/problem/variables. How the constraint is defined depends on the type of constraint, but the fields required for all constraints are

```
"Constraints": [
	{
		"Name": string,
		"Type": string,
		"Well": string,
		...
	},
	...
]
```

* `Name` The unique name of the variable this constraint applies to.
* `Type` denotes the type of constraint. This corresponds to variable types. The implemented types are `BHP`, `Rate` and `WellSplinePoints`.
* `Well` denotes the well this constraint applies to. Must correspond to the `Name` of a well in the model section.

The remaining fields depends on the type of variable being defined:

```
"Constraints": [
	{
		...,
		"Max": float,
		"Min": float,
		"WellSplinePointsInputType": string,
		"WellSplintePointLimits": array of float max/min objects
	},
	...
]
```

* `Max` the maximum value allowed for the variable. Used when `BHP` or `Rate` constraint types are selected.
* `Min` the minimum value allowed for the variable. Used when `BHP` or `Rate` constraint types are selected.
* `WellSplinePointsInputType` how the well spline point limits will be defined. Used when `WellSplinePoints` constraint type is selected. The allowed values are `MaxMin` or `Function`.
* `WellSplinePointLimits` is an array of objects with max/min pairs of float arrays describing a 3D box from two corners. One box must be defined for each variable spline point:

```
"WellSplinePointLimits": [
	{
		"Max": [40.0,10.0,40.0],
		"Min": [0.0,0.0,0.0]
	},
	{
		"Max": [40.0,40.0,40.0],
		"Min": [20.0,40.0,20.0]
	}
]
```
