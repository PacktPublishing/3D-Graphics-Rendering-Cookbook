cmake_minimum_required(VERSION 3.12)

project(Chapter2)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/easy_profiler/include)

add_definitions(-DBUILD_WITH_EASY_PROFILER=1)

SETUP_APP(Ch2_Sample05_EasyProfiler "Chapter 02")

target_link_libraries(Ch2_Sample05_EasyProfiler glad glfw easy_profiler)
