#include "pch.h"
#include "PhysXObject.h"

namespace EduEngine
{
	PhysXObject::PhysXObject(PxRigidActor* actor, PhysicsWorld* world) :
		m_Actor(actor),
		m_World(world)
	{
	}

	PhysXObject::~PhysXObject()
	{
		m_World->RemoveActorFromScene(*m_Actor);
	}

	void PhysXObject::AddForce(const Vector3& force, ForceMode forceMode)
	{
		auto dynamicActor = m_Actor->is<PxRigidDynamic>();

		if (dynamicActor)
		{
			dynamicActor->addForce(PxVec3(force.x, force.y, force.z), (PxForceMode::Enum)forceMode);
		}
	}

	Vector3 PhysXObject::GetPosition() const
	{
		PxTransform transform = m_Actor->getGlobalPose();
		return Vector3(transform.p.x, transform.p.y, transform.p.z);
	}

	Quaternion PhysXObject::GetRotation() const
	{
		PxTransform transform = m_Actor->getGlobalPose();
		return Quaternion(transform.q.x, transform.q.y, transform.q.z, transform.q.w);
	}

	ColliderShape PhysXObject::GetShape() const
	{
		const PxU32 shapeCount = m_Actor->getNbShapes();
		
		if (shapeCount > 0) 
		{
			PxShape* shape;
			m_Actor->getShapes(&shape, 1);

			PxGeometryHolder geom = shape->getGeometry();

			if (geom.getType() == PxGeometryType::eBOX)
			{
				PxBoxGeometry boxGeom = geom.box();
				return ColliderShape(boxGeom.halfExtents.x, boxGeom.halfExtents.y, boxGeom.halfExtents.z);
			}
			if (geom.getType() == PxGeometryType::eSPHERE)
			{
				PxSphereGeometry sphereGeom = geom.sphere();
				return ColliderShape(sphereGeom.radius);
			}
			if (geom.getType() == PxGeometryType::eCAPSULE)
			{
				PxCapsuleGeometry capsuleGeom = geom.capsule();
				return ColliderShape(capsuleGeom.radius, capsuleGeom.halfHeight);
			}
		}
	}
}