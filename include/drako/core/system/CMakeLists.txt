cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# target_sources(${PROJECT_NAME}_vulkan_app_win PRIVATE "src/native_window_win.cpp")
# target_include_directories(${PROJECT_NAME}_vulkan_app_win PUBLIC ${CMAKE_CURRENT_LIST_DIR})

if(WIN32)
    add_library(native_sys_api STATIC
        "src/native_io_api_win32.cpp"
        "src/native_io_file_win32.cpp"
        "src/native_window_win32.cpp"
        "src/native_keyboard_device_win32.cpp"
        "src/native_mouse_device_win32.cpp"
        "src/hid_device_win32.cpp"
        "src/system_info_win32.cpp")

    target_compile_definitions(native_sys_api PRIVATE
        "UNICODE"
        "WIN32_LEAN_AND_MEAN"
        "NOMINMAX"
        "STRICT")

    add_library(drako::native_sys ALIAS native_sys_api)
endif()

#set_target_properties(native_window_api native_keyboard_api PROPERTIES CXX_STANDARD 17)