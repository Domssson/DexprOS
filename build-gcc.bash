#!/bin/bash

mkdir -p build/Kernel

cd build/Kernel


export PATH="$HOME/.cross-compilers/x86_64-unknown-elf-gcc/install/bin:$PATH"

cmake ../../cmake/Kernel/CMakeLists.txt -DCMAKE_TOOLCHAIN_FILE=../../toolchains/gcc-x86_64-kernel-elf/x86_64-elf-gcc-generic.cmake -G Ninja -DCMAKE_BUILD_TYPE=Release

cmake --build .


cd ..

mkdir -p Boot

cd Boot


cmake ../../cmake/Boot/CMakeLists.txt -DCMAKE_TOOLCHAIN_FILE=../../toolchains/mingw-x86_64-efi/x86_64-w64-mingw32-generic.cmake -DDEXPROSBOOT_EMBED_KERNEL_PATH=../../build/Kernel/KERNEL -G Ninja -DCMAKE_BUILD_TYPE=Release

cmake --build .


cd ../..


./generate-img.bash

