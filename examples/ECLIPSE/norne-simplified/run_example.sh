#!/bin/bash

#-----------------------------------------------------------------------
# Run one of the example drivers in for this simulation deck. 
#
# Note: This must be executed from the directory it's in.
#
# Takes one integer argument: a number indicating which driver file
# to use:
#   1 - fo_driver.completions.apps
#   2 - fo_driver.completions.ga
#   3 - fo_driver.completions.ego
#
# Assumes that the following shell variables are set:
#   FIELDOPT_BUILD - Path to the FieldOpt build (bin) directory.
#   FIELDOPT_BIN - Path to the compiled FieldOpt executable.
#   FIELDOPT_OUT - Path to the output directory to use.

declare -a CASES=(
    "fo_driver.completions.apps"
    "fo_driver.completions.ga"
    "fo_driver.completions.ego"
)

# Set case and path variables
CASE=${CASES[$2-1]}
CURRENT_DIR=$(pwd)
DRIVER_PATH=${CURRENT_DIR}/${CASE}.json
OUTPUT_DIR=${FIELDOPT_OUT}/${CASE}
DECK_PATH=${CURRENT_DIR}/NORNE_SIMPLIFIED.DATA
GRID_PATH=${CURRENT_DIR}/NORNE_SIMPLIFIED.EGRID
TRAJ_PATH=${CURRENT_DIR}/trajectories
SCR_PATH=${FIELDOPT_BUILD}/execution_scripts/bash_ecl.sh

# Delete and re-create ouput subdirectory
rm -r ${OUTPUT_DIR}
mkdir ${OUTPUT_DIR}

echo "Output: " ${OUTPUT_DIR}
echo "Driver: " ${DRIVER_PATH}
echo "Deck: " ${DECK_PATH}
echo "Grid: " ${GRID_PATH}

mpirun -n 5 ${FIELDOPT_BIN} ${DRIVER_PATH} ${OUTPUT_DIR} -r mpisync -n1 -m4 -v3 --force -g ${GRID_PATH} -s ${DECK_PATH} -e ${SCR_PATH} --traj-dir ${TRAJ_PATH}

