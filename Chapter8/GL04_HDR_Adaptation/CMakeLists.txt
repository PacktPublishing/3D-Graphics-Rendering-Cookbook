cmake_minimum_required(VERSION 3.12)

project(Chapter8)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/vulkan/include)
include_directories(../../deps/src/imgui)
include_directories(../../shared)

SETUP_APP(Ch8_SampleGL04_HDR_Adaptation "Chapter 08")

target_link_libraries(Ch8_SampleGL04_HDR_Adaptation PRIVATE SharedUtils)
