cmake_minimum_required(VERSION 3.12)

project(Chapter2)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Ch2_Sample09_TaskFlow "Chapter 02")

target_link_libraries(Ch2_Sample09_TaskFlow glad glfw)
