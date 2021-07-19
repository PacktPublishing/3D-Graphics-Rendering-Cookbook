cmake_minimum_required(VERSION 3.12)

project(Chapter8)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/vulkan/include)

include_directories(../../deps/src/imgui)

include_directories(../../shared)

SETUP_APP(Ch8_SampleVK03_HDR "Chapter 08")

target_link_libraries(Ch8_SampleVK03_HDR PRIVATE SharedUtils)

if(BUILD_WITH_EASY_PROFILER)
	target_link_libraries(Ch8_SampleVK03_HDR PRIVATE easy_profiler)
endif()

if(BUILD_WITH_OPTICK)
	target_link_libraries(Ch8_SampleVK03_HDR PRIVATE OptickCore)
endif()
