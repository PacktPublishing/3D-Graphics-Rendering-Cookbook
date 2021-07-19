cmake_minimum_required(VERSION 3.12)

project(Chapter6)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/imgui)
include_directories(../../deps/src/vulkan/include)
include_directories(../../shared)

SETUP_APP(Ch6_SampleVK01_BRDF_LUT "Chapter 06")

target_link_libraries(Ch6_SampleVK01_BRDF_LUT PRIVATE SharedUtils)
