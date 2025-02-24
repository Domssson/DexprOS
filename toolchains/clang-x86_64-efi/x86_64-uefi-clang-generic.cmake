SET(CMAKE_SYSTEM_NAME    Generic)
SET(CMAKE_SYSTEM_VERSION 1)

SET(CMAKE_C_COMPILER     "${CMAKE_CURRENT_LIST_DIR}/bin/x86_64-efi-clang")
SET(CMAKE_ASM_COMPILER   "${CMAKE_CURRENT_LIST_DIR}/bin/x86_64-efi-clang")
SET(CMAKE_AR llvm-ar)

SET(CMAKE_FIND_ROOT_PATH "${PROJECT_SOURCE_DIR}")
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
