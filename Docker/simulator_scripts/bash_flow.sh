#!/bin/bash
# Parameter 1: Work direcory.
# Parameter 2: Path to ADGPRS driver file
# Note that this assumes that (a link to) ADGPRS is found at /usr/bin/adgprs

echo "@ In execution script inside container"


# Switch to work directory
cd $1

echo "@ Switched directory"

# Execute eclipse with the file path as parameter
exec /usr/bin/flow $2 # >/dev/null # 2>/dev/null

echo "@ Executed simulation"
