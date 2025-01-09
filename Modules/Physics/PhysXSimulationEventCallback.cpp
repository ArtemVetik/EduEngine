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
		for (PxU32 i = 0; i < nbPairs; i++)
		{
			const PxContactPair& contactPair = pairs[i];

			if (contactPair.events & PxPairFlag::eNOTIFY_TOUCH_FOUND || contactPair.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				PhysXShapeUserData* userData = (PhysXShapeUserData*)contactPair.shapes[0]->userData;
				PhysXShapeUserData* otherUserData = (PhysXShapeUserData*)contactPair.shapes[1]->userData;

				if (!userData || !otherUserData)
					continue;

				PxContactPairPoint contacts[PhysXCollisionData::MaxContacts];
				contactPair.extractContacts(contacts, contactPair.contactCount);

				PhysXCollisionData data;
				data.OtherUserData = otherUserData->UserData;
				data.ContactCount = min(contactPair.contactCount, PhysXCollisionData::MaxContacts);

				for (size_t i = 0; i < data.ContactCount; i++)
				{
					data.Contacts[i].Point = { contacts[i].position.x, contacts[i].position.y, contacts[i].position.z };
					data.Contacts[i].Normal = { contacts[i].normal.x, contacts[i].normal.y, contacts[i].normal.z };
					data.Contacts[i].Impulse = { contacts[i].impulse.x, contacts[i].impulse.y, contacts[i].impulse.z };
					data.Contacts[i].Separation = contacts[i].separation;
				}

				if (contactPair.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
				{
					data.OtherUserData = otherUserData->UserData;
					userData->Shape->CallCollisionEnter(data);

					data.OtherUserData = userData->UserData;
					otherUserData->Shape->CallCollisionEnter(data);
				}
				else
				{
					data.OtherUserData = otherUserData->UserData;
					userData->Shape->CallCollisionExit(data);

					data.OtherUserData = userData->UserData;
					otherUserData->Shape->CallCollisionExit(data);
				}
			}
		}
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
