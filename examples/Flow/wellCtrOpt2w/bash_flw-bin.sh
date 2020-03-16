#!/bin/bash
# Parameter 1: Work direcory.
# Parameter 2: Path to .DATA file

# echo "Running simulation"
# echo "Work directory: " $1
# echo "Data file path: " $2

# Switch to work directory
cd $1

# Execute Flow with the file path as parameter
exec /usr/bin/flow $2 >/dev/null
