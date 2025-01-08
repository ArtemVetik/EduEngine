#pragma once
#include <functional>

#include "framework.h"
#include "PhysicsDefinitions.h"
#include "../EduMath/SimpleMath.h"

namespace EduEngine
{
	class PHYSICS_API PhysXCollisionContactData
	{
	public:
		DirectX::XMFLOAT3 Point;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT3 Impulse;
		float Separation;
	};

	class PHYSICS_API PhysXCollisionData
	{
	public:
		static constexpr int MaxContacts = 8;

		void* OtherUserData;
		int ContactCount;
		PhysXCollisionContactData Contacts[MaxContacts];
	};

	class PHYSICS_API IPhysicsShape
	{
	public:
		virtual void SetLocalTransform(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Quaternion rotation) = 0;
		virtual void SetGeometry(ColliderShape& shape) = 0;
		virtual void SetMaterial(float staticFriction, float dynamicFriction, float restitution) = 0;
		virtual void SetTrigger(bool isTrigger) = 0;
		virtual ColliderShape GetGeometry() = 0;

		virtual void SetTriggerEnterCallback(std::function<void(void*)> callback) = 0;
		virtual void SetTriggerExitCallback(std::function<void(void*)> callback) = 0;
		virtual void SetCollisionEnterCallback(std::function<void(PhysXCollisionData)> callback) = 0;
		virtual void SetCollisionExitCallback(std::function<void(PhysXCollisionData)> callback) = 0;
	};
}