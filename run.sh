#!/usr/bin/env bash
if [ -d "build" ]; then
    cd build
    cmake .. && cmake --build . && ./NES $1
else
    echo "please run ./setup.sh first"
    return 1 2>/dev/null
    exit 1
fi