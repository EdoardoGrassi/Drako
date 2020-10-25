cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

add_library(drako-obj STATIC
    "src/parser.cpp"
    "src/object.cpp")
add_library(drako::obj ALIAS drako-obj)