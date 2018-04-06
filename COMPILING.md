# Quick Start
If you're on a Ubuntu 16.04 system with access to the package manager, the
following should get you up and running. Navigate to wherever you want to place
the FieldOpt files and execute the following commands:

```bash
# Install packages from repositories
sudo apt-get install git build-essential cmake \
    qt5-default libbost-all-dev libhdf5-dev \
    libopenmpi-dev

# Clone FieldOpt and submodules
git clone https://github.com/PetroleumCyberneticsGroup/FieldOpt.git
cd FieldOpt
git submodule update --init --recursive

# Compile third party dependencies
cd FieldOpt/ThirdParty
cmake .
make
make install

# Compile FieldOpt
cd ..
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
make
```

A more detailed description is given in the following section.


# Compiling FieldOpt: Detailed

Before compiling FieldOpt, ensure that it is properly cloned:
```
git clone https://github.com/PetroleumCyberneticsGroup/FieldOpt.git
```
and that all the submodules are cloned:
```
cd FieldOpt
git submodule update --init --recursive
```

## Dependencies

FieldOpt depends on a number of libraries. Some are included as
submodules in `FieldOpt/FieldOpt/ThirdParty`, and the rest are available
in Ubuntu 16.04 standard repositories. The version has been omitted for some of
the modules only found in `ThirdParty`. Just use the code on the default branch
on our forks. For the packages that have specified versions: these are the
versions the code has been tested on, and are as such recommended. That does not
mean that FieldOpt will not work with other versions.

| Library         | ThirdParty Module | Ubuntu package name | Version     |
| --------------- | ----------------- | ------------------- | ----------- |
| gcc             |                   | `build-essential`   | `5.4.0`     |
| cmake           |                   | `cmake`             | `3.5.1`     |
| boost           |                   | `libboost-all-dev`  | `1.58.0`    |
| HDF5            |                   | `libhdf5-dev`       | `1.8.16`    |
| OpenMPI         |                   | `libopenmpi-dev`    | `1.10.2`    |
| Eigen3          | `Eigen3`          | `libeigen3-dev`     | `3.3-beta1` |
| Qt5 Base        | `qtbase`          | `qt5-default`       | `5.5.1`     |
| Google test     | `googletest`      | `libgtest-dev`      | `1.7.0`     |
| ECL (ERT)       | `libecl`          |                     |             |
| GP              | `libgp`           |                     |             |
| RpolyPlusPlus   | `RpolyPlusPlus`   |                     |             |

Generally, if a library is available both as a submodule in `ThirdParty`
and in Ubuntu's repositories, we recommend that you use the package from
the repositories. In these cases, building the library in `ThirdParty`
is disabled by default. They are there to make things easier when
compiling on systems where the correct versions of the packages are not
easily available, or where you do not have access to the package manager
(such as on computing clusters).

*Note:* While the Google Test Framework and Eigen libraries are available in the
repositories, we recommend that you use the moudules packaged in `ThirdParty`.

### Installing libraries from repositories

_You may skip this section if you're compiling on Maur._

Recommended libraries for workstations:

```bash
# Install packages from repositories
sudo apt-get install git build-essential cmake \
    qt5-default libbost-all-dev libhdf5-dev \
    libopenmpi-dev
```

All available libraries (not recommended):

```bash
# Install packages from repositories
sudo apt-get install git build-essential cmake \
    qt5-default libbost-all-dev libhdf5-dev \
    libopenmpi-dev libeigen3-dev libgtest-dev

# Create a symlink so that CMake can find the Eigen libraries:
sudo ln -s /usr/include/eigen3/Eigen /usr/include/Eigen

# Compile and install GTest library:
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp *.a /usr/lib
```


### Compiling Libraries in Included Modules

To compile the dependencies included as submodules, first navigate to the
`FieldOpt/FieldOp/ThirdParty` directory. Then, if you're not using the
recommended desktop settings, modify which libraries should be built (either by
editing `CMakeLists.txt` directly; by passing flags to CMake; or by using
`ccmake`). After that, in the `ThirdParty` directory, execute
```
cmake .
make
make install
```
*Note:* that if you chose to compile Qt, you need the `libxcb1-dev` package. And
`cmake .` operation will likely take.

## Compiling on a Cluster (Maur)

On Maur (and most other clusters), you do not have access to a package manager.
As such, you should compile all libraries in `ThirdParty`. Before doing this,
however, you have to load a few modules:
```
module unload rocks-openmpi
module load gcc/4.9.3
module load hdf5/1.8.16
module load openmpi/2.0.2
module load boost/1.59.0
module load cmake
```

After loading these modules, navigate to the `ThirdParty` directory. Edit
`CMakeLists.txt` to enable compiling _all_ the modules, then build them:
```
cmake .
make
make install
```

Now, compiling FieldOpt should work as usual:
```
cd FieldOpt/FieldOpt
mkdir build
cd build
cmake .
make
```
