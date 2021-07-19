cmake_minimum_required(VERSION 3.12)

project(Chapter2)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/imgui)

SETUP_APP(Ch2_Sample04_ImGui "Chapter 02")

target_link_libraries(Ch2_Sample04_ImGui glad glfw)
