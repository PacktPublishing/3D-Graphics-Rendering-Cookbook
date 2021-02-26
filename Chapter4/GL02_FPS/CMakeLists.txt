cmake_minimum_required(VERSION 3.0)

project(Chapter4)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Ch4_SampleGL02_FPS "Chapter 4")

target_link_libraries(Ch4_SampleGL02_FPS SharedUtils)
