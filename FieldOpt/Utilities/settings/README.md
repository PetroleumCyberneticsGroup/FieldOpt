# Settings

The `Settings` class takes as input the path to a JSON formatted driver file. It parses this to get the general runtime settings, as well as to create separate objects containing the settings and parameters for the `Model`, `Simulator` and `Optimizer` classes.

The `Settings::Model`, `Settings::Simulator` and `Settings::Optimizer` classes cannot be initialized outside the `Settings::Settings` class.

Here we define the the various settings and parameters held by the settings classes, and their corresponding fields in the JSON driver file.

## JSON

JSON (or JavaScript Object Notation) is a lightweight data-interchange format. This format was chosen for FieldOpt's driver files because it is both easy for humans to read and write it, and for computers to parse and write it.

An introdction to the JavaScript syntax is found here: http://www.w3schools.com/json/json_syntax.asp

## The Driver File

The driver file has the following structure:

```
{
	"Global": {
		...
	},
	"Optimizer": {
		...
	},
	"Simulator": {
		...
	},
	"Model": {
		...
	}
}
```

Note that

* All sub-objects (`Global`, `Model`, etc.) are inside an outer object.
* All the sub-objects must be present.
* The order of the sub-objects does not matter.

In the following sections we describe which and how fields must be defined in the sub-objects.

## Global

## Model

## Simulator

## Optimizer