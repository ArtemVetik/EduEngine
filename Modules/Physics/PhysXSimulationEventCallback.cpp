#include "pch.h"
#include "PhysXSimulationEventCallback.h"
#include "PhysXShape.h"

namespace EduEngine
{
	void PhysXSimulationEventCallback::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
	{
	}

	void PhysXSimulationEventCallback::onWake(PxActor** actors, PxU32 count)
	{
	}

	void PhysXSimulationEventCallback::onSleep(PxActor** actors, PxU32 count)
	{
	}

	void PhysXSimulationEventCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
	{
	}

	void PhysXSimulationEventCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
	{
		for (PxU32 i = 0; i < count; i++)
		{
			if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				PhysXShapeUserData* userData = (PhysXShapeUserData*)pairs[i].triggerShape->userData;
				PhysXShapeUserData* otherUserData = (PhysXShapeUserData*)pairs[i].otherShape->userData;
				userData->Shape->CallTriggerEnter(otherUserData->UserData);
			}
			else if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				PhysXShapeUserData* userData = (PhysXShapeUserData*)pairs[i].triggerShape->userData;
				PhysXShapeUserData* otherUserData = (PhysXShapeUserData*)pairs[i].otherShape->userData;
				userData->Shape->CallTriggerExit(otherUserData->UserData);
			}
		}
	}

	void PhysXSimulationEventCallback::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
	{
	}
}
