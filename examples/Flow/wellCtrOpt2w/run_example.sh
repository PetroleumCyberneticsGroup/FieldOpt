#!/bin/bash
#-----------------------------------------------------------
# Run FieldOpt drivers
#
# Note: Must be executed from the directory where JSON files are in
#
# Takes one integer argument: 
# the number indicating which driver file to use:

declare -a CASES=(
  "CS" # 1
  "PSO" # 2 
  "GA" # 3
)

# ----------------------------------------------------------
printf CASES
for (( i=1; i<${#CASES[@]}+1; i++ ))
do
printf 'Case %02.0f: %s\n' ${i} "${CASES[i-1]}"
done

# ----------------------------------------------------------
# This line needs to changed if you call FieldOpt differently
FIELDOPT_BIN=FieldOpt

CURRENT_DIR=$(pwd)
FIELDOPT_OUT=${CURRENT_DIR}/output

# Set the MODEL_DIR variable to where you have the 2D2W model files
MODEL_DIR=${CURRENT_DIR}/2D2WMODEL

# ==========================================================
printf '=%.0s' {1..100}; printf '\n' " "

# Setting case and path variables
CASE_IDX=$(expr $1 - 1)
CASE=${CASES[$CASE_IDX]}
DRIVER_PATH=${CURRENT_DIR}/fo_driver.CntrlOpt.${CASE}.json
OUTPUT_DIR=${FIELDOPT_OUT}/${CASE}

# ----------------------------------------------------------
DECK_PATH=${MODEL_DIR}/2D2WMODEL.DATA
GRID_PATH=${MODEL_DIR}/2D2WMODEL.EGRID


# ----------------------------------------------------------
# Delete ouput subdirectory
printf '%s\n' "DBG: rm -rfv ${OUTPUT_DIR}:" 
rm -rfv ${OUTPUT_DIR}

# ==========================================================
printf '=%.0s' {1..100}; printf '\n' " "

# Re-create ouput subdirectory
printf '%s\n' "DBG: mkdir -v ${OUTPUT_DIR}:" 
mkdir -p ${OUTPUT_DIR}

# Print paths
printf '%s\n' "FieldOpt: ${FIELDOPT_BIN}"
printf '%s\n' "Output: ${OUTPUT_DIR}"
printf '%s\n' "Driver: ${DRIVER_PATH}"
printf '%s\n' "Deck:   ${DECK_PATH}"
printf '%s\n' "Grid:   ${GRID_PATH}"

# ==========================================================
printf '=%.0s' {1..100}; printf '\n' " "

# ----------------------------------------------------------
# Make run command (MPI)
CMDMPI="mpirun -np 10 -mca btl ^openib ${FIELDOPT_BIN} "\
"${DRIVER_PATH} ${OUTPUT_DIR} -r mpisync -n1 -m3 "\
"-g ${GRID_PATH} -s ${DECK_PATH} "\
"-e bash_flw-bin.sh -f -v 3 -t 0 --sim-delay 2" 

# printf '%s\n' "CMDMPI=${CMDMPI}"

# ----------------------------------------------------------
# Make run command (SERIAL)
CMDSER="${FIELDOPT_BIN} "\
"${DRIVER_PATH} ${OUTPUT_DIR} -r serial "\
"-g ${GRID_PATH} -s ${DECK_PATH} "\
"-e bash_flw-bin.sh -f -v 3 -t 0"

# printf '%s\n' "CMDSER=${CMDSER}"

# ----------------------------------------------------------
# RUN IN PARALLEL MODE
# eval ${CMDMPI}

# RUN IN SERIAL MODE
eval ${CMDSER}
