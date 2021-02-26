cmake_minimum_required(VERSION 3.0)

project("Bullet")

if(MSVC)
	add_definitions(/wd4018)
	add_definitions(/wd4244)
	add_definitions(/wd4267)
	add_definitions(/wd4305)
	add_definitions(/wd4316)
	add_definitions(/wd4359)
	add_definitions(-DB3_USE_CLEW)
	if(CMAKE_SIZEOF_VOID_P LESS 8)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GS- /fp:fast")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox /Ob2 /Oy /Ot")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Ox /Ob2 /Oy /Ot")
	STRING (REGEX REPLACE "/RTC[^ ]*" "" CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG}")
endif(MSVC)

include_directories(../../src/bullet/src)

file(GLOB CPP_FILES
	../../src/bullet/src/BulletCollision/BroadphaseCollision/*.cpp
	../../src/bullet/src/BulletCollision/CollisionDispatch/*.cpp
	../../src/bullet/src/BulletCollision/CollisionShapes/*.cpp
	../../src/bullet/src/BulletCollision/Gimpact/*.cpp
	../../src/bullet/src/BulletCollision/NarrowPhaseCollision/*.cpp
	../../src/bullet/src/BulletDynamics/Character/*.cpp
	../../src/bullet/src/BulletDynamics/ConstraintSolver/*.cpp
	../../src/bullet/src/BulletDynamics/Dynamics/*.cpp
	../../src/bullet/src/BulletDynamics/Featherstone/*.cpp
	../../src/bullet/src/BulletDynamics/MLCPSolvers/*.cpp
	../../src/bullet/src/BulletDynamics/Vehicle/*.cpp
	../../src/bullet/src/BulletSoftBody/*.cpp
	../../src/bullet/src/LinearMath/*.cpp
# OpenCL support
#	../../src/bullet/src/clew/*.cpp
#	../../src/bullet/src/Bullet3OpenCL/BroadphaseCollision/*.cpp
#	../../src/bullet/src/Bullet3OpenCL/Initialize/*.cpp
#	../../src/bullet/src/Bullet3OpenCL/NarrowphaseCollision/*.cpp
#	../../src/bullet/src/Bullet3OpenCL/ParallelPrimitives/*.cpp
#	../../src/bullet/src/Bullet3OpenCL/Raycast/*.cpp
#	../../src/bullet/src/Bullet3OpenCL/RigidBody/*.cpp
)

file(GLOB H_FILES
	../../src/bullet/*.h
	../../src/bullet/src/BulletCollision/BroadphaseCollision/*.h
	../../src/bullet/src/BulletCollision/CollisionDispatch/*.h
	../../src/bullet/src/BulletCollision/CollisionShapes/*.h
	../../src/bullet/src/BulletCollision/Gimpact/*.h
	../../src/bullet/src/BulletCollision/NarrowPhaseCollision/*.h
	../../src/bullet/src/BulletDynamics/Character/*.h
	../../src/bullet/src/BulletDynamics/ConstraintSolver/*.h
	../../src/bullet/src/BulletDynamics/Dynamics/*.h
	../../src/bullet/src/BulletDynamics/Featherstone/*.h
	../../src/bullet/src/BulletDynamics/MLCPSolvers/*.h
	../../src/bullet/src/BulletDynamics/Vehicle/*.h
	../../src/bullet/src/BulletSoftBody/*.h
	../../src/bullet/src/LinearMath/*.h
)

set(SRC_FILES ${CPP_FILES} ${H_FILES})

add_library(Bullet ${SRC_FILES})

set_property(TARGET Bullet PROPERTY FOLDER "ThirdPartyLibraries")
