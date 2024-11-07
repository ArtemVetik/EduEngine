#pragma once
#include "framework.h"
#include "PxPhysicsAPI.h"
#include <string>
#include <sstream>
#include "PhysXAllocator.h"
#include "PhysXErrorCallback.h"

namespace EduEngine
{
	using namespace physx;

	class PHYSICS_API PhysicsWorld
	{
	public:
		PhysicsWorld();
		~PhysicsWorld();

	private:
		PxFoundation* m_Foundation = NULL;
		PxPhysics* m_Physics = NULL;
		PxDefaultCpuDispatcher* m_Dispatcher = NULL;

		PhysXAllocator m_Allocator = {};
		PhysXErrorCallback m_ErrorCallback = {};
	};
}