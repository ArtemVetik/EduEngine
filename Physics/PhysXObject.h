#pragma once
#include <PxPhysicsAPI.h>
#include "IPhysicsObject.h"
#include "PhysicsWorld.h"

namespace EduEngine
{
    using namespace physx;
    using namespace DirectX::SimpleMath;

	class PhysXObject : public IPhysicsObject
	{
    public:
        PhysXObject(PxRigidActor* actor, PhysicsWorld* world);
        ~PhysXObject();

        void AddForce(const Vector3& force, ForceMode forceMode) override;
        Vector3 GetPosition() const override;
        Quaternion GetRotation() const override;
        ColliderShape GetShape() const override;

    private:
        PxRigidActor* m_Actor;
        PhysicsWorld* m_World;
    };
}