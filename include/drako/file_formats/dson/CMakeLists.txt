cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

add_library(dson STATIC
    "src/dson.cpp"
    "src/dson_lexer.cpp")
add_library(drako::dson ALIAS dson)


find_package(GTest)
add_executable(dson-tests
    "test/dson_lexer_test.cpp"
    "test/dson_parser_test.cpp"
)
target_link_libraries(dson-tests PRIVATE drako::dson gtest_main)
gtest_discover_tests(dson-tests)