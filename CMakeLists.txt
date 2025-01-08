    cmake_minimum_required(VERSION 3.5)

project(transport_catalogue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY output)
set(CMAKE_CXX_COMP)

add_executable (
    transport_catalogue
    src/main.cpp
    src/transport_catalogue.cpp
    src/input_reader.cpp
    src/stat_reader.cpp
)

target_compile_options(transport_catalogue PRIVATE -Wall -Wextra -g)
