cmake_minimum_required(VERSION 3.0)

project(Chapter6)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/vulkan/include)
include_directories(../../shared)

SETUP_APP(Ch6_Util01_FilterEnvmap "Chapter 6")

target_link_libraries(Ch6_Util01_FilterEnvmap PRIVATE SharedUtils)
