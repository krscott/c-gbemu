#!/usr/bin/env bash

# exit when any command fails
set -e
trap 'echo ''; echo Error at $(basename "$0"):${LINENO}: $BASH_COMMAND' ERR

# Get this script's directory
SDIR="$(dirname "$(realpath "${BASH_SOURCE[0]})")")"

cd "$SDIR"

make

./test-c-gbemu
