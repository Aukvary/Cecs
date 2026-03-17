#!/bin/bash

TARGET="${1:-all}"

cmake -G Ninja -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=gcc
cmake --build cmake-build-debug --target "$TARGET"

if [ -d "cmake-build-debug" ]; then
    rm -rf cmake-build-debug
fi