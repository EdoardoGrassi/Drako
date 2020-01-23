cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(drako_obj STATIC "src/wavefront_parser.cpp")
add_library(drako::obj ALIAS drako_obj)

add_library(drako_bmp STATIC "src/bmp_format_parser.cpp")
add_library(drako::bmp ALIAS drako_bmp)

add_library(drako_json STATIC "src/json.cpp")
add_library(drako::json ALIAS drako_json)