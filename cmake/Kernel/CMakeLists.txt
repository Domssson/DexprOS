cmake_minimum_required(VERSION 3.17...3.30)


set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)


project(DexprOS VERSION 1.0 LANGUAGES C ASM)


set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS OFF)


get_filename_component(DEXPROS_BASE_DIR "../.." ABSOLUTE)

set(DEXPROS_SRC_DIR "${DEXPROS_BASE_DIR}/src")
set(DEXPROS_INCLUDE_DIR "${DEXPROS_BASE_DIR}/include")


set(DEXPROS_KERNEL_SOURCES ${DEXPROS_SRC_DIR}/Kernel/kmain.c
                           ${DEXPROS_SRC_DIR}/Kernel/kstdlib/string.c)

set(DEXPROS_KERNEL_HEADERS ${DEXPROS_INCLUDE_DIR}/DexprOS/Kernel/kstdlib/string.h)


add_executable(KERNEL ${DEXPROS_KERNEL_SOURCES} ${DEXPROS_KERNEL_HEADERS})



if (CMAKE_C_COMPILER_ID MATCHES GNU)

    target_compile_options(KERNEL PRIVATE -ffreestanding -mno-red-zone -fno-stack-protector)
    target_link_options(KERNEL PRIVATE -mno-red-zone -nostdlib -lgcc -static -e kmain)

elseif (CMAKE_C_COMPILER_ID MATCHES Clang)

    target_compile_options(KERNEL PRIVATE -B"${DEXPROS_BASE_DIR}/toolchains/clang-x86_64-kernel-elf/bin" -ffreestanding -mno-red-zone -fno-stack-protector)
    target_link_options(KERNEL PRIVATE -B"${DEXPROS_BASE_DIR}/toolchains/clang-x86_64-kernel-elf/bin" -fuse-ld=lld -mno-red-zone -nostdlib -static -e kmain)

endif()


target_compile_options(KERNEL PRIVATE -Wall -Wextra -Wpedantic)


target_include_directories(KERNEL PRIVATE ${DEXPROS_INCLUDE_DIR}
                                          "${DEXPROS_BASE_DIR}/third-party/gnu-efi-code/inc")


