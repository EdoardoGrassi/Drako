cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

option(DRAKO_LOCKFREE_BUILD_TESTS "Build unit tests" ON)

add_library(lockfree INTERFACE) # header only library
add_library(drako::lockfree ALIAS lockfree)

target_compile_features(lockfree INTERFACE cxx_std_17)
target_include_directories(lockfree INTERFACE .)

target_sources(lockfree
    PUBLIC
        "lockfree_ringbuffer.hpp"
        "lockfree_mrmw_queue.hpp"
)

if (DRAKO_LOCKFREE_BUILD_TESTS)
    enable_testing()
    add_subdirectory("test")
endif()