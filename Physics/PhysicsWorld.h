#pragma once
#include <string>
#include <sstream>
#include "IPhysicsWorld.h"
#include "PxPhysicsAPI.h"
#include "PhysXAllocator.h"
#include "PhysXErrorCallback.h"

namespace EduEngine
{
	using namespace physx;

	class PhysicsWorld : public IPhysicsWorld
	{
	public:
		PhysicsWorld();
		~PhysicsWorld();

		std::shared_ptr<IPhysicsObject> AddBody(Vector3 position, Quaternion rotation, ColliderShape& geometry, bool isStatic) override;
		void Update() override;
	
		void RemoveActorFromScene(PxActor& actor);

	private:
		PxGeometry* GetGeometry(ColliderShape& shape);

	private:
		PxFoundation* m_Foundation = NULL;
		PxPhysics* m_Physics = NULL;
		PxDefaultCpuDispatcher* m_Dispatcher = NULL;
		PxScene* m_Scene = NULL;
		PxMaterial* m_Material = NULL;

		PhysXAllocator m_Allocator = {};
		PhysXErrorCallback m_ErrorCallback = {};
	};
}