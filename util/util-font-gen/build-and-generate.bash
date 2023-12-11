#!/bin/bash

mkdir -p build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

cd ..

./build/DexprOS_UtilFontGen


cp font256.h ../../include/DexprOS/font256.h

