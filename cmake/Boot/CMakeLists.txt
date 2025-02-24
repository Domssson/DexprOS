cmake_minimum_required(VERSION 3.17...3.30)


set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)


project(DexprOS VERSION 1.0 LANGUAGES C ASM)


set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS OFF)



set(DEXPROSBOOT_EMBED_KERNEL_PATH "" CACHE FILEPATH "Path to the kernel to embed")

if (NOT DEXPROSBOOT_EMBED_KERNEL_PATH)
    message(FATAL_ERROR "No path to the kernel to embed is given!")
endif()

get_filename_component(DEXPROSBOOT_EMBED_KERNEL_PATH ${DEXPROSBOOT_EMBED_KERNEL_PATH} ABSOLUTE)



get_filename_component(DEXPROS_BASE_DIR "../.." ABSOLUTE)

set(DEXPROS_SRC_DIR "${DEXPROS_BASE_DIR}/src")
set(DEXPROS_INCLUDE_DIR "${DEXPROS_BASE_DIR}/include")


set(DEXPROS_KERNEL_SOURCES ${DEXPROS_SRC_DIR}/Boot/x86_64-efi/UefiBoot.c
                           ${DEXPROS_SRC_DIR}/Boot/x86_64-efi/EmbeddedKernel.S
                           ${DEXPROS_SRC_DIR}/Kernel/kstdlib/string.c)

set(DEXPROS_KERNEL_HEADERS ${DEXPROS_INCLUDE_DIR}/DexprOS/Boot/x86_64-efi/EmbeddedKernel.h
                           ${DEXPROS_INCLUDE_DIR}/DexprOS/Kernel/kstdlib/string.h)


set_property(SOURCE ${DEXPROS_SRC_DIR}/Boot/x86_64-efi/EmbeddedKernel.S
             APPEND PROPERTY OBJECT_DEPENDS ${DEXPROS_BASE_DIR}/build/Kernel/KERNEL)


add_executable(BOOTX64.EFI ${DEXPROS_KERNEL_SOURCES} ${DEXPROS_KERNEL_HEADERS})



if (CMAKE_C_COMPILER_ID MATCHES GNU) # MinGW

    target_compile_options(BOOTX64.EFI PRIVATE -ffreestanding -mno-red-zone -fshort-wchar -fno-stack-protector)
    target_link_options(BOOTX64.EFI PRIVATE -mno-red-zone -nostdlib -shared -Wl,-dll -Wl,--subsystem,10 -e efi_main)

elseif (CMAKE_C_COMPILER_ID MATCHES Clang)

    target_compile_options(BOOTX64.EFI PRIVATE -B"${DEXPROS_BASE_DIR}/toolchains/clang-x86_64-efi/bin" -ffreestanding -mno-red-zone -fshort-wchar -fno-stack-protector)
    target_link_options(BOOTX64.EFI PRIVATE -B"${DEXPROS_BASE_DIR}/toolchains/clang-x86_64-efi/bin" -fuse-ld=lld-link -mno-red-zone -nostdlib -Wl,-subsystem:efi_application -Wl,-entry:efi_main)

endif()



target_compile_definitions(BOOTX64.EFI PRIVATE DEXPROSBOOT_EMBED_KERNEL_PATH="${DEXPROSBOOT_EMBED_KERNEL_PATH}")


target_compile_options(BOOTX64.EFI PRIVATE -Wall -Wextra -Wpedantic)


target_include_directories(BOOTX64.EFI PRIVATE ${DEXPROS_INCLUDE_DIR}
                                               "${DEXPROS_BASE_DIR}/third-party/gnu-efi-code/inc")


