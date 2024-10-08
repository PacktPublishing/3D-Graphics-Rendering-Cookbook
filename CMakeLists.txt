cmake_minimum_required(VERSION 3.12)

project(RenderingCookbook CXX C)

option(BUILD_WITH_EASY_PROFILER "Enable EasyProfiler usage" ON)
option(BUILD_WITH_OPTICK "Enable Optick usage" OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(WIN32)
	set(PYTHON_EXECUTABLE "python")
else()
	set(PYTHON_EXECUTABLE "python3")
endif()

if((NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/.bootstrap.json") OR ("${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt" IS_NEWER_THAN "${CMAKE_CURRENT_SOURCE_DIR}/deps/.bootstrap.json"))
	execute_process(
		COMMAND ${PYTHON_EXECUTABLE} "bootstrap.py"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		RESULT_VARIABLE bootstrap_result
	)
	if (NOT (${bootstrap_result} EQUAL 0))
		message(FATAL_ERROR "Cannot run bootstrap.py")
	endif()
endif()

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

set(EASY_PROFILER_NO_GUI ON CACHE BOOL "")
set(EASY_PROFILER_NO_SAMPLES ON CACHE BOOL "")

set(ASSIMP_NO_EXPORT ON CACHE BOOL "")
set(ASSIMP_BUILD_DRACO OFF CACHE BOOL "")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
set(ASSIMP_INSTALL_PDB OFF CACHE BOOL "")
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "")
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "")
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "")

set(MESHOPT_BUILD_DEMO OFF CACHE BOOL "")
set(MESHOPT_BUILD_TOOLS OFF CACHE BOOL "")
set(MESHOPT_BUILD_SHARED_LIBS OFF CACHE BOOL "")

add_subdirectory(deps/src/etc2comp)

if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

#include_directories(deps/src/vulkan/include)

add_subdirectory(deps/src/glfw)
if(BUILD_WITH_OPTICK)
add_subdirectory(deps/src/optick)
endif()
add_subdirectory(deps/src/easy_sources)
add_subdirectory(deps/src/assimp)
add_subdirectory(deps/src/meshoptimizer)
add_subdirectory(deps/src/volk)

set(ENABLE_GLSLANG_BINARIES  OFF  CACHE BOOL "")
set(ENABLE_HLSL              OFF  CACHE BOOL "")
set(ENABLE_CTEST             OFF  CACHE BOOL "")
set(ENABLE_OPT               OFF  CACHE BOOL "")
set(SKIP_GLSLANG_INSTALL     ON   CACHE BOOL "")
add_subdirectory(deps/src/glslang)

include_directories(.)
include_directories(deps/src)
include_directories(deps/src/glfw/include)
include_directories(deps/src/glm)
include_directories(deps/src/gli)
include_directories(deps/glad/include)
include_directories(deps/src/assimp/include)
include_directories(deps/src/etc2comp/EtcLib/Etc)
include_directories(deps/src/etc2comp/EtcLib/EtcCodec)
include_directories(deps/src/glslang/glslang/Include)
include_directories(deps/src/taskflow)
include_directories(deps/src/rapidjson/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/deps/src/assimp/include)
include_directories(deps/src/stb)
include_directories(deps/src/glslang)
include_directories(deps/src/glslang/StandAlone)
include_directories(deps/src/glslang/src)
include_directories(deps/src/bullet/src)
include_directories(deps/src/ImGuizmo/src)

add_subdirectory(deps/cmake/bullet)
add_subdirectory(deps/cmake/ImGuizmo)

set(GLAD_SOURCES deps/glad/src/gl.c deps/glad/src/vulkan.c)
if(WIN32)
	set(GLAD_SOURCES ${GLAD_SOURCES} deps/glad/src/wgl.c)
endif()

add_library(glad ${GLAD_SOURCES})

if(BUILD_WITH_EASY_PROFILER)
	message("Enabled EasyProfiler")
	add_definitions(-DBUILD_WITH_EASY_PROFILER=1)
	include_directories(deps/src/easy_profiler/include)
	set_property(TARGET easy_profiler PROPERTY FOLDER "ThirdPartyLibraries")
	set_property(TARGET profiler_converter PROPERTY FOLDER "ThirdPartyLibraries")
endif()
if(BUILD_WITH_OPTICK)
	message("Enabled Optick")
	add_definitions(-DBUILD_WITH_OPTICK=1)
	set_property(TARGET OptickCore PROPERTY FOLDER "ThirdPartyLibraries")
endif()

set_property(TARGET glfw          PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET assimp        PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET EtcLib        PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET EtcTool       PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET glad          PROPERTY FOLDER "ThirdPartyLibraries")
#set_property(TARGET IrrXML        PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET meshoptimizer PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET volk          PROPERTY FOLDER "ThirdPartyLibraries")
if(WIN32)
  if(TARGET zlibstatic)
    set_property(TARGET zlibstatic  PROPERTY FOLDER "ThirdPartyLibraries")
  endif()
  if(TARGET UpdateAssimpLibsDebugSymbolsAndDLLs)
    set_property(TARGET UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTY FOLDER "ThirdPartyLibraries")
  endif()
endif()
set_property(TARGET uninstall     PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET glslang       PROPERTY FOLDER "ThirdPartyLibraries/glslang")
set_property(TARGET OGLCompiler   PROPERTY FOLDER "ThirdPartyLibraries/glslang")
set_property(TARGET OSDependent   PROPERTY FOLDER "ThirdPartyLibraries/glslang")
set_property(TARGET SPIRV         PROPERTY FOLDER "ThirdPartyLibraries/glslang")
set_property(TARGET SPVRemapper   PROPERTY FOLDER "ThirdPartyLibraries/glslang")

# shared utils code for GL and Vulkan
add_subdirectory(shared)

target_sources(SharedUtils PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/src/glslang/StandAlone/ResourceLimits.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/src/glslang/StandAlone/resource_limits_c.cpp)

add_subdirectory(Chapter1/2_CMake)

add_subdirectory(Chapter2/01_GLFW)
add_subdirectory(Chapter2/02_GLM)
add_subdirectory(Chapter2/03_STB)
add_subdirectory(Chapter2/04_ImGui)
add_subdirectory(Chapter2/05_EasyProfiler)
if(BUILD_WITH_OPTICK)
add_subdirectory(Chapter2/06_Optick)
endif()
add_subdirectory(Chapter2/07_Assimp)
add_subdirectory(Chapter2/08_ETC2Comp)
add_subdirectory(Chapter2/09_Taskflow)
add_subdirectory(Chapter2/10_MeshOptimizer)

add_subdirectory(Chapter3/GL01_APIWrapping)
add_subdirectory(Chapter3/GL02_VtxPulling)
add_subdirectory(Chapter3/GL03_CubeMap)
add_subdirectory(Chapter3/VK01_GLSLang)
add_subdirectory(Chapter3/VK02_DemoApp)

add_subdirectory(Chapter4/GL01_Camera)
add_subdirectory(Chapter4/GL02_FPS)
add_subdirectory(Chapter4/VK01_DemoApp)

add_subdirectory(Chapter5/GL01_Grid)
add_subdirectory(Chapter5/GL02_Tessellation)
add_subdirectory(Chapter5/GL03_MeshRenderer)
add_subdirectory(Chapter5/VK01_MultiMeshDraw)
add_subdirectory(Chapter5/MeshConvert)

add_subdirectory(Chapter6/GL01_PBR)
add_subdirectory(Chapter6/VK01_BRDF_LUT)
add_subdirectory(Chapter6/VK02_DescriptorIndexing)
add_subdirectory(Chapter6/VK03_ComputeTexture)
add_subdirectory(Chapter6/VK04_ComputeMesh)
add_subdirectory(Chapter6/VK05_PBR)
add_subdirectory(Chapter6/Util01_FilterEnvmap)

add_subdirectory(Chapter7/GL01_LargeScene)
add_subdirectory(Chapter7/SceneConverter)
add_subdirectory(Chapter7/VK01_SceneGraph)
add_subdirectory(Chapter7/VK02_LargeScene)

add_subdirectory(Chapter8/GL01_ShadowMapping)
add_subdirectory(Chapter8/GL02_SSAO)
add_subdirectory(Chapter8/GL03_HDR)
add_subdirectory(Chapter8/GL04_HDR_Adaptation)
add_subdirectory(Chapter8/VK01_ShadowMapping)
add_subdirectory(Chapter8/VK02_SSAO)
add_subdirectory(Chapter8/VK03_HDR)

add_subdirectory(Chapter9/VK01_Physics)

add_subdirectory(Chapter10/GL01_CullingCPU)
add_subdirectory(Chapter10/GL02_CullingGPU)
add_subdirectory(Chapter10/GL03_OITransparency)
add_subdirectory(Chapter10/GL04_LazyLoading)
add_subdirectory(Chapter10/GL05_Final)
add_subdirectory(Chapter10/VK01_AtomicsTest)
add_subdirectory(Chapter10/VK02_Final)
