cmake_minimum_required(VERSION 3.15)

# Console interface for Drako projects
add_executable(drako-cli-editor "cli_editor.cpp")
set_target_properties(drako-cli-editor
    PROPERTIES
        OUTPUT_NAME "drako-cli-editor"
        #RUNTIME_OUTPUT_DIRECTORY "D:/Desktop/drako_test_zone/projects"
)
target_link_libraries(drako-cli-editor PRIVATE drako::devel)

install(
    TARGETS drako-cli-editor
    CONFIGURATIONS Debug
    RUNTIME DESTINATION "bin"
    COMPONENT drako-cli-editor
)