cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

if(WIN32)
    add_library(drako-audio STATIC
        "src/audio_input_device_win32.cpp"
        "src/audio_output_device_win32.cpp"
        "src/audio_engine_win32.cpp")

    target_compile_definitions(drako-audio PRIVATE
        "UNICODE"
        "WIN32_LEAN_AND_MEAN"
        "NOMINMAX"
        "STRICT")
endif()

add_library(drako::audio ALIAS drako-audio)

#add_executable(test_io_1 "test/input_output_test.cpp")
#add_test(NAME test1 COMMAND test_io_1)

add_executable(${PROJECT_NAME}_audio_test "test/main_audio_test.cpp")
target_link_libraries(${PROJECT_NAME}_audio_test drako::audio)
