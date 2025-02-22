#!/bin/bash

mkdir -p build

cd build

cmake .. -DCMAKE_TOOLCHAIN_FILE="../cmake/cross-compile-toolchains/x86_64-w64-mingw32-generic.cmake" -DCMAKE_BUILD_TYPE=Debug
cmake --build .

cd ..


./generate-img.bash

