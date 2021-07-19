cmake_minimum_required(VERSION 3.12)

project(Chapter2)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Ch2_Sample08_ETC2Comp "Chapter 02")

target_sources(Ch2_Sample08_ETC2Comp PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../../../deps/src/etc2comp/EtcTool/EtcFile.cpp)
target_sources(Ch2_Sample08_ETC2Comp PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../../../deps/src/etc2comp/EtcTool/EtcFileHeader.cpp)

target_link_libraries(Ch2_Sample08_ETC2Comp glad glfw EtcLib)
