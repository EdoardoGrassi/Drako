cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include_directories(${PROJECT_SOURCE_DIR}/include)

#add_subdirectory("dson")
add_subdirectory("png")
add_subdirectory("wave")
add_subdirectory("wavefront")

add_library(bmp STATIC "bmp/bmp.cpp")
add_library(drako::bmp ALIAS bmp)

add_library(json STATIC "src/json.cpp")
add_library(drako::json ALIAS json)

add_library(dson STATIC "dson/src/dson.cpp" "dson/src/dson_lexer.cpp")
target_link_libraries(dson PRIVATE drako::io)
add_library(drako::dson ALIAS dson)

add_executable(dson_lexer_test "dson/test/dson_lexer_test.cpp")
target_link_libraries(dson_lexer_test PRIVATE drako::dson)

add_executable(dson_parser_test "dson/test/dson_parser_test.cpp")
target_link_libraries(dson_parser_test PRIVATE drako::dson)