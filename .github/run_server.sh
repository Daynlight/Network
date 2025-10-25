#!/bin/bash

# Get the absolute path to the script
SCRIPT_DIR=$(dirname "$(realpath "$0")")

# Run the commands with the full path to the binaries
setsid konsole -e "${SCRIPT_DIR}/../bin/Server" &