#pragma once
#include "framework.h"
#include "PhysicsDefinitions.h"

namespace EduEngine
{
	class PHYSICS_API IPhysicsShape
	{
	public:
		virtual void SetGeometry(ColliderShape& shape) = 0;
		virtual void SetMaterial(float staticFriction, float dynamicFriction, float restitution) = 0;
		virtual void SetTrigger(bool isTrigger) = 0;
		virtual ColliderShape GetGeometry() = 0;
	};
}