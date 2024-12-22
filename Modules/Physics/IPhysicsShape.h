#pragma once
#include <functional>

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

		virtual void SetTriggerEnterCallback(std::function<void(void*)> callback) = 0;
		virtual void SetTriggerExitCallback(std::function<void(void*)> callback) = 0;
	};
}