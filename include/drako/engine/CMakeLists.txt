cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
enable_testing()

add_library(drako-runtime STATIC
    "src/runtime_asset_manager.cpp")
add_library(drako::runtime ALIAS drako-runtime)