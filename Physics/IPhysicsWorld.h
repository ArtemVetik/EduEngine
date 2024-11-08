#pragma once
#include <memory>
#include "framework.h"
#include "PhysicsDefinitions.h"
#include "IPhysicsObject.h"

namespace EduEngine
{
	class PHYSICS_API IPhysicsWorld
	{
	public:
		virtual std::shared_ptr<IPhysicsObject> AddBody(Vector3 position, Quaternion rotation, ColliderShape& geometry, bool isStatic) = 0;
		virtual void Update() = 0;
	};
}