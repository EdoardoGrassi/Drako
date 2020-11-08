cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

add_library(drako-format-wave STATIC "src/wave.cpp")
add_library(drako::format-wave ALIAS drako-format-wave)

add_executable(drako-wave-viewer "src/wave_viewer.cpp")
target_link_libraries(drako-wave-viewer PRIVATE drako::format-wave drako::io)
set_target_properties(drako-wave-viewer
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "D:/Desktop/drako_test_zone/wave")
# install(TARGETS drako-obj-viewer DESTINATION "")