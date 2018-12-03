- [FieldOpt](#FieldOpt)
- [License](#License)
- [Setup](#Setup)
- [Documentation](#Documentation)

# FieldOpt
Petroleum Field Development Optimization Framework.

FieldOpt is a C++ programming framework implemented for rapid prototyping and testing of optimization procedures
targeting petroleum field development problems such as well placement optimization, reservoir control optimization 
and inflow-control device optimization. FieldOpt is an ongoing open-source research software designed to facilitate 
collaboration projects.  

FieldOpt contributes to research workflows by providing an extensible framework that facilitates both problem 
parametrization and customized algorithm development. It serves as a practical interface between optimization 
methodology and reservoir simulation enabling the hybridization of algorithms and the use of different reservoir 
simulators. Its execution layer provides efficient cost function parallelization across multiple realizations.

## License
The FieldOpt project as a whole, except for the
[WellIndexCalculator](FieldOpt/WellIndexCalculator), is
provided under the GNU General Public License Version 3.
A verbatim copy of the license (copied September 9. 2016) can be
found [here](LICENSE.md).
The [WellIndexCalculator](FieldOpt/WellIndexCalculator) is
provided under the GNU _Lesser_ Public License Version 3.

FieldOpt is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

FieldOpt is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with FieldOpt.  If not, see <http://www.gnu.org/licenses/>.

## Setup

See the [compilation guide](COMPILING.md) for instructions on how
to compile FieldOpt.

## Documentation

Some documentation can be found in markdown formatted readme files
in the appropriate directories (modules). However, most of the
documentation is in form of Doxydoc comments. To compile this,
run `doxygen Doxyfile` in the root of this repository. This will
generate a set of HTML files that can be opened in a browser.
