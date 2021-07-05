#pragma once

#include <cstdlib>
#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
using glm::mat4;
using glm::vec3;
using glm::vec4;

#if defined(_MSC_VER)
#	pragma warning( push )
#	pragma warning( disable:4305 )
#endif

#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "LinearMath/btVector3.h"
#include "LinearMath/btAlignedObjectArray.h"
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"

#if defined(_MSC_VER)
#	pragma warning( pop )
#endif

#include "shared/UtilsMath.h"

struct Physics
{
	Physics():
		collisionDispatcher(&collisionConfiguration),
		dynamicsWorld(&collisionDispatcher, &broadphase, &solver, &collisionConfiguration)
	{
		dynamicsWorld.setGravity( btVector3( 0.0f, -9.8f, 0.0f ) );

		// add "floor" object - large massless box
		addBox( vec3(100.f, 0.05f, 100.f), btQuaternion(0,0,0,1), vec3(0,0,0), 0.0f);
	}

	void addBox( const vec3& halfSize, const btQuaternion& orientation, const vec3& position, float mass);

	void update(float deltaSeconds);

	std::vector<mat4> boxTransform;

private:
	std::vector<std::unique_ptr<btRigidBody>> rigidBodies;

	btDefaultCollisionConfiguration collisionConfiguration;
	btCollisionDispatcher collisionDispatcher;
	btDbvtBroadphase broadphase;
	btSequentialImpulseConstraintSolver solver;
	btDiscreteDynamicsWorld dynamicsWorld;
};
