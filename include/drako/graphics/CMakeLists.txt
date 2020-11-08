cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# include Vulkan library
# find_package(Vulkan REQUIRED)
# compile_definitions(PRIVATE VK_USE_PLATFORM_WIN32_KHR)
# include_directories(PRIVATE Vulkan::Vulkan)
# target_link_libraries(vulkaninfo Vulkan::Vulkan)

include_directories(${PROJECT_SOURCE_DIR}/include)


# Find Vulkan Path using CMake's Vulkan Module
# This will return Boolean 'Vulkan_FOUND' indicating the status of find as success(ON) or fail(OFF).
# Include directory path - 'Vulkan_INCLUDE_DIRS' and 'Vulkan_LIBRARY' with required libraries.
find_package(Vulkan REQUIRED)
	
# Try extracting VulkanSDK path from ${Vulkan_INCLUDE_DIRS}
if(NOT ${Vulkan_INCLUDE_DIRS} STREQUAL "")
	# set(VULKAN_PATH ${Vulkan_INCLUDE_DIRS})
	# string(REGEX REPLACE "/Include" "" VULKAN_PATH ${VULKAN_PATH})
endif()
		 
if(NOT Vulkan_FOUND)
	# CMake may fail to locate the libraries but could be able to 
	# provide some path in Vulkan SDK include directory variable
	# 'Vulkan_INCLUDE_DIRS', try to extract path from this.
	message(STATUS "Failed to locate Vulkan SDK, retrying again...")
	if(EXISTS "${VULKAN_PATH}")
		message(STATUS "Successfully located the Vulkan SDK: ${VULKAN_PATH}")
	else()
		message(SEND_ERROR "Error: Unable to locate Vulkan SDK."
			"Please turn off auto locate option by specifying 'AUTO_LOCATE_VULKAN' as 'OFF'"
	        "and manually specify path using 'VULKAN_SDK' and 'VULKAN_VERSION' variables in CMakeLists.txt.")
		return()
	endif()
endif()


add_library(vulkan-forward-renderer STATIC "src/vulkan_forward_renderer.cpp" "src/vulkan_memory_allocator.cpp")
add_library(drako::vulkan-forward-renderer ALIAS vulkan-forward-renderer)

# set_property(TARGET vulkan_forward_renderer PROPERTY CXX_STANDARD 17)
target_include_directories(vulkan-forward-renderer PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(vulkan-forward-renderer PUBLIC ${Vulkan_LIBRARIES})
if(WIN32)
	target_compile_definitions(vulkan-forward-renderer PUBLIC VK_USE_PLATFORM_WIN32_KHR)
endif()

# Disable exceptions in Vulkan headers.
# target_compile_definitions(vulkan-forward-renderer PRIVATE VULKAN_HPP_NO_EXCEPTIONS)
