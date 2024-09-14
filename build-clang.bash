#!/bin/bash

mkdir -p build

cd build

cmake .. -DCMAKE_TOOLCHAIN_FILE="../cmake/cross-compile-toolchains/x86_64-uefi-clang-generic.cmake" -DCMAKE_BUILD_TYPE=Release
cmake --build .

cd ..


./generate-img.bash

