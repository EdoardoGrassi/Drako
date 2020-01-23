cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# configure api header with project metadata
configure_file(drako_api_win.hpp drako_api_win.hpp)

add_subdirectory("system")