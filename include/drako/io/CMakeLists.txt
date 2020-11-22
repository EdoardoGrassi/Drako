cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

add_library(dk-io STATIC    
    "src/aio_file_handle.cpp"
    "src/vio_file_handle.cpp"
    "test/input_output_test.cpp")

if(WIN32)
    target_sources(dk-io PRIVATE "src/core_win32.cpp")
    target_compile_definitions(dk-io PRIVATE
        "UNICODE"
        "WIN32_LEAN_AND_MEAN"
        "NOMINMAX"
        "STRICT")
endif()

add_library(drako::io ALIAS dk-io)

add_executable(dk-io-test_1 "test/input_output_test.cpp")
target_link_libraries(dk-io-test_1 PRIVATE drako::io)
add_test(NAME io-test_1 COMMAND dk-io-test_1)

add_executable(dk-io-test_2 "test/reader_pool_test.cpp")
target_link_libraries(dk-io-test_2 PRIVATE drako::io)
add_test(NAME io-test_2 COMMAND dk-io-test_2)

#add_executable(drako-hid-test WIN32 "./test/hid_app_test.cpp")
#target_link_libraries(drako-hid-test PRIVATE drako::native-sys)