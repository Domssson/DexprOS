#!/bin/bash

mkdir -p build

cd build

env CC=clang CXX=clang++ cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

cd ..


./generate-img.bash

