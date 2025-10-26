#!/bin/bash

# Exit immediately if a command fails
set -e
set -o pipefail

# Get the absolute path to this script
SCRIPT_DIR=$(dirname "$(realpath "$0")")
cd "${SCRIPT_DIR}/.."

# Name of your systemd service
SERVICE_NAME="chat.service"

# File to store the last known commit hash
LAST_COMMIT_FILE=".last_commit"

# Fetch the latest commits
git fetch origin release

# Get the current and remote commit hashes
CURRENT_COMMIT=$(git rev-parse HEAD)
REMOTE_COMMIT=$(git rev-parse origin/release)

# If no record exists yet, create one
if [ ! -f "$LAST_COMMIT_FILE" ]; then
  echo "$CURRENT_COMMIT" > "$LAST_COMMIT_FILE"
  echo "[INFO] Created initial commit record: $CURRENT_COMMIT"
  exit 0
fi

# Load the last known commit
LAST_COMMIT=$(cat "$LAST_COMMIT_FILE")

# Compare hashes
if [ "$REMOTE_COMMIT" != "$LAST_COMMIT" ]; then
  git checkout release
  git pull origin release

  rm -rf build
  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release ..
  make -j"$(nproc)"
  cd ..

  echo "[INFO] Restarting systemd service: $SERVICE_NAME"
  sudo systemctl restart "$SERVICE_NAME"

  # Update stored commit hash
  echo "$REMOTE_COMMIT" > "$LAST_COMMIT_FILE"

  echo "[INFO] Update complete."
else
  echo "[INFO] No changes detected. Service is up to date."
fi
