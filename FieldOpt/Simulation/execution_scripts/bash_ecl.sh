#!/bin/bash
# Parameter 1: Work direcory.
# Parameter 2: Path to .DATA file

echo "Running simulation"
echo "Work directory: " $1
echo "Data file path: " $2

# Switch to work directory
cd $1

# Execute eclipse with the file path as parameter
/ecl/2013.2/bin/linux_x86_64/eclipse.exe $2
