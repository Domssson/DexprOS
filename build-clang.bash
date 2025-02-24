#!/bin/bash


mkdir -p build/Kernel

cd build/Kernel


cmake ../../cmake/Kernel/CMakeLists.txt -DCMAKE_TOOLCHAIN_FILE=../../toolchains/clang-x86_64-kernel-elf/x86_64-elf-clang-generic.cmake -G Ninja -DCMAKE_BUILD_TYPE=Release

cmake --build .


cd ..
mkdir -p Boot
cd Boot


cmake ../../cmake/Boot/CMakeLists.txt -DCMAKE_TOOLCHAIN_FILE=../../toolchains/clang-x86_64-efi/x86_64-uefi-clang-generic.cmake -DDEXPROSBOOT_EMBED_KERNEL_PATH=../../build/Kernel/KERNEL -G Ninja -DCMAKE_BUILD_TYPE=Release

cmake --build .


cd ../..


./generate-img.bash

