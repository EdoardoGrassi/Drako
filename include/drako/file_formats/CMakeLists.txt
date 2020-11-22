cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory("png")
add_subdirectory("wave")
add_subdirectory("wavefront")

add_library(bmp STATIC "bmp/bmp.cpp")
add_library(drako::bmp ALIAS bmp)

add_library(json STATIC "src/json.cpp")
add_library(drako::json ALIAS json)

add_library(dson STATIC "src/dson.cpp")
add_library(drako::dson ALIAS dson)

add_executable(dson-test "test/dson_test.cpp")
target_link_libraries(dson-test PRIVATE drako::dson)