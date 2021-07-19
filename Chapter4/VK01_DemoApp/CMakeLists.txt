cmake_minimum_required(VERSION 3.12)

project(Chapter4)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/imgui)
include_directories(../../deps/src/vulkan/include)
include_directories(../../shared)

SETUP_APP(Ch4_SampleVK01_DemoApp "Chapter 04")

target_link_libraries(Ch4_SampleVK01_DemoApp PRIVATE SharedUtils)

if(BUILD_WITH_EASY_PROFILER)
	target_link_libraries(Ch4_SampleVK01_DemoApp PRIVATE easy_profiler)
endif()

if(BUILD_WITH_OPTICK)
	target_link_libraries(Ch4_SampleVK01_DemoApp PRIVATE OptickCore)
endif()
