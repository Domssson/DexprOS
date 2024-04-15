cmake_minimum_required(VERSION 3.12...3.28)

project(DexprOSKernelStringHTest VERSION 1.0 LANGUAGES C)


set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS OFF)


add_executable(${PROJECT_NAME} KernelStringHTest.c)


