cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

add_library(drako-io STATIC
    "src/input_output_file_stream.cpp"
    "src/input_file_stream.cpp"
    "src/output_file_stream.cpp"
    "src/aio_file_handle.cpp"
    "src/vio_file_handle.cpp" "test/input_output_test.cpp")

if(WIN32)
    target_compile_definitions(drako-io PRIVATE
        "UNICODE"
        "WIN32_LEAN_AND_MEAN"
        "NOMINMAX"
        "STRICT")
endif()

add_library(drako::io ALIAS drako-io)

add_executable(test_io_1 "test/input_output_test.cpp")
add_test(NAME test1 COMMAND test_io_1)

#add_executable(drako-hid-test WIN32 "./test/hid_app_test.cpp")
#target_link_libraries(drako-hid-test PRIVATE drako::native-sys)