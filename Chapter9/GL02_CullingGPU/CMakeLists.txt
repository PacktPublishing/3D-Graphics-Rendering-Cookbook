cmake_minimum_required(VERSION 3.0)

project(Chapter9)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/vulkan/include)
include_directories(../../deps/src/imgui)
include_directories(../../shared)

SETUP_APP(Ch9_SampleGL02_CullingGPU "Chapter 9")

target_link_libraries(Ch9_SampleGL02_CullingGPU PRIVATE SharedUtils)
