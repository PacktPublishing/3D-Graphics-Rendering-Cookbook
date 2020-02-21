cmake_minimum_required(VERSION 3.0)

project(Chapter3)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Ch3_SampleGL01_APIWrapping "Chapter 3")

target_link_libraries(Ch3_SampleGL01_APIWrapping glfw)
