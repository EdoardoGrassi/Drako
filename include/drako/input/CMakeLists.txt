cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

add_library(device-system STATIC)

target_compile_features(device-system PRIVATE cxx_std_17)

if(WIN32)
    target_sources(device-system PRIVATE
        "src/device_system_win32.cpp"
        "src/mouse_device_win32.cpp"
    )
    find_library(XINPUT_FOUND "xinput" REQUIRED DOC "XInput library")
    if(XINPUT_FOUND)
        message(STATUS "XInput found at ${XINPUT_FOUND}")
        target_compile_definitions(device-system PRIVATE "WIN32_LEAN_AND_MEAN" "NOMINMAX")
        target_link_libraries(device-system PRIVATE ${XINPUT_FOUND})
    else()
        message(SEND_ERROR "Cannot locate XInput library")
    endif()
endif()
add_library(drako::devices ALIAS device-system)


add_library(input-system STATIC
    "src/input_system.cpp"
    "src/input_types.cpp"
)
target_link_libraries(input-system
    PRIVATE drako::devices
    PUBLIC yaml-cpp
)
add_library(drako::input ALIAS input-system)


add_executable(device-system-testrun "test/device_system_testrun.cpp")
target_link_libraries(device-system-testrun PRIVATE drako::devices drako::input)

add_executable(input-system-testrun "test/input_system_testrun.cpp")
target_link_libraries(input-system-testrun PRIVATE drako::input)


# get GoogleTest suite
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.10.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(gtest_build_gmock OFF)
FetchContent_MakeAvailable(googletest)

add_executable(device-system-tests "test/mouse_device_tests.cpp")
target_link_libraries(device-system-tests PRIVATE drako::devices gtest_main)
gtest_discover_tests(device-system-tests)

add_executable(device-system-mouse-app "test/mouse_app.cpp")
target_link_libraries(device-system-mouse-app PRIVATE drako::devices)