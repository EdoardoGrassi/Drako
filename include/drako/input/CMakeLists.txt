cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(drako_input STATIC "./src/input_system.cpp")
if(WIN32)
    find_library(XINPUT_FOUND "xinput" DOC "XInput library")
    if(XINPUT_FOUND)
        message(STATUS "XInput found at ${XINPUT_FOUND}")
        target_compile_definitions(drako_input PRIVATE "WIN32_LEAN_AND_MEAN" "NOMINMAX")
        target_sources(drako_input PRIVATE "./src/xinput_device_interface_win32.cpp")
        target_link_libraries(drako_input PRIVATE ${XINPUT_FOUND})
    else()
        message(SEND_ERROR "Cannot locate XInput library")
    endif()
endif()
add_library(drako::input ALIAS drako_input)


add_library(drako_gestures STATIC "./src/gesture_system.cpp")
target_link_libraries(drako_gestures PRIVATE drako::input)
add_library(drako::gestures ALIAS drako_gestures)


add_executable(input-test-app "./test/input_gesture_app.cpp")
target_link_libraries(input-test-app PRIVATE drako::input)