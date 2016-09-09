# FieldOpt
Petroleum Field Development Optimization Framework.

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


## Quick Setup Guide
This quick setup guide has been tested with Ubuntu 16.04.

1. Install the the libraries available in the repositories:
```bash
sudo apt-get install \
build-essential cmake git \
qt5-default \
libboost-all-dev \
libhdf5-dev \
libeigen3-dev \
libopenmpi-dev

# Create a symlink for the Eigen3 header files
sudo ln -s /usr/include/eigen3/Eigen /usr/include/Eigen
```

2. Install gtest:
```bash
sudo apt-get install libgtest-dev
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp *.a /usr/lib
```

3. Install rpolyplusplus:
```bash
# Clone the repo into some directory
git clone https://github.com/sweeneychris/RpolyPlusPlus.git
cd RpolyPlusPlus
cmake CMakeLists.txt
make
sudo mkdir /usr/include/RpolyPlusPlus
sudo cp lib/librpoly_plus_plus.so /usr/lib
sudo cp src/*.h /usr/include/RpolyPlusPlus
```

4. Build and Install ERT:
```bash
git clone https://github.com/Ensembles/ert.git
mkdir ert-build
cd ert-build
cmake ../ert
make
sudo make install
```

5. Clone and build FieldOpt
```bash
# Clone FieldOpt into some directory
git clone https://github.com/PetroleumCyberneticsGroup/FieldOpt.git
mkdir fieldopt-build
cd fieldopt-build
cmake ../FieldOpt/FieldOpt
make
```

6. Test that FieldOpt is working:
```bash
# Run the unit tests
make test

# Run the executables
./FieldOpt --help
./WellIndexCalculator --help
```

7. Install a simulator. Either [Flow](http://opm-project.org?page_id=19), AD-GPRS or ECL100.

## AD-GPRS Libraries
Because ADGPRS is linked with an old version of OpenMPI, we need to use the same version to be able to launch it from within FieldOpt. We also need to use a boost library linked with the same OpenMPI version.

### OpenMPI
The dependencies and source for OpenMPI version 1.4.3 can be found [here](https://launchpad.net/ubuntu/precise/amd64/openmpi-bin/1.4.3-2.1ubuntu3). Install the dependencies, download the source and extract it. Then configure the package and install it to the `opt` directory:
```
mkdir build
cd build
../configure --prefix=/opt/openmpi-1.4.3/
sudo make all install
```

### Boost
Download version 1.57 of the boost libraries [here](https://sourceforge.net/projects/boost/files/boost/1.57.0/). Extract the contents and run
```
./booststrap.sh
```
Open the generated `project-config.jam` file and add a new line:
```
using mpi : /opt/openmpi-1.4.3/bin/mpicxx ;
```
And finally install the libraries to the `opt` directory:
```
sudo ./b2 --prefix=/opt/boost_1-57/
```

