cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
enable_testing()


include(FetchContent)

FetchContent_Declare(
    obj-cpp
    GIT_REPOSITORY https://github.com/EdoardoGrassi/obj-cpp.git
)
set(OBJCPP_BUILD_TESTS OFF)

FetchContent_Declare(
    uuid-cpp
    GIT_REPOSITORY https://github.com/EdoardoGrassi/uuid-cpp.git
)
set(UUID_BUILD_TESTS OFF)

FetchContent_MakeAvailable(obj-cpp uuid-cpp)

add_library(drako-devel STATIC
    "src/crc.cpp"
    "src/project_types.cpp"
    "src/project_utils.cpp"
    "src/mesh_importers.cpp")

target_link_libraries(drako-devel
    PRIVATE drako::dson obj-cpp uuid-cpp yaml-cpp
)
add_library(drako::devel ALIAS drako-devel)

# vvv test executables vvv
#find_package(GTest)

add_executable(crc32_test "test/crc32_tests.cpp")
target_link_libraries(crc32_test PRIVATE drako::devel gtest_main)
#gtest_discover_tests(crc32_test)

add_executable(project_utils_test "test/project_utils_test.cpp")
target_link_libraries(project_utils_test PRIVATE drako::devel gtest_main)
#gtest_discover_tests(project_utils_test)

add_executable(asset_tests "test/asset_tests.cpp")
target_link_libraries(asset_tests PRIVATE drako::devel gtest_main)
#gtest_discover_tests(asset_tests)