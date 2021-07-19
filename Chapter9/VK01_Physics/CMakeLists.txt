cmake_minimum_required(VERSION 3.12)

project(Chapter9)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/vulkan/include)

include_directories(../../deps/src/imgui)

include_directories(../../shared)

SETUP_APP(Ch9_SampleVK01_Physics "Chapter 09")

target_link_libraries(Ch9_SampleVK01_Physics PRIVATE SharedUtils Bullet)

if(BUILD_WITH_EASY_PROFILER)
	target_link_libraries(Ch9_SampleVK01_Physics PRIVATE easy_profiler)
endif()

if(BUILD_WITH_OPTICK)
	target_link_libraries(Ch9_SampleVK01_Physics PRIVATE OptickCore)
endif()
