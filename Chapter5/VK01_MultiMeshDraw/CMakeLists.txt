cmake_minimum_required(VERSION 3.12)

project(Chapter5)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/imgui)
include_directories(../../deps/src/vulkan/include)
include_directories(../../shared)

SETUP_APP(Ch5_SampleVK01_MultiMeshDraw "Chapter 05")

target_link_libraries(Ch5_SampleVK01_MultiMeshDraw PRIVATE SharedUtils)

if(BUILD_WITH_EASY_PROFILER)
	target_link_libraries(Ch5_SampleVK01_MultiMeshDraw PRIVATE easy_profiler)
endif()

if(BUILD_WITH_OPTICK)
	target_link_libraries(Ch5_SampleVK01_MultiMeshDraw PRIVATE OptickCore)
endif()
