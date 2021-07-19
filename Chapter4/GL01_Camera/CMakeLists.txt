cmake_minimum_required(VERSION 3.12)

project(Chapter4)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Ch4_SampleGL01_Camera "Chapter 04")

target_link_libraries(Ch4_SampleGL01_Camera SharedUtils)
