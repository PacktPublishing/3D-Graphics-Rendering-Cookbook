cmake_minimum_required(VERSION 3.12)

project(Chapter5)

include(../../CMake/CommonMacros.txt)

include_directories(../../shared)

SETUP_APP(Ch5_Tool05_MeshConvert "Chapter 05")

target_link_libraries(Ch5_Tool05_MeshConvert PRIVATE assimp meshoptimizer SharedUtils)
