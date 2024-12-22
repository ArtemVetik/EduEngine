#include "pch.h"
#include "PhysicsFactory.h"
#include "PhysicsWorld.h"

namespace EduEngine
{
    std::shared_ptr<IPhysicsWorld> PhysicsFactory::Create()
    {
        return std::static_pointer_cast<IPhysicsWorld>(std::make_shared<PhysicsWorld>());
    }
}