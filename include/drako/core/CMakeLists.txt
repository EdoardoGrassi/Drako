cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
enable_testing()

# configure api header with project metadata
configure_file("drako_api_defs.hpp" "drako_api_defs.hpp")

add_executable(drako-intrinsics-test EXCLUDE_FROM_ALL
    "test/intrinsics_test.cpp" "container/soa.hpp" "container/fixed_vector.hpp")

add_test(NAME intrinsics-test COMMAND drako-intrinsics-test)