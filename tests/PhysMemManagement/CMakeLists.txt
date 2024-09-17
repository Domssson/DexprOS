cmake_minimum_required(VERSION 3.12...3.30)

project(DexprOSPhysMemManagementTest VERSION 1.0 LANGUAGES C)


set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS OFF)


set(TEST_FILES main.c)


set(OS_SOURCE_FILES ../../src/Kernel/Memory/PhysicalMemMap.c
                    ../../src/Kernel/Memory/PhysicalMemTree.c
                    ../../src/Kernel/efi/EfiMemMapIteration.c
                    ../../src/Kernel/efi/PhysicalMemMapEfi.c
                    ../../src/Kernel/efi/PhysicalMemStructsGenEfi.c
                    ../../src/Kernel/efi/PhysicalMemTreeGenEfi.c)


add_executable(${PROJECT_NAME} ${OS_SOURCE_FILES} ${TEST_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ../../include
                                                   ../../third-party/gnu-efi-code/inc)


