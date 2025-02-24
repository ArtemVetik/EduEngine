#pragma once
#include "framework.h"
#include "IPhysicsShape.h"
#include <PxPhysicsAPI.h>

namespace EduEngine
{
	using namespace physx;

	class PhysXShape;

	class PhysXShapeUserData
	{
	public:
		PhysXShape* Shape;
		void* UserData;
	};

	class PhysXShape : public IPhysicsShape
	{
	public:
		PhysXShape(ColliderShape& shape, PxPhysics* physics, void* userData);
		~PhysXShape();

		void SetLocalTransform(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Quaternion rotation) override;
		void SetGeometry(ColliderShape& shape) override;
		void SetMaterial(float staticFriction, float dynamicFriction, float restitution) override;
		void SetTrigger(bool isTrigger) override;
		ColliderShape GetGeometry() override;

		void SetTriggerEnterCallback(std::function<void(void*)> callback) override;
		void SetTriggerExitCallback(std::function<void(void*)> callback) override;
		void SetCollisionEnterCallback(std::function<void(PhysXCollisionData)> callback) override;
		void SetCollisionExitCallback(std::function<void(PhysXCollisionData)> callback) override;
		
		PxShape& GetPxShape() const { return *m_Shape; };

		void CallTriggerEnter(void* otherUserData);
		void CallTriggerExit(void* otherUserData);
		void CallCollisionEnter(PhysXCollisionData collisionData);
		void CallCollisionExit(PhysXCollisionData collisionData);

	private:
		PxGeometry* ToPxGeometry(ColliderShape& shape);

	private:
		PxPhysics* m_Physics;
		PxShape* m_Shape;
		PxMaterial* m_Material;
		ColliderShape m_ColliderGeometry;

		PhysXShapeUserData m_UserData;

		std::function<void(void*)> m_TriggerEnter;
		std::function<void(void*)> m_TriggerExit;
		std::function<void(PhysXCollisionData)> m_CollisionEnter;
		std::function<void(PhysXCollisionData)> m_CollisionExit;
	};
}