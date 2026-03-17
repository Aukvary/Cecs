@echo off
set "TARGET=%~1"

if "%TARGET%"=="" set "TARGET=all"

if not exist cmake-build-debug (
    cmake -G Ninja -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=gcc
)
cmake --build cmake-build-debug --target %TARGET%

@REM if exist cmake-build-debug rmdir /s /q cmake-build-debug