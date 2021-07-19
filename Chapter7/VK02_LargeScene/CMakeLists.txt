cmake_minimum_required(VERSION 3.12)

project(Chapter7)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/vulkan/include)

include_directories(../../deps/src/imgui)

include_directories(../../shared)

SETUP_APP(Ch7_SampleVK02_LargeScene "Chapter 07")

target_link_libraries(Ch7_SampleVK02_LargeScene PRIVATE SharedUtils)

if(BUILD_WITH_EASY_PROFILER)
	target_link_libraries(Ch7_SampleVK02_LargeScene PRIVATE easy_profiler)
endif()

if(BUILD_WITH_OPTICK)
	target_link_libraries(Ch7_SampleVK02_LargeScene PRIVATE OptickCore)
endif()
