cmake_minimum_required(VERSION 3.16)

project(transport_catalogue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Werror -Werror=maybe-uninitialized)

add_executable(
    transport_catalogue

    main.cpp
    domain.cpp
    geo.cpp
    json.cpp
    json_reader.cpp
    map_renderer.cpp
    request_handler.cpp
    svg.cpp
    json_builder.cpp
    transport_catalogue.cpp
)
