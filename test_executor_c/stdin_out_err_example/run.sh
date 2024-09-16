#!/bin/bash
set -e
set -o pipefail

mkdir -p build
cd build
cmake ..
make

# stdbuf используется для того, чтобы stdout (/stdin/stderr) был не буферизованным
stdbuf -o0 ./uut_runner &
CHILD_PID=$!
echo $CHILD_PID
sleep 1

kill -SIGCONT $CHILD_PID
echo "66" > /proc/$CHILD_PID/fd/0
echo "77" > /proc/$CHILD_PID/fd/0
echo "88" > /proc/$CHILD_PID/fd/0
echo "99" > /proc/$CHILD_PID/fd/0

# Ждем завершения
while kill -0 $CHILD_PID; [ $? -eq 0 ]; do
    sleep 1
done

echo "stdout.txt:"
cat ./stdout.txt
echo "stderr.txt:"
cat ./stderr.txt
