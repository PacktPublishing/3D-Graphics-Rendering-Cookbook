cmake_minimum_required(VERSION 3.12)

project(Chapter6)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Ch6_SampleGL01_PBR "Chapter 06")

target_link_libraries(Ch6_SampleGL01_PBR SharedUtils)
