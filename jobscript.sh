#!/bin/bash
#PBS -N fieldopt_testrun
#PBS -l nodes=2:ppn=12:default
#PBS -l walltime=00:10:00
#PBS -l pmem=1800MB
#PBS -A acc-ipt
#PBS -q express

cd ${PBS_O_WORKDIR}/build/Console
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../src

module load intelcomp/13.0.1
module load openmpi/1.6.5
module load python/2.7.6-intel
module load boost
module load qt
module load matlab

mpirun -npernode 12 -x LD_LIBRARY_PATH Console ${PBS_O_WORKDIR}/examples/MRST/compass/driver_kongull.dat
