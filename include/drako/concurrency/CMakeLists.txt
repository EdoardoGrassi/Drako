cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS)

add_executable(sync-queue-test ${CMAKE_CURRENT_SOURCE_DIR}/test/concurrent_bounded_queue.cpp)
add_test(NAME test_1
    COMMAND sync-queue-test)
set_tests_properties(test_1 PROPERTIES TIMEOUT 10)