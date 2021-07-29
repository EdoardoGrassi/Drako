cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

option(SYS_BUILD_TESTS "Build unit tests" ON)

add_library(drako-sys STATIC)
add_library(drako::sys ALIAS drako-sys)

target_compile_features(drako-sys PUBLIC cxx_std_17)

target_sources(drako-sys PUBLIC
    "dynamic_library.hpp"
    "src/file_system_watcher.cpp"
)

if (WIN32)
    target_sources(drako-sys PRIVATE
        "src/desktop_window_win32.cpp"
        "src/keyboard_device_win32.cpp"
        #"src/hid_api_win32.cpp"
        "src/system_info_win32.cpp")

    target_compile_definitions(drako-sys PRIVATE
        "UNICODE"
        "WIN32_LEAN_AND_MEAN"
        "NOMINMAX"
        "STRICT")

    #target_link_libraries(drako-sys PUBLIC Kernel32)
endif()

#add_executable(drako-sys-keyboard-app "./test/keyboard_app_001.cpp")
#target_link_libraries(drako-sys-keyboard-app PRIVATE drako::sys)


if (SYS_BUILD_TESTS)
    enable_testing()
    add_subdirectory("test")
endif()