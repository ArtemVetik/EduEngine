#pragma once
#include "../EduMath/SimpleMath.h"
#include "PhysicsDefinitions.h"
#include "IPhysicsShape.h"

namespace EduEngine
{
	class PHYSICS_API IPhysicsObject
	{
	public:
		virtual ~IPhysicsObject() = default;

		virtual void AddForce(const DirectX::SimpleMath::Vector3& force, NativeForceMode forceMode) = 0;
		virtual DirectX::SimpleMath::Vector3 GetPosition() const = 0;
		virtual DirectX::SimpleMath::Quaternion GetRotation() const = 0;
		virtual void AttachShape(IPhysicsShape* shape) = 0;
		virtual void DetachShape(IPhysicsShape* shape) = 0;

		virtual void FreezePosition(bool x, bool y, bool z) = 0;
		virtual void FreezeRotation(bool x, bool y, bool z) = 0;
		virtual void SetGlobalTransform(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Quaternion rotation) = 0;
		virtual void SetMass(float mass) = 0;
		virtual void SetLinearVelocity(DirectX::SimpleMath::Vector3 velocity) = 0;
		virtual DirectX::SimpleMath::Vector3 GetLinearVelocity() = 0;
	};
}