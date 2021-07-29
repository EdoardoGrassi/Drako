cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
enable_testing()


include(FetchContent)

FetchContent_Declare(
    obj-cpp
    GIT_REPOSITORY https://github.com/EdoardoGrassi/obj-cpp.git
)
set(OBJCPP_BUILD_TESTS OFF CACHE BOOL "exclude tests")

FetchContent_Declare(
    uuid-cpp
    GIT_REPOSITORY https://github.com/EdoardoGrassi/uuid-cpp.git
)
set(UUID_CPP_BUILD_TESTS OFF CACHE BOOL "exclude tests")

FetchContent_MakeAvailable(obj-cpp uuid-cpp)

add_library(drako-devel STATIC
    "src/crc.cpp"
    "src/asset_bundle_types.cpp"
    "src/editor_system.cpp"
    "src/project_types.cpp"
    "src/project_utils.cpp"
    "src/mesh_importers.cpp"
)

target_link_libraries(drako-devel
    PUBLIC obj-cpp uuid-cpp yaml-cpp
)
add_library(drako::devel ALIAS drako-devel)

# vvv test executables vvv

# get GoogleTest suite
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.10.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(gtest_build_gmock OFF)
FetchContent_MakeAvailable(googletest)

add_executable(drako-devel-tests
    "test/asset_bundle_types_tests.cpp"
    "test/crc32_tests.cpp"
    "test/project_utils_tests.cpp"
    "test/project_types_tests.cpp"
    "test/version_tests.cpp"
)
target_link_libraries(drako-devel-tests PRIVATE drako::devel gtest_main)

include(GoogleTest)
gtest_discover_tests(drako-devel-tests)