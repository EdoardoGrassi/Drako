cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
enable_testing()

include(FetchContent)
FetchContent_Declare(
    rio
    GIT_REPOSITORY https://github.com/EdoardoGrassi/rio
)
set(RIO_BUILD_TESTS OFF CACHE BOOL "not include tests")
FetchContent_MakeAvailable(rio)

add_library(drako-runtime STATIC
    "src/asset_system.cpp"
)
target_link_libraries(drako-runtime PRIVATE glm drako::devel rio)
add_library(drako::runtime ALIAS drako-runtime)

#[[
add_library(render-system STATIC "src/render_system.cpp")
target_link_libraries(render-system PRIVATE drako::vulkan-forward-renderer)
add_library(drako::render-system ALIAS render-system)
#]]