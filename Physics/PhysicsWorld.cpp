#include "pch.h"
#include "PhysicsWorld.h"

namespace EduEngine
{
	PhysicsWorld::PhysicsWorld()
	{
		m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

		m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale(), true);

		PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		m_Dispatcher = PxDefaultCpuDispatcherCreate(2);
	}

	PhysicsWorld::~PhysicsWorld()
	{
		PX_RELEASE(m_Physics);
		PX_RELEASE(m_Foundation);
	}
}