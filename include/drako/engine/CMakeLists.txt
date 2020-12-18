cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
enable_testing()


add_library(drako-runtime STATIC "src/asset_manager.cpp")
target_link_libraries(drako-runtime PRIVATE drako::io)
add_library(drako::runtime ALIAS drako-runtime)

add_library(render-system STATIC "src/render_system.cpp")
target_link_libraries(render-system PRIVATE drako::vulkan-forward-renderer)
add_library(drako::render-system ALIAS render-system)