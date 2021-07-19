cmake_minimum_required(VERSION 3.12)

project(Chapter2)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/optick/src)

add_definitions(-DBUILD_WITH_OPTICK=1)

SETUP_APP(Ch2_Sample06_Optick "Chapter 02")

target_link_libraries(Ch2_Sample06_Optick glad glfw OptickCore)
