cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

find_package(GTest)

add_library(dson STATIC
    "src/dson.cpp"
    "src/dson_lexer.cpp")
add_library(drako::dson ALIAS dson)

add_executable(dson_lexer_test "test/dson_lexer_test.cpp")
target_link_libraries(dson_lexer_test PRIVATE drako::dson gtest gtest_main)
gtest_discover_tests(dson_lexer_test)

add_executable(dson_parser_test "test/dson_parser_test.cpp")
target_link_libraries(dson_parser_test PRIVATE drako::dson gtest gtest_main)
gtest_discover_tests(dson_parser_test)