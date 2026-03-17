#!/bin/bash

TARGET="${1:NULL}"

cmake -G Ninja -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=gcc
cmake --build cmake-build-debug --target "$TARGET"