cmake_minimum_required(VERSION 3.12)

project(Chapter9)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/vulkan/include)
include_directories(../../deps/src/imgui)
include_directories(../../shared)

SETUP_APP(Ch10_SampleGL02_CullingGPU "Chapter 10")

target_link_libraries(Ch10_SampleGL02_CullingGPU PRIVATE SharedUtils)
