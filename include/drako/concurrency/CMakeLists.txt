cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS)

add_executable(test1 ${CMAKE_CURRENT_SOURCE_DIR}/test/concurrent_bounded_queue.cpp)
add_test(NAME test_1
    COMMAND test1)
set_tests_properties(test_1 PROPERTIES TIMEOUT 10)