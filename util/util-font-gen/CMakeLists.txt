cmake_minimum_required(VERSION 3.12...3.28)

project(DexprOS_UtilFontGen VERSION 1.0)


set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS OFF)


add_executable(${PROJECT_NAME} src/DexprBitmapFontGenerator.c)


find_package(Freetype REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE ${FREETYPE_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${FREETYPE_INCLUDE_DIRS})

