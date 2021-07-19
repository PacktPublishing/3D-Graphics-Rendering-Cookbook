cmake_minimum_required(VERSION 3.12)

project(Chapter5)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/vulkan/include)
include_directories(../../deps/src/imgui)
include_directories(../../shared)

SETUP_APP(Ch5_SampleGL03_MeshRenderer "Chapter 05")

target_link_libraries(Ch5_SampleGL03_MeshRenderer PRIVATE SharedUtils)
