#pragma once
#include <SimpleMath.h>
#include "PhysicsDefinitions.h"

namespace EduEngine
{
    using namespace DirectX::SimpleMath;
    
    class PHYSICS_API IPhysicsObject
    {
    public:
        virtual ~IPhysicsObject() = default;
        virtual void AddForce(const Vector3& force, ForceMode forceMode) = 0;
        virtual Vector3 GetPosition() const = 0;
        virtual Quaternion GetRotation() const = 0;
        virtual ColliderShape GetShape() const = 0;
    };
}