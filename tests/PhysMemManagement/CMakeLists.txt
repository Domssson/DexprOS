cmake_minimum_required(VERSION 3.12...3.30)

project(DexprOSPhysMemManagementTest VERSION 1.0 LANGUAGES C)


set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS OFF)


set(TEST_SOURCE_FILES main.c InitialMemMapGenEfi_manual_alloc.c)


set(OS_SOURCE_FILES ../../src/Kernel/Memory/PhysicalMemMap.c
                    ../../src/Kernel/Memory/PhysicalMemTree.c
                    ../../src/Kernel/Memory/PhysicalMemMapGen.c
                    ../../src/Kernel/Memory/PhysicalMemStructsGen.c
                    ../../src/Kernel/Memory/PhysicalMemTreeGen.c
                    ../../src/Kernel/Memory/InitialMemMap.c)


add_executable(${PROJECT_NAME} ${OS_SOURCE_FILES} ${TEST_SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ../../include
                                                   ../../third-party/gnu-efi-code/inc)


