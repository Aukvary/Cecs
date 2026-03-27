@echo off

pushd "%~dp0"

set "TARGET=%~1"
if "%TARGET%"=="" set "TARGET=all"

if not exist cmake-build-debug (
    cmake -G Ninja -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=gcc
)
cmake --build cmake-build-debug --target %TARGET%

popd