cmake_minimum_required(VERSION 3.0)

project("ImGuizmo")

include_directories(../../src/imgui)
include_directories(../../src/ImGuizmo)

add_library(ImGuizmo ../../src/ImGuizmo/ImGuizmo.cpp ../../src/ImGuizmo/ImGuizmo.h)

set_property(TARGET ImGuizmo PROPERTY FOLDER "ThirdPartyLibraries")
