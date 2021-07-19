cmake_minimum_required(VERSION 3.12)

project(Chapter7)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/imgui)
include_directories(../../deps/src/vulkan/include)
include_directories(../../shared)

SETUP_APP(Ch7_Tool01_SceneConverter "Chapter 07")

target_link_libraries(Ch7_Tool01_SceneConverter PRIVATE SharedUtils meshoptimizer)

# For Linux we need Thread Building Blocks
if(UNIX)
	target_link_libraries(Ch7_Tool01_SceneConverter PRIVATE tbb)
endif()

if(BUILD_WITH_EASY_PROFILER)
	target_link_libraries(Ch7_Tool01_SceneConverter PRIVATE easy_profiler)
endif()

if(BUILD_WITH_OPTICK)
	target_link_libraries(Ch7_Tool01_SceneConverter PRIVATE OptickCore)
endif()
