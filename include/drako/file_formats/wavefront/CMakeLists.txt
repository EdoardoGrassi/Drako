cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

add_library(drako-obj STATIC
    "src/parser.cpp"
    "src/object.cpp")
add_library(drako::obj ALIAS drako-obj)

add_executable(drako-obj-viewer "src/drako_obj_viewer.cpp")
target_link_libraries(drako-obj-viewer PRIVATE drako::obj drako::io)
set_target_properties(drako-obj-viewer
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "D:/Desktop/drako_test_zone")
# install(TARGETS drako-obj-viewer DESTINATION "")