cmake_minimum_required(VERSION 3.12)

project(Chapter7)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/vulkan/include)
include_directories(../../deps/src/imgui)
include_directories(../../shared)

SETUP_APP(Ch7_SampleGL01_LargeScene "Chapter 07")

target_link_libraries(Ch7_SampleGL01_LargeScene PRIVATE SharedUtils)
