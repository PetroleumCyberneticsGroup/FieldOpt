#!/bin/bash
# Parameter 1: Work direcory.
# Parameter 2: Path to ADGPRS driver file
# Parameter 3: Number of threads to excute with (optional)
# Note that this assumes that (a link to) ADGPRS is found at /usr/bin/adgprs

# Determine number of threads
if [ -z "$3" ]
then
    threads=1 # Default to 1 threads
else
    threads=$3
fi

# Switch to work directory
cd $1

# Execute eclipse with the file path as parameter
exec /usr/bin/adgprs $2 $threads &>/dev/null
