#include "pch.h"
#include "PhysXShape.h"

namespace EduEngine
{
	PhysXShape::PhysXShape(ColliderShape& shape, PxPhysics* physics, void* userData) :
		m_ColliderGeometry(shape),
		m_Physics(physics),
		m_TriggerEnter(nullptr)
	{
		PxGeometry* pxGeo = ToPxGeometry(shape);
		m_Material = m_Physics->createMaterial(0.5f, 0.5f, 0.5f);
		m_Shape = m_Physics->createShape(*pxGeo, *m_Material, true);

		m_UserData.Shape = this;
		m_UserData.UserData = userData;

		m_Shape->userData = &m_UserData;

		delete pxGeo;
	}

	PhysXShape::~PhysXShape()
	{
		m_Shape->userData = nullptr;
		m_UserData.Shape = nullptr;
		m_UserData.UserData = nullptr;

		PX_RELEASE(m_Material);
		PX_RELEASE(m_Shape);

		m_Material = nullptr;
		m_Shape = nullptr;
	}

	void PhysXShape::SetLocalTransform(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Quaternion rotation)
	{
		PxTransform transform = {};
		transform.p = { position.x, position.y, position.z };
		transform.q = { rotation.x, rotation.y, rotation.z, rotation.w };

		m_Shape->setLocalPose(transform);
	}

	void PhysXShape::SetGeometry(ColliderShape& shape)
	{
		m_ColliderGeometry = shape;
		PxGeometry* pxGeo = ToPxGeometry(shape);
		m_Shape->setGeometry(*pxGeo);
		delete pxGeo;
	}

	void PhysXShape::SetMaterial(float staticFriction, float dynamicFriction, float restitution)
	{
		m_Material->setStaticFriction(staticFriction);
		m_Material->setDynamicFriction(dynamicFriction);
		m_Material->setRestitution(restitution);
		m_Shape->setMaterials(&m_Material, 1);
	}

	void PhysXShape::SetTrigger(bool isTrigger)
	{
		m_Shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !isTrigger);
		m_Shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, isTrigger);

		PxFilterData filterData;
		filterData.word0 = 1;
		m_Shape->setSimulationFilterData(filterData);
	}

	ColliderShape PhysXShape::GetGeometry()
	{
		return m_ColliderGeometry;
	}

	void PhysXShape::SetTriggerEnterCallback(std::function<void(void*)> callback)
	{
		m_TriggerEnter = callback;
	}

	void PhysXShape::SetTriggerExitCallback(std::function<void(void*)> callback)
	{
		m_TriggerExit = callback;
	}

	void PhysXShape::SetCollisionEnterCallback(std::function<void(PhysXCollisionData)> callback)
	{
		m_CollisionEnter = callback;
	}

	void PhysXShape::SetCollisionExitCallback(std::function<void(PhysXCollisionData)> callback)
	{
		m_CollisionExit = callback;
	}

	void PhysXShape::CallTriggerEnter(void* otherUserData)
	{
		if (m_TriggerEnter)
			m_TriggerEnter(otherUserData);
	}

	void PhysXShape::CallTriggerExit(void* otherUserData)
	{
		if (m_TriggerExit)
			m_TriggerExit(otherUserData);
	}

	void PhysXShape::CallCollisionEnter(PhysXCollisionData collisionData)
	{
		if (m_CollisionEnter)
			m_CollisionEnter(collisionData);
	}

	void PhysXShape::CallCollisionExit(PhysXCollisionData collisionData)
	{
		if (m_CollisionExit)
			m_CollisionExit(collisionData);
	}

	PxGeometry* PhysXShape::ToPxGeometry(ColliderShape& shape)
	{
		switch (shape.type)
		{
		case ColliderType::Box: return new PxBoxGeometry(shape.box.width, shape.box.height, shape.box.depth);
		case ColliderType::Capsule: return new PxCapsuleGeometry(shape.capsule.radius, shape.capsule.halfHeight);
		case ColliderType::Sphere: return new PxSphereGeometry(shape.sphere.radius);
		default: return nullptr;
		}
	}
}