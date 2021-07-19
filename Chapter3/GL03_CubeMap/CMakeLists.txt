cmake_minimum_required(VERSION 3.12)

project(Chapter3)

include(../../CMake/CommonMacros.txt)

SETUP_APP(Ch3_SampleGL03_CubeMap "Chapter 03")

target_sources(Ch3_SampleGL03_CubeMap PRIVATE ${CMAKE_SOURCE_DIR}/shared/glFramework/GLShader.cpp)
target_sources(Ch3_SampleGL03_CubeMap PRIVATE ${CMAKE_SOURCE_DIR}/shared/Utils.cpp)
target_sources(Ch3_SampleGL03_CubeMap PRIVATE ${CMAKE_SOURCE_DIR}/shared/UtilsCubemap.cpp)

target_link_libraries(Ch3_SampleGL03_CubeMap glad glfw assimp)
