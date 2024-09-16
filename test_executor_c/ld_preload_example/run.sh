#!/bin/bash
set -e
set -o pipefail

mkdir -p build
cd build
cmake ..
make

LD_PRELOAD=./liboverride.so ./uut
