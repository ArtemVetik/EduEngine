#pragma once
#include <memory>
#include "framework.h"
#include "IPhysicsWorld.h"

namespace EduEngine
{
	class PHYSICS_API PhysicsFactory
	{
	public:
		std::shared_ptr<IPhysicsWorld> Create();
	};
}