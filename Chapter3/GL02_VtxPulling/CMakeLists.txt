cmake_minimum_required(VERSION 3.12)

project(Chapter3)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Ch3_SampleGL02_VtxPulling "Chapter 03")

target_sources(Ch3_SampleGL02_VtxPulling PRIVATE ${CMAKE_SOURCE_DIR}/shared/glFramework/GLShader.cpp)
target_sources(Ch3_SampleGL02_VtxPulling PRIVATE ${CMAKE_SOURCE_DIR}/shared/Utils.cpp)

target_link_libraries(Ch3_SampleGL02_VtxPulling glad glfw assimp)
