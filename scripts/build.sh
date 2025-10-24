#!/bin/bash
set -e
mkdir -p build && cd build
cmake ..
cmake --build . -j
cd ..
echo "Build xong."
echo "Run demo bằng lệnh: ./scripts/run_demo.sh"
./scripts/run_demo.sh