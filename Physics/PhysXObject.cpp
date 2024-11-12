#include "pch.h"
#include <vector>
#include "PhysXObject.h"
#include "PhysXShape.h"

namespace EduEngine
{
	PhysXObject::PhysXObject(PxPhysics* physics, PxScene* scene, DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Quaternion rotation, bool isStatic)
	{
		PxVec3 pxPos(position.x, position.y, position.z);
		PxQuat pxQuat(rotation.x, rotation.y, rotation.z, rotation.w);
		PxTransform pxTransform(pxPos, pxQuat);

		if (isStatic)
			m_Actor = physics->createRigidStatic(pxTransform);
		else
			m_Actor = physics->createRigidDynamic(pxTransform);

		if (!isStatic)
			PxRigidBodyExt::updateMassAndInertia(*m_Actor->is<PxRigidDynamic>(), 10.0f);

		m_Scene = scene;
		m_Scene->addActor(*m_Actor);
	}

	PhysXObject::~PhysXObject()
	{
		auto shapeCount = m_Actor->getNbShapes();
		std::vector<PxShape*> shapes(shapeCount);
		m_Actor->getShapes(shapes.data(), shapeCount);

		for (int i = 0; i < shapeCount; i++)
			m_Actor->detachShape(*shapes[i]);

		m_Scene->removeActor(*m_Actor);
		PX_RELEASE(m_Actor);
	}

	void PhysXObject::AddForce(const DirectX::SimpleMath::Vector3& force, NativeForceMode forceMode)
	{
		auto dynamicActor = m_Actor->is<PxRigidDynamic>();

		if (dynamicActor)
		{
			dynamicActor->addForce(PxVec3(force.x, force.y, force.z), (PxForceMode::Enum)forceMode);
		}
	}

	DirectX::SimpleMath::Vector3 PhysXObject::GetPosition() const
	{
		PxTransform transform = m_Actor->getGlobalPose();
		return DirectX::SimpleMath::Vector3(transform.p.x, transform.p.y, transform.p.z);
	}

	DirectX::SimpleMath::Quaternion PhysXObject::GetRotation() const
	{
		PxTransform transform = m_Actor->getGlobalPose();
		return DirectX::SimpleMath::Quaternion(transform.q.x, transform.q.y, transform.q.z, transform.q.w);
	}

	void PhysXObject::AttachShape(IPhysicsShape* shape)
	{
		PhysXShape* physXShape = dynamic_cast<PhysXShape*>(shape);
		assert(physXShape != nullptr);

		m_Actor->attachShape(physXShape->GetPxShape());
	}

	void PhysXObject::DetachShape(IPhysicsShape* shape)
	{
		PhysXShape* physXShape = dynamic_cast<PhysXShape*>(shape);
		assert(physXShape != nullptr);

		m_Actor->detachShape(physXShape->GetPxShape());
	}
}