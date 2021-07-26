cmake_minimum_required(VERSION 3.12)

project(Chapter9)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/vulkan/include)

include_directories(../../deps/src/imgui)

include_directories(../../shared)

SETUP_APP(Ch10_SampleVK02_Final "Chapter 10")

target_link_libraries(Ch10_SampleVK02_Final PRIVATE SharedUtils)

if(BUILD_WITH_EASY_PROFILER)
	target_link_libraries(Ch10_SampleVK02_Final PRIVATE easy_profiler)
endif()

if(BUILD_WITH_OPTICK)
	target_link_libraries(Ch10_SampleVK02_Final PRIVATE OptickCore)
endif()
