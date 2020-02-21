cmake_minimum_required(VERSION 3.12)

project(SharedUtils CXX C)

include(../CMake/CommonMacros.txt)

include_directories(../deps/src/imgui)
include_directories(../deps/src/vulkan/include)

file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c??)
file(GLOB_RECURSE HEADER_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)

add_library(SharedUtils ${SRC_FILES} ${HEADER_FILES})

set_property(TARGET SharedUtils PROPERTY CXX_STANDARD 20)
set_property(TARGET SharedUtils PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(SharedUtils PUBLIC glad glfw volk glslang SPIRV assimp)

if(BUILD_WITH_EASY_PROFILER)
	target_link_libraries(SharedUtils PUBLIC easy_profiler)
endif()

if(BUILD_WITH_OPTICK)
	target_link_libraries(SharedUtils PUBLIC OptickCore)
endif()
