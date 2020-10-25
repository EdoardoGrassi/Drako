cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# target_sources(${PROJECT_NAME}_vulkan_app_win PRIVATE "src/native_window_win.cpp")
# target_include_directories(${PROJECT_NAME}_vulkan_app_win PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_library(drako-io STATIC
    "src/inout_file_stream.cpp"
    "src/input_file_stream.cpp"
    "src/output_file_stream.cpp"
    "src/aio_file_handle.cpp"
    "src/vio_file_handle.cpp")

if(WIN32)

    target_compile_definitions(drako-io PRIVATE
        "UNICODE"
        "WIN32_LEAN_AND_MEAN"
        "NOMINMAX"
        "STRICT")
endif()

add_library(drako::io ALIAS drako-io)

#set_target_properties(native_window_api native_keyboard_api PROPERTIES CXX_STANDARD 17)

#add_executable(drako-hid-test WIN32 "./test/hid_app_test.cpp")
#target_link_libraries(drako-hid-test PRIVATE drako::native-sys)