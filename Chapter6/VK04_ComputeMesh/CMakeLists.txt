cmake_minimum_required(VERSION 3.12)

project(Chapter6)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/vulkan/include)

include_directories(../../deps/src/imgui)

include_directories(../../shared)

SETUP_APP(Ch6_SampleVK04_ComputeMesh "Chapter 06")

target_link_libraries(Ch6_SampleVK04_ComputeMesh PRIVATE SharedUtils)

if(BUILD_WITH_EASY_PROFILER)
	target_link_libraries(Ch6_SampleVK04_ComputeMesh PRIVATE easy_profiler)
endif()

if(BUILD_WITH_OPTICK)
	target_link_libraries(Ch6_SampleVK04_ComputeMesh PRIVATE OptickCore)
endif()
