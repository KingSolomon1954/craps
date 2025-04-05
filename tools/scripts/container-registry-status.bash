#!/usr/bin/env bash
#
# File: container-registry-status.bash
#
# ---------------------------------------------------------------------

CNTR_TECH=$1
CNTR_REGISTRY=$2

if [ -z ${CNTR_REGISTRY} ]; then
    echo "Missing argument 2, CNTR_REGISTRY must be supplied"
    return 1   # return false
fi

# Where to find lib-container-registry script.
# Will be co-located with this script.
tmp1=${0%/}         # grab directory path of this script
dirName=${tmp1%/*}  # remove last level in path

source ${dirName}/lib-container-registry.bash

if cntrIsLoggedIn ${CNTR_TECH} ${CNTR_REGISTRY}; then
    echo "(${CNTR_TECH}) Currently logged in to ${CNTR_REGISTRY}"
else
    echo "(${CNTR_TECH}) Currently not logged in to ${CNTR_REGISTRY}"
fi
