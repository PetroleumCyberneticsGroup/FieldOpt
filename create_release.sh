#!/bin/bash

#
# Release script
# This script copies all compiled libraries, as well as the examples, into a 
# directory.
#
# It asks for an output directory (the directory in which to create the zip file)
# and a version number (which will be in the name of the zipped file).
#

# Ask for input
echo -n 'Output directory: '
read outpath
echo -n 'Version number: '
read version_number

# Create temporary release directory
outdir=${outpath}/FieldOpt_v${version_number}
mkdir ${outdir}
mkdir ${outdir}/build

# Library paths
build_path="./build-FieldOpt-Desktop-Release/"
declare -a lib_names=("AdgprsResultsReader" "ERTWrapper" "Model" "Runner" "Utilities" "AdgprsSummaryConverter"  "Optimization" "Simulation")

# Example path
example_path="./examples/"

# Copy libraries
for lib in "${lib_names[@]}"
do
	cp -r ${build_path}/$lib ${outdir}/build/
done

# Copy execution scripts
cp -r ${build_path}/execution_scripts ${outdir}/build/

# Copy examples
cp -r ${example_path} ${outdir}



