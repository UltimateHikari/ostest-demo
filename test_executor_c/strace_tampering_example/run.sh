#!/bin/bash
set -e
set -o pipefail

mkdir -p build
cd build
cmake ..
make

./uut &

# Важно: меняется возврат именно системного вызова, а возврат враппера может быть другим
# Также ставится ошибка именно системного вызова, а не враппера

# Ошибка с кодом 2 без выполнения реального системного вызова для каждого системного вызова clock_nanosleep
strace -f -p $! --inject=clock_nanosleep:error=2 >/dev/null 2>/dev/null

# Возврат 5 без выполнения реального системного вызова для каждого системного вызова clock_nanosleep
#strace -f -p $! --inject=clock_nanosleep:retval=5 >/dev/null 2>/dev/null

# ---

# Работает только на 3-й
#strace -f -p $! --inject=clock_nanosleep:error=2:when=3 >/dev/null 2>/dev/null

# Работает только на 2-й, 2+3=5-й, 5+3=8-й, 8+3=11-й, ...
#strace -f -p $! --inject=clock_nanosleep:error=2:when=2+3 >/dev/null 2>/dev/null

# Работает только на 2-й, 2+3=5-й, 5+3=8-й (до 8-го)
#strace -f -p $! --inject=clock_nanosleep:error=2:when=2..8+3 >/dev/null 2>/dev/null
