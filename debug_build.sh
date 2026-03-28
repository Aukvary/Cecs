#!/bin/bash

cd "$(dirname "$(readlink -f "$0")")"

TARGET=${1:-all}

if [ ! -d "cmake-build-debug" ]; then
    cmake -G Ninja -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=gcc
fi

cmake --build cmake-build-debug --target "$TARGET"