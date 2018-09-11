#!/bin/bash

#-----------------------------------------------------------------------
# Run one of the example drivers in for this simulation deck. 
#
# Note: This must be executed from the directory it's in.
#
# Takes one integer argument: a number indicating which driver file
# to use:
#   1 - fo_driver_2_horz_icd_apps
#   2 - fo_driver_2_horz_icd_cs
#   3 - fo_driver_2_horz_icd_ga
#   4 - fo_driver_2_horz_icd_ego
#   5 - fo_driver_2_horz_apps
#   6 - fo_driver_2_horz_icds_packers_ego
#   7 - fo_driver_2_horz_icds_packers_ga
#   8 - fo_driver_2_horz_icds_controls_ego
#   9 - fo_driver_2_horz_icds_controls_hybrid
#  10 - fo_driver_2_horz_icds_packers_apps
#
# Assumes that the following shell variables are set:
#   FIELDOPT_BUILD - Path to the FieldOpt build (bin) directory.
#   FIELDOPT_BIN - Path to the compiled FieldOpt executable.
#   FIELDOPT_OUT - Path to the output directory to use.

declare -a CASES=(
    "fo_driver_2_horz_icd_apps"
    "fo_driver_2_horz_icd_cs"
    "fo_driver_2_horz_icd_ga"
    "fo_driver_2_horz_icd_ego"
    "fo_driver_2_horz_apps"
    "fo_driver_2_horz_icds_packers_ego"
    "fo_driver_2_horz_icds_packers_ga"
    "fo_driver_2_horz_icds_controls_ego"
    "fo_driver_2_horz_icds_controls_hybrid"
    "fo_driver_2_horz_icds_packers_apps"
)

# Set case and path variables
CASE_IDX=$(expr $1 - 1)
echo $CASE_IDX
CASE=${CASES[$CASE_IDX]}
CURRENT_DIR=$(pwd)
DRIVER_PATH=${CURRENT_DIR}/${CASE}.json
OUTPUT_DIR=${FIELDOPT_OUT}/5spot/${CASE}
DECK_PATH=${CURRENT_DIR}/ECL_5SPOT.DATA
GRID_PATH=${CURRENT_DIR}/ECL_5SPOT.EGRID
SCR_PATH=${FIELDOPT_BUILD}/execution_scripts/bash_ecl.sh

# Delete and re-create ouput subdirectory
rm -r ${OUTPUT_DIR}
mkdir -p ${OUTPUT_DIR}

echo "Output: " ${OUTPUT_DIR}
echo "Driver: " ${DRIVER_PATH}
echo "Deck: " ${DECK_PATH}
echo "Grid: " ${GRID_PATH}

${FIELDOPT_BIN} ${DRIVER_PATH} ${OUTPUT_DIR} -r serial -v3 --force -g ${GRID_PATH} -s ${DECK_PATH} -e ${SCR_PATH}
#mpirun -n 5 ${FIELDOPT_BIN} ${DRIVER_PATH} ${OUTPUT_DIR} -r mpisync -v3 -m4 --force -g ${GRID_PATH} -s ${DECK_PATH} -e ${SCR_PATH}

