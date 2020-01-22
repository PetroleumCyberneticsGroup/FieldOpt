#!/bin/bash

# --------------------------------------------------------------------
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

# --------------------------------------------------------------------
clear
printf '=%.0s' {1..100}; printf '\n' " "

declare -a CASES=(
	# orig jsons: 1-5
    "fo_driver.controls.cs"              # 1
    "fo_driver.curved_wells.controls.cs" # 2
    "fo_driver.control.npv.apps"         # 3
    "fo_driver.control.npv.cs"           # 4
    "fo_driver.control.rate.npv.apps"    # 5
    "fo_driver.compartments.npv.apps"    # 6
    # dbg jsons:
    "fo_driver.controls.cs.dbg"              #  7 [SER=OK] [MPI=OK]
    "fo_driver.curved_wells.controls.cs.dbg" #  8 [SER=OK] [MPI=OK]
    "fo_driver.control.npv.apps.dbg"         #  9 [SER=OK] [MPI=OK]
    "fo_driver.control.npv.cs.dbg"           # 10 [SER=OK] [MPI=OK]
    "fo_driver.control.rate.npv.apps.dbg"    # 11 [SER=OK] [MPI=OK]
    "fo_driver.compartments.npv.apps.dbg"    # 12 [SER=OK] [MPI=OK]
    #
    "fo_driver.wplc.const-2w.AAA.wsum.cs.aaa.dbg" # 13 no.const
    "fo_driver.wplc.const-2w.AAA.wsum.cs.bbb.dbg" # 14 simple.const
    "fo_driver.wplc.const-2w.AAA.wsum.cs.sln.dbg" # 15
)

# --------------------------------------------------------------------
for (( i=1; i<${#CASES[@]}+1; i++ ))
do
printf 'Case %02.0f: %s\n' ${i} "${CASES[i-1]}"
done

# --------------------------------------------------------------------
FODIRTLS="${HOME}/projects/PetroleumCyberneticsGroup/FieldOpt-Research"
FODIRMB="${HOME}/git/PCG/FieldOpt"
FODIR=${FODIRMB}

FIELDOPT_BUILD=${FODIR}/FieldOpt/cmake-build-debug/bin
FIELDOPT_BIN=${FODIR}/FieldOpt/cmake-build-debug/bin/FieldOpt
FIELDOPT_OUT=${FODIR}/FieldOpt-output

# --------------------------------------------------------------------
printf '=%.0s' {1..100}; printf '\n' " "

# Set case and path variables
CASE_IDX=$(expr $1 - 1)
CASE=${CASES[$CASE_IDX]}

CURRENT_DIR=$(pwd)
DRIVER_PATH=${CURRENT_DIR}/${CASE}.json
OUTPUT_DIR=${FIELDOPT_OUT}/Brugge/${CASE}
DECK_PATH=${CURRENT_DIR}/BRG_BLT1_DFO.DATA
GRID_PATH=${CURRENT_DIR}/BRG_BLT1_DFO.EGRID

# -------------------------------------------------------------------- ***
# Choose simulator
# SCR_PATH=${FIELDOPT_BUILD}/execution_scripts/bash_ecl.sh
SCR_PATH=${FIELDOPT_BUILD}/execution_scripts/bash_flow.sh

# --------------------------------------------------------------------
# Delete ouput subdirectory
printf '%s\n' "DBG: rm -rfv ${OUTPUT_DIR}:"
rm -rfv ${OUTPUT_DIR}

# --------------------------------------------------------------------
printf '=%.0s' {1..100}; printf '\n' " "

# Re-create ouput subdirectory
printf '%s\n' "DBG: mkdir -v ${OUTPUT_DIR}:"
mkdir -vp ${OUTPUT_DIR}

# --------------------------------------------------------------------
printf '=%.0s' {1..100}; printf '\n%s\n' "Paths:"

# Print paths
printf '%s\n' "Output: ${OUTPUT_DIR}"
printf '%s\n' "Driver: ${DRIVER_PATH}"
printf '%s\n' "Deck:   ${DECK_PATH}"
printf '%s\n' "Grid:   ${GRID_PATH}"

# --------------------------------------------------------------------
printf '=%.0s' {1..100}; printf '\n' " "

# Set LD_LIBRARY_PATH
LDPATH="LD_LIBRARY_PATH=\
~/git/PCG/FieldOpt/FieldOpt/cmake-build-debug/lib"

# Make run command (MPI)
export ${LDPATH}
CMDMPI="mpirun -np 3 -mca btl ^openib ${FIELDOPT_BIN} "\
"${DRIVER_PATH} ${OUTPUT_DIR} -r mpisync -n1 -m3 "\
"-g ${GRID_PATH} -s ${DECK_PATH} "\
"-e ${SCR_PATH} -f -v 3 -t 0"

printf '%s\n' "CMDMPI=${CMDMPI}"

# --------------------------------------------------------------------
printf '=%.0s' {1..100}; printf '\n' " "

# Make run command (SERIAL)
CMDSER="${FIELDOPT_BIN} "\
"${DRIVER_PATH} ${OUTPUT_DIR} -r serial "\
"-g ${GRID_PATH} -s ${DECK_PATH} "\
"-e ${SCR_PATH} -f -v 3 -t 0"

printf '%s\n' "CMDSER=${CMDSER}"

# --------------------------------------------------------------------
printf '=%.0s' {1..100}; printf '\n' " "
eval ${CMDSER}
# eval ${CMDMPI}