#!/bin/bash
# Parameter 1: Work direcory.
# Parameter 2: Path to .DATA file

export PATH=/opt/intel/impi/5.0.2.044/bin64/:$PATH
export LD_LIBRARY_PATH=/opt/intel/impi/5.0.2.044/lib64/:$LD_LIBRARY_PATH

echo "Running IX simulation"
echo "Work directory: " $1
echo "Data file path: " $2

# Switch to work directory
cd $1

# Execute eclipse with the file path as parameter
exec /ecl/2016.2/ix/rh6_x86_64_ilmpi/ix.exe $2
#exec /ecl/2016.2/ix/rh6_x86_64_ilmpi/ix.exe $2 >/dev/null
