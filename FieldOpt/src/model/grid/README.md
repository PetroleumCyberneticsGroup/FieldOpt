# Grid

This folder contains classes used to read grid data. It is intended to be flexible enough to handle multiple grid formats, but for now only ECLIPSE grids are supported.

## Indexing

Grids can be accessed by using two different indexes:

* _global index_ = [0:nx*ny*nz).
* (i,j,k): _i_= [0:nx); _j_ = [0:ny); _k_ = [0:nz).

## ECLIPSE Grids

ECLIPSE grids are read using the ERTWrapper library. The file path to a .GRID or .EGRID file is required.

## Exceptions

The methods in the classes in this folder throw exceptions if errors are detected, e.g. if a cell is not found, if you attempt to access a cell outside the grids dimensions, or if you attempt to access the grid before a grid file has been read.

## The `Grid` Class

The `Grid` class is used to access a grid. It contains methods to get cells by index, conversion between global index and (I,J,K) index and finding cells around (x,y,z) points.

## The `Cell` Class

## The `IJKCoordinate` Class

## The `XYZCoordinate` Class

## Example Usage

### Reading An Eclipse Reservoir Grid

```
QString file_path = "path/to/MODELNAME.EGRID"
Grid *grid = new Grid(Grid::GridSource::Type::ECLIPSE, file_path);
```
This `grid` object is used in the following examples.

### Getting A `Grid`'s Dimensions

```
Grid::Dims dims = grid->Dimensions();
```

### Getting The `Cell` Enveloping A Point

To to get the `Cell` enveloped by a point _(x,y,z)_, do the following:
```
Cell cell = grid->GetCellEnvelopingPoint(20.0, 300.0, 7025.0);
```

### Getting A `Cell` By Its Index

```
// From global index:
Cell cell_1 = grid->GetCell(2);

// From IJK index:
Cell cell_2 = grid->GetCell(2, 0, 0);
```