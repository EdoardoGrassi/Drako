cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# target_sources(${PROJECT_NAME}_vulkan_app_win PRIVATE "src/native_window_win.cpp")
# target_include_directories(${PROJECT_NAME}_vulkan_app_win PUBLIC ${CMAKE_CURRENT_LIST_DIR})

if(WIN32)
    add_library(drako-system STATIC
        "src/desktop_window_win32.cpp"
        "src/keyboard_device_win32.cpp"
        "src/mouse_device_win32.cpp"
        #"src/hid_api_win32.cpp"
        "src/system_info_win32.cpp")

    target_compile_definitions(drako-system PRIVATE
        "UNICODE"
        "WIN32_LEAN_AND_MEAN"
        "NOMINMAX"
        "STRICT")

    add_library(drako::system ALIAS drako-system)
endif()

#set_target_properties(native_window_api native_keyboard_api PROPERTIES CXX_STANDARD 17)

#add_executable(drako-hid-test "./test/hid_test_001.cpp")
#target_link_libraries(drako-hid-test PRIVATE drako::system)

add_executable(drako-sys-mouse-app "./test/mouse_app_001.cpp")
target_link_libraries(drako-sys-mouse-app PRIVATE drako::system)

add_executable(drako-sys-keyboard-app "./test/keyboard_app_001.cpp")
target_link_libraries(drako-sys-keyboard-app PRIVATE drako::system)

add_executable(thread-test-001 "test/thread_test_001.cpp")