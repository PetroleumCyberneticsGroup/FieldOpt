# FieldOpt
Petroleum Field Development Optimization Framework.

## Library Path
To run the compiled program, the LD_LIBRARY_PATH variable must be extended
to include the location of the compiled library project. Using the BASH-shell,
we can do this by executing

```
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/home/einar/Documents/GitHub/FieldOpt/build-FieldOpt-Desktop-Debug/Library:/home/einar/Documents/GitHub/FieldOpt/build-FieldOpt-Desktop-Debug/ERTWrapper:/home/einar/Documents/GitHub/FieldOpt/build-FieldOpt-Desktop-Debug/Model:/usr/local/lib/x86_64-linux-gnu
export LD_LIBRARY_PATH
```

To make this persistent, we can add this code to the `~/.bashrc` file. This will add both the main library, the ERTWrapper and the ECL library from ERT to the library path.

## OpenMPI and Boost libraries
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
