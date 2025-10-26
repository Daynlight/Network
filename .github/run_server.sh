#!/bin/bash

# Get the absolute path to the script
SCRIPT_DIR=$(dirname "$(realpath "$0")")

cd ${SCRIPT_DIR}
cd ..

# rebuild
git pull -f
git branch -m origin/release
git pull -f

rm -rf build/
mkdir build/
cd build/

cmake -DCMAKE_BUILD_TYPE=Release ..
make -j"$(nproc)"

cd ..

# Run the commands with the full path to the binaries
bash -c "bin/Server"