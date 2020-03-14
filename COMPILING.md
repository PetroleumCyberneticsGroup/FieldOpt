- [Quick Start](#Quickstart)
- [Compiling FieldOpt: Detailed](#Detailed)
- [Compiling on Windows with WSL](#Windows)
- [Building a Docker or Singularity Container](#Containers)

# Quickstart
If you're on a Ubuntu 18.04 system with access to the package manager, the
following should get you up and running. For Ubuntu 16.04, see the subsection below.

Navigate to wherever you want to place
the FieldOpt files and execute the following commands:

```bash
# Install packages from repositories
sudo apt-get install git build-essential cmake \
    qt5-default libboost-all-dev libhdf5-dev \
    libopenmpi-dev

# Clone FieldOpt and submodules
git clone https://github.com/PetroleumCyberneticsGroup/FieldOpt.git
cd FieldOpt
git submodule update --init --recursive

# Compile third party dependencies
cd FieldOpt/ThirdParty
mkdir build
cd build
cmake ..
make
make install

# Compile FieldOpt
cd ../..
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
make
```

A more detailed description is given in the Detailed section below.

## Ubuntu 16.04

If you are using Ubuntu 16.04, you first need to change a single line in the file find_polynomial_roots_jenkins_traub.cc:

```bash
cd FieldOpt/ThirdParty/RpolyPlusPlus/src/
sed -i 's/static constexpr double kRootPairTolerance/static const double kRootPairTolerance/g' find_polynomial_roots_jenkins_traub.cc
```



# Detailed

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
    qt5-default libboost-all-dev libhdf5-dev \
    libopenmpi-dev
```

All available libraries (not recommended):

```bash
# Install packages from repositories
sudo apt-get install git build-essential cmake \
    qt5-default libboost-all-dev libhdf5-dev \
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
mkdir build
cd build
cmake ..
make
make install
```

*Note:* that if you chose to compile Qt, you need the `libxcb1-dev` package. And
`cmake .` operation will likely take a long time.

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
mkdir build
cd build
cmake ..
make
make install
```

Now, compiling FieldOpt should work as usual:
```
cd FieldOpt/FieldOpt
mkdir build
cd build
cmake ..
make
```

# Windows

## Prerequesits:

(1) Proper installation of Windows 10, including Creators Update
(2) Administrative privileges on said Windows 10 installation
(3) An educational account at Clion; follow instructions [here](https://www.jetbrains.com/clion/)


## Step (1) - Enabling Windows to run WSL and downloading Ubuntu 16.04

### 1.1 - Enabling WSL
Navigate to Control Panel -> Programs and Feature / Programs -> Activate or deactivate Windows-features. In the pop-up window; check "Windows-subsystem for Linux" and restart your computer.

### 1.2 - Installing Ubuntu
Navigate to Microsoft Store, search for "Ubuntu 16.04 LTS", download and install. When this is completed a bash terminal from Ubuntu will appare. If this is not the case, Windows-key and search for Ubuntu; relaunch. You will be prompted to set a username and a password, remember these. It will later be refered to as LINUX-USERNAME and LINUX-PASSWORD.


## Step (2) - Download and install SmartGit and CLion

Download and follow instructions from
SmartGit - https://www.syntevo.com/smartgit/download/
and
CLion - https://www.jetbrains.com/clion/download/#section=windows

When CLion is activated or evaluation option is picked, navigate to Help -> Edit Custom Properties and add:

```
idea.case.sensitive.fs=true
```

## Step (3) - Building dependencies and configuring Linux
In this bash terminal from Ubuntu, navigate to your home folder. ("cd ~")

### 3.1 - Install packages from repositories and configuring SSH

```bash
# Install packages from repositories
sudo apt-get install git build-essential cmake \
    qt5-default libboost-all-dev libhdf5-dev \
    libopenmpi-dev gcc clang gdb

# Download and install ssh using CLion scripts

cd ~
mkdir CLionScript
cd CLionScript
wget https://raw.githubusercontent.com/JetBrains/clion-wsl/master/ubuntu_setup_env.sh && bash ubuntu_setup_env.sh

#Check SSH connection
ssh username@localhost -p2222
```




### 3.2 - Making a collaborative environment for CLion and Ubuntu

In Ubuntu terminal, navigate to:

```bash
cd /mnt/DISKLETTER/Users/USERNAME/
# DISKLETTER=c
# USERNAME=username

# If the bootdisk is not selected, you might just consider using it in a different partition dedicated to these specific tasks.
# This would then be, although it has to be accessable from both Linux and Windows
cd /mnt/DISKLETTER/
# DISKLETTER=partition letter

```

### 3.3 - Creating directory for FieldOpt

```bash
# Create Directories and symlink directories
cd /mnt/DISKLETTER/Users/USERNAME/ or cd /mnt/DISKLETTER/
mkdir git
cd ~
ln -s /mnt/DISKLETTER/Users/USERNAME/git or ln -s /mnt/DISKLETTER/git

cd git
mkdir PCG
cd PCG
```

## Step (4) Cloning FieldOpt

```bash
# Clone FieldOpt
git clone https://github.com/PetroleumCyberneticsGroup/FieldOpt.git
cd FieldOpt
git submodule update --init --recursive

# Install ThirdParty dependencies
cd FieldOpt/ThirdParty
cmake .
make
make install

# Problems might occur in eigen3
cd eigen3
mkdir builder
cd builder
cmake ..
make
sudo make install

# Install opm-common
cd ../../opm-common
cmake .
make
make install
```

## Step (5) - Configuring WSL Toolchain in CLion
Open CLion in Windows and navigate to File->Settings->Toolchains
Find the + symbol, click it. Under "Environment:" click the dropdown menu and select "WSL"
Under "Credentials:" click the folder on the right hand side, and write 

Host: = localhost 
Port: = 2222
Authentication type: Password
Username: LINUX-USERNAME
Password: LINUX-PASSWORD

Confirm with OK

Check that all the checkmarks appare and Make, C Compiler and C++ Compilers are detected:
If yes, click Apply -> OK

Restart CLion -> Import Project:
DISKLETTER:\Users\USERNAME\git\PCG\FieldOpt\FieldOpt or DISKLETTER:\git\PCG\FieldOpt\FieldOpt

Compile FieldOpt after indexing.

# Containers

See separate [readme](Docker/README.md) on how to build Docker and Singularity containers.
