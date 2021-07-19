cmake_minimum_required(VERSION 3.12)

project(Chapter5)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Ch5_SampleGL01_Grid "Chapter 05")

target_link_libraries(Ch5_SampleGL01_Grid SharedUtils)
