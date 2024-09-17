#!/bin/bash
set -e
set -o pipefail

mkdir -p build
cd build
cmake ..
make

# Выключение системного вызова
./uut_runner &
./ptrace_disable_syscall $!
