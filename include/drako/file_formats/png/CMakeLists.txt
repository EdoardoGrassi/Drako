cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

#find_package(PNG REQUIRED)

add_library(drako-png STATIC "src/png.cpp")
add_library(drako::png ALIAS drako-png)

# add_executable(drako-png-viewer "src/drako_png_viewer.cpp")
# target_link_libraries(drako-png-viewer PRIVATE drako::png)
# set_target_properties(drako-png-viewer
   # PROPERTIES RUNTIME_OUTPUT_DIRECTORY "D:/Desktop/drako_test_zone")
# install(TARGETS drako-png-viewer DESTINATION "")