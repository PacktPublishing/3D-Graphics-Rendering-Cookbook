#include "physics.h"

static inline btVector3 Vec3ToBulletVec3( const vec3& v )
{
	return btVector3( v.x, v.y, v.z );
}

void Physics::addBox( const glm::vec3& halfSize, const btQuaternion& orientation, const vec3& position, float mass)
{
	boxTransform.push_back(glm::mat4(1.0f));

	btCollisionShape* collisionShape = new btBoxShape( Vec3ToBulletVec3( halfSize ) );
	btDefaultMotionState* motionState = new btDefaultMotionState( btTransform(orientation, Vec3ToBulletVec3( position ) ) );

	btVector3 localInertia( 0, 0, 0 );
	collisionShape->calculateLocalInertia( mass, localInertia );

	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
		mass, motionState, collisionShape, localInertia
	);

	rigidBodyCI.m_friction = 0.1f;
	rigidBodyCI.m_rollingFriction = 0.1f;

	rigidBodies.emplace_back(std::make_unique<btRigidBody>(rigidBodyCI));
	dynamicsWorld.addRigidBody(rigidBodies.back().get());
}

void Physics::update(float deltaSeconds)
{
	dynamicsWorld.stepSimulation( deltaSeconds, 10, 0.01f );

	// sync with physics
	for (size_t i = 0; i != rigidBodies.size(); i++)
	{
		if (!rigidBodies[i]->isActive())
			continue;

		btTransform trans;
		rigidBodies[i]->getMotionState()->getWorldTransform(trans);
		trans.getOpenGLMatrix(glm::value_ptr(boxTransform[i]));
	}
}
