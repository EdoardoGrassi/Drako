cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(drako-obj STATIC "src/wavefront_obj.cpp")
add_library(drako::obj ALIAS drako-obj)

add_library(drako-bmp STATIC "src/bmp.cpp")
add_library(drako::bmp ALIAS drako-bmp)

add_library(drako-json STATIC "src/json.cpp")
add_library(drako::json ALIAS drako-json)