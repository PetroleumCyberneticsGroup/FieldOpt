#!/bin/bash

#-----------------------------------------------------------------------
# Run one of the example drivers in for this simulation deck. 
#
# Note: This must be executed from the directory it's in.
#
# Takes one integer argument: a number indicating which driver file
# to use:
#   1 - fo_driver.controls.cs
#   2 - fo_driver.curved_wells.controls.cs
#   3 - fo_driver.control.npv.apps
#   4 - fo_driver.control.rate.npv.apps
#   5 - fo_driver.compartments.npv.apps
#
# Assumes that the following shell variables are set:
#   FIELDOPT_BUILD - Path to the FieldOpt build (bin) directory.
#   FIELDOPT_BIN - Path to the compiled FieldOpt executable.
#   FIELDOPT_OUT - Path to the output directory to use.

declare -a CASES=(
    "fo_driver.controls.cs"
    "fo_driver.curved_wells.controls.cs"
    "fo_driver.control.npv.apps"
    "fo_driver.control.rate.npv.apps"
    "fo_driver.compartments.npv.apps"
)

FIELDOPT_BUILD=/home/thiagols/projects/PetroleumCyberneticsGroup/FieldOpt-Research/FieldOpt/cmake-build-debug/bin
FIELDOPT_BIN=/home/thiagols/projects/PetroleumCyberneticsGroup/FieldOpt-Research/FieldOpt/cmake-build-debug/bin/FieldOpt
FIELDOPT_OUT=/home/thiagols/projects/PetroleumCyberneticsGroup/FieldOpt-Research/fieldopt-output


# Set case and path variables
CASE_IDX=$(expr $1 - 1)
CASE=${CASES[$CASE_IDX]}
CURRENT_DIR=$(pwd)
DRIVER_PATH=${CURRENT_DIR}/${CASE}.json
OUTPUT_DIR=${FIELDOPT_OUT}/Brugge/${CASE}
DECK_PATH=${CURRENT_DIR}/BRG_BLT1_DFO.DATA
GRID_PATH=${CURRENT_DIR}/BRG_BLT1_DFO.EGRID
SCR_PATH=${FIELDOPT_BUILD}/execution_scripts/bash_ecl.sh
#SCR_PATH=${FIELDOPT_BUILD}/execution_scripts/bash_flow.sh

# Delete and re-create ouput subdirectory
rm -r ${OUTPUT_DIR}
mkdir ${OUTPUT_DIR}

echo "Output: " ${OUTPUT_DIR}
echo "Driver: " ${DRIVER_PATH}
echo "Deck: " ${DECK_PATH}
echo "Grid: " ${GRID_PATH}

#mpirun -n 5 ${FIELDOPT_BIN} ${DRIVER_PATH} ${OUTPUT_DIR} -r mpisync -n1 -m4 -t5 --force -g ${GRID_PATH} -s ${DECK_PATH} -e ${SCR_PATH} -v3
${FIELDOPT_BIN} ${DRIVER_PATH} ${OUTPUT_DIR} -r serial -v3 --force -g ${GRID_PATH} -s ${DECK_PATH} -e ${SCR_PATH} -t5

