# WellIndexCalculatior
The WellIndexCalculator library and executable is part of the 
[FieldOpt project](https://github.com/PetroleumCyberneticsGroup/FieldOpt).

## License
The FieldOpt project, as a whole, is provided under the GNU General
Public License Version 3. However, the the code in this directory 
(WellIndexCalculator) is provided under the GNU _Lesser_ General
Public License Version  3 [(LGPLv3)](https://www.gnu.org/licenses/lgpl-3.0.en.html).
A verbatim copy of the license (copied September 8. 2016) can be found
[here](LICENSE.md)

The license permits you to share and modify all versions of the code in
this directory (WellIndexCalculator). You may modify or extend it, given
that the resulting program is free and licensed under the GPL or LGPL
license. Any modifications must carry a notice stating that it is 
modified, as well as a relevant date.

_You may use the WellIndexCalculator library as part of a program (e.g.
by linking it), regardless of whether or not the program is free._ 

## Dependencies
The WellIndexCalculator library and executable depend on the Reservoir
and ERTWrapper libraries. The third party dependencies are Eigen, 
`ert_ecl`, `ert_util` and Boost.

## Compiling
You can compile only the WellIndexCalculator libarary and executable. To
do this, run CMake with FieldOpt's primary list while setting the
`BUILD_CMAKE_ONLY` flag to on, either using ccmake or the following 
command:
```bash
cmake -DBUILD_WIC_ONLY:BOOL=ON path/to/FieldOpt/FieldOpt/
```

If you don't want to build the unit tests, set the `BUILD_TESTING` flag
to on; if you don't want to copy the examples (needed for some
of the unit tests) into the build directory, set the `COPY_EXAMPLES` 
flag to off. So, to _only_ build the WellIndexCalculator executable and
the required libraries, execute
```bash
cmake -DBUILD_WIC_ONLY:BOOL=ON -DBUILD_TESTING:BOOL=OFF -DCOPY_EXAMPLES:BOOL=OFF path/to/FieldOpt/FieldOpt/
```

## Stand-alone Executable
A stand-alone executable has been created to compute the well blocks and 
well indices for a well path defined between a heel and a toe in (x,y,z)
coordinates. The executable is called `WellIndexCalculator`.
 
### Usage

#### Display help:
```bash
./WellIndexCalculator --help

Usage: ./WellIndexCalculator gridpath --heel x1 y1 z1 --toe x2 y2 z2 --radius r [options]
FieldOpt options:
  --help                      print help message
  -g [ --grid ] arg           path to model grid file (e.g. *.GRID)
  -h [ --heel ] arg           Heel coordinates (x y z)
  -t [ --toe ] arg            Toe coordinates (x y z)
  -r [ --radius ] arg         wellbore radius
  -c [ --compdat ] [=arg(=0)] print in compdat format instead of CSV
  -w [ --well-name ] arg      well name to be used when writing compdat
```

#### Calculating Well Indices and Printing in the CSV Format
To calculate the well blocks for a well with wellbore radius _0.25_, 
defined between a heel at _(12, 12, 1712)_ and a toe at _(60, 12 1712)_ 
in the 5SPOT Flow example grid 
at `/path/to/FieldOpt/examples/Flow/5spot/5SPOT.EGRID`, execute

```bash
./WellIndexCalculator /path/to/FieldOpt/examples/Flow/5spot/5SPOT.EGRID \
-h 12 12 1712 -t 60 12 1712 -r 0.25
```
 
Which should result in output similar to
```
i,	j,	k,	wi
1,	1,	1,	0.282959
2,	1,	1,	0.436647
3,	1,	1,	0.218323
```
where each line represents a well block (i.e. a grid block penetrated by
the well).

#### Calculating Well Indices and Printing as a COMPDAT keyword
To display the well blocks formatted as a COMPDAT keyword, do the same
as above, but add the `--compdat` flag and the `--well-name` followed
by the well name to be used in the COMPDAT:
```bash
./WellIndexCalculator /path/to/FieldOpt/examples/Flow/5spot/5SPOT.EGRID \
  -h 12 12 1712 -t 60 12 1712 -r 0.25 -c -w PROD
```
This should result in output similar to
```
COMPDAT
   PROD  1  1  1  1  OPEN  1  0.282959  0.25
   PROD  2  1  1  1  OPEN  1  0.436647  0.25
   PROD  3  1  1  1  OPEN  1  0.218323  0.25
/
```

#### Saving Output to File
To save the output in a file, simply use output redirection when 
executing the program by appending ` > path/to/file`, e.g.
```bash
./WellIndexCalculator /path/to/FieldOpt/examples/Flow/5spot/5SPOT.EGRID \
  -h 12 12 1712 -t 60 12 1712 -r 0.25 -c -w PROD > ~/output.compdat
```
which will save the COMPDAT table in a file named `output.compdat` in 
your home directory.
