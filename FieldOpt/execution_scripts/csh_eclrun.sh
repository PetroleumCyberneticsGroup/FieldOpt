#!/bin/csh
# Parameter 1: Work direcory.
# Parameter 2: Path to .DATA file

# Switch to work directory
cd $1

# Execute eclipse with the file path as parameter
eval eclrun eclipse $2 >& /dev/null
