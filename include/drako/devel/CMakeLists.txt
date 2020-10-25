cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
enable_testing()

add_library(drako-devel STATIC
    "./src/build_utils.cpp"
    "./src/project_types.cpp"
    "./src/project_utils.cpp"
    "./src/uuid.cpp"
    "./src/mesh_importers.cpp")
add_library(drako::devel ALIAS drako-devel)

# vvv test executables vvv
add_executable(uuid-test1-exe "./test/uuid_test1.cpp")
target_link_libraries(uuid-test1-exe PRIVATE drako::devel)
add_test(NAME "uuid-test1" COMMAND uuid-test1-exe)

add_executable(build-utils-test1-exe "./test/build_utils_test1.cpp")
target_link_libraries(build-utils-test1-exe PRIVATE drako::devel)
add_test(NAME "build-utils-test1" COMMAND build-utils-test1-exe)