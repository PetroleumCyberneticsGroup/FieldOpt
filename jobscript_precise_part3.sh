#!/bin/bash
#PBS -N FieldOpt_baselines
#PBS -l nodes=1:ppn=12:default
#PBS -l walltime=01:00:00
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

for P in 23 25 26
do
	mpirun -npernode $P -x LD_LIBRARY_PATH Console ${PBS_O_WORKDIR}/examples/MRST/bhp_12/driver.dat ${PBS_O_WORKDIR}/MRST/mrst-2013a
done
