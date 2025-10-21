cmake_minimum_required (VERSION 3.10)

project ("FiguraSDK")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/examples/out")

add_subdirectory("lib")
add_subdirectory("examples")