cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory("png")
add_subdirectory("wave")
add_subdirectory("wavefront")

add_library(drako-bmp STATIC "bmp/bmp.cpp")
add_library(drako::bmp ALIAS drako-bmp)

add_library(drako-json STATIC "src/json.cpp")
add_library(drako::json ALIAS drako-json)

add_library(drako-drakoson STATIC "src/dson.cpp")
add_library(drako::drakoson ALIAS drako-drakoson)