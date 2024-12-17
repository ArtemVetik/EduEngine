#pragma once
#include <string>
#include <sstream>
#include <vector>
#include "IPhysicsWorld.h"
#include "PxPhysicsAPI.h"
#include "PhysXAllocator.h"
#include "PhysXErrorCallback.h"
#include "PhysXSimulationEventCallback.h"

namespace EduEngine
{
	using namespace physx;

	class PhysXObject;
	class PhysXShape;

	class PhysicsWorld : public IPhysicsWorld
	{
	public:
		PhysicsWorld();
		~PhysicsWorld();

		IPhysicsObject* AddBody(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Quaternion rotation, bool isStatic) override;
		void RemoveBody(IPhysicsObject* object) override;
		IPhysicsShape* CreateShape(ColliderShape& geometry, void* userData) override;
		void RemoveShape(IPhysicsShape* shape) override;

		void Update() override;

	private:
		PxFoundation* m_Foundation = NULL;
		PxPhysics* m_Physics = NULL;
		PxDefaultCpuDispatcher* m_Dispatcher = NULL;
		PxScene* m_Scene = NULL;

		PhysXAllocator m_Allocator = {};
		PhysXErrorCallback m_ErrorCallback = {};
		PhysXSimulationEventCallback m_EventCallback = {};

		std::vector<std::shared_ptr<PhysXObject>> m_Objects;
		std::vector<std::shared_ptr<PhysXShape>> m_Shapes;
	};
}