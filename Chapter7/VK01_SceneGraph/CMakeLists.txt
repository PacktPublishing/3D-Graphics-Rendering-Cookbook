cmake_minimum_required(VERSION 3.12)

project(Chapter7)

include(../../CMake/CommonMacros.txt)

include_directories(../../deps/src/vulkan/include)

include_directories(../../deps/src/imgui)
include_directories(../../deps/src/ImGuizmo)

include_directories(../../shared)

SETUP_APP(Ch7_SampleVK01_SceneGraph "Chapter 07")

target_link_libraries(Ch7_SampleVK01_SceneGraph PRIVATE SharedUtils ImGuizmo)
