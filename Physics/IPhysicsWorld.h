#pragma once
#include <memory>
#include "framework.h"
#include "PhysicsDefinitions.h"
#include "IPhysicsObject.h"
#include "IPhysicsShape.h"

namespace EduEngine
{
	class PHYSICS_API IPhysicsWorld
	{
	public:
		virtual IPhysicsObject* AddBody(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Quaternion rotation, bool isStatic) = 0;
		virtual void RemoveBody(IPhysicsObject* object) = 0;
		virtual IPhysicsShape* CreateShape(ColliderShape& geometry, void* userData) = 0;
		virtual void RemoveShape(IPhysicsShape* shape) = 0;

		virtual void Update() = 0;
	};
}