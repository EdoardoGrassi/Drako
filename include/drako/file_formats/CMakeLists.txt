cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory("dson")
add_subdirectory("png")
add_subdirectory("wave")

add_library(bmp STATIC "bmp/bmp.cpp")
add_library(drako::bmp ALIAS bmp)

add_library(json STATIC "src/json.cpp")
add_library(drako::json ALIAS json)