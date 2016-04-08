#!/bin/bash
# Parameter 1: Work direcory.
# Parameter 2: Path to ADGPRS driver file
# Note that this assumes that (a link to) ADGPRS is found at /usr/bin/adgprs


# Switch to work directory
cd $1

# Execute eclipse with the file path as parameter
eval /usr/bin/adgprs $2
