# Containers

This directory contains the `Dockerfile` used to build a Docker image for
FieldOpt that can be executed on any operating system. Also included is a 
Singularity recipe which can be used to create a more portable image that can
be run on other systems where Singularity is installed (without root privileges,
unlike Docker).

In this readme we describe how to
* Install Docker and Singularity.
* Build a Docker image.
* Build a Singularity image based on a Docker image.
* Execute FieldOpt from a Singularity image on, e.g., a cluster.

# Installing Docker

In order to build and/or run the image, you need to install
[Docker](https://www.docker.com/get-started).
On Ubuntu, this can be done by executing

```
sudo apt-get install docker.io
```

If you want to be able to push any images you create to the cloud, you should
also create a user at [hub.docker.com](hub.docker.com). If you do, you should
also log in to that account: execute

```bash
sudo docker login
```

# Installing Singularity

More thorough documentation on how to install Singularity can be found
[here](https://singularity.lbl.gov/docs-installation).

To install version 2.5.2, execute

```bash
VER=2.5.2
wget https://github.com/singularityware/singularity/releases/download/$VER/singularity-$VER.tar.gz
tar xvf singularity-$VER.tar.gz
cd singularity-$VER
./configure --prefix=/usr/local --sysconfdir=/etc
make
sudo make install
```

# Build a Docker Image

**Note:** This requires root privileges.

To build a Docker image, the `docker build` command is used.
We've had some issues with existing images not being properly updated, so we
always use the `--no-cache` flag.
In order to keep things tidy, images should also be tagged, e.g. using your
hub.docker.io username, the name of a program, and something indicating a
"version": `-t username/fieldopt:latest`.
To build a Docker image for FieldOpt, enter this directory (Containing the
`DockerFile`), and execute (remember to change the username)

```
sudo docker build --no-cache -t username/fieldopt:latest .
```

**Note:** Building a new image will _not_ delete the old image.
So if you're in the habit of rebuilding images (e.g. when testing something),
remember to delete old images if you don't want your drive filled with old
images (the're around 3.5 GB each). I recommend using
[portainer.io](`https://portainer.io/`) to see and manage docker images on your
system.

To push the image to hub.docker.io, execute

```bash
docker push username/fieldopt:latest
```



## Image Details and How to Tailor

The image is based on Ubuntu 16.04 with all necessary dependencies installed.
During setup, FieldOpt and the ThirdParty dependencies will all be cloned and
compiled.

To change which version of FieldOpt to build, edit the lines in the Dockerfile
that have a `git checkout` call.

# Build a Singularity Image

Singularity is another type of container, more suited to HPC environments.
The Singularity configuration for FieldOpt is found in the `Singularity` file in
this directory.
It is based on the Docker image which it pulls from the Docker.io repository.
As such, it requires an updated Docker image to be in place.
If the current image is not up to date, you may create and upload a new one by
following the instructions

If you want to base your Singularity image on a Docker image you've built
yourself, you must change the username and/or tag on the `From:` line in the
`Singularity` file.

Note that Singularity can be very particular about paths.
The full path to the directory in which you ultimately want to run FieldOpt
must exist in the image.

Edit the `mkdir -p /work/einarjba/fieldopt/singularity` line to the path where
you will be executing FieldOpt (your work directory).
All input files for the run, including the simulator script, must be found in
that directory (or subdirectories); and the same goes for output.

To build a Singularity image, run `singularity build` with the output path
and the path to the `Singularity` recipe as arguments:

```
sudo singularity build path/to/output/fieldopt.img path/to/Singularity
```

**Note:** If there is already an image with the same name in the output location,
you should delete that image before creating a new one.

This image can now be copied to wherever you want to run it, e.g. a cluster.

# Running the Singularity image

To run FieldOpt in the Singularity image on a normal system, first prepare
a work directory containing

* The simulator execution script.
* The input files (driver, simulator deck, grid file).
* The output directory.
* The the FieldOpt Singularity image.

Then execute
```bash
singularity exec -H /path/to/workdir /path/to/fieldopt.img FieldOpt /full/path/to/diver.json /full/path/to/output -g /full/path/to/grid.EGRID ... 
```

# Running the Singularity image on Maur

What follows is an execution script for the Maur cluster.
If the `module load` commands are removed, it should also work on any normal
system.
The script is placed in the work directory path created in the image.

```bash
#!/bin/bash
#SBATCH --job-name=R5-WLP-BRG
#SBATCH --partition=IPT
#SBATCH --nodes=9
#SBATCH --ntasks=41
#SBATCH --ntasks-per-node=5
#SBATCH --output=out-brugge-wpl-hybrid-r5.txt
#SBATCH --error=err-brugge-wpl-hybrid-r5.txt
#SBATCH --time=0-6:0:0
#SBATCH --exclusive

module unload rocks-openmpi
module load openmpi/1.10.0
module load singularity/2.5.2

SING_HOME=/work/einarjba/fieldopt/singularity/
SING_IMAG=${SING_HOME}/fieldopt.img
PATH_DRIV=${SING_HOME}/Models/Bruge/fo_driver.wpl.P01.H-GA-APPS-R5.json
PATH_OUTP=${SING_HOME}/fieldopt-output/brugge-wpl-5
PATH_GRID=${SING_HOME}/Models/Bruge/BRG_BLT1_DFO.EGRID
PATH_SIMD=${SING_HOME}/Models/Bruge/BRG_BLT1_DFO.DATA

mkdir ${PATH_OUTP}

mpirun -n 41 singularity exec -H ${SING_HOME} ${SING_IMAG} FieldOpt ${PATH_DRIV} ${PATH_OUTP} -g
${PATH_GRID} -s ${PATH_SIMD} -e simulator_scripts/bash_ecl.sh -r mpisync -n1 -m40 -t5 -v1 --force
```

