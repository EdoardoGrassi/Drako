cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

add_library(wave STATIC "src/wave.cpp")
add_library(drako::wave ALIAS wave)

add_executable(wave-viewer "src/wave_viewer.cpp")
target_link_libraries(wave-viewer PRIVATE drako::wave)