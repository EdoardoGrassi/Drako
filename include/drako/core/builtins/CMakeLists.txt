cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_executable(intrinsics_test EXCLUDE_FROM_ALL
    "test\intrinsics_test.cpp")

add_test(NAME drako_intrinsics COMMAND intrinsics_test)