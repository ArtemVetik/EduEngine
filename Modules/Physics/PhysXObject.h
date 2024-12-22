#pragma once
#include <PxPhysicsAPI.h>
#include "IPhysicsObject.h"

namespace EduEngine
{
    using namespace physx;

	class PhysXObject : public IPhysicsObject
	{
    public:
        PhysXObject(PxPhysics*                      physics,
                    PxScene*                        scene,
                    DirectX::SimpleMath::Vector3    position,
                    DirectX::SimpleMath::Quaternion rotation,
                    bool                            isStatic);
        ~PhysXObject();

        void AddForce(const  DirectX::SimpleMath::Vector3& force, NativeForceMode forceMode) override;
        DirectX::SimpleMath::Vector3 GetPosition() const override;
        DirectX::SimpleMath::Quaternion GetRotation() const override;
        void AttachShape(IPhysicsShape* shape) override;
        void DetachShape(IPhysicsShape* shape) override;

        PxRigidActor& GetActor() const { return *m_Actor; }

    private:
        PxRigidActor* m_Actor;
        PxScene* m_Scene;
    };
}