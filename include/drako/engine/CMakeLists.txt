cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
enable_testing()



add_library(drako-runtime STATIC "src/asset_manager.cpp")
add_library(drako::runtime ALIAS drako-runtime)



add_library(render-system STATIC "src/render_system.cpp")
add_library(drako::render-system ALIAS render-system)

target_link_libraries(render-system PRIVATE drako::vulkan-forward-renderer)