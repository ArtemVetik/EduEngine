#include "pch.h"
#include "PhysXShape.h"

namespace EduEngine
{
	PhysXShape::PhysXShape(ColliderShape& shape, PxPhysics* physics) :
		m_ColliderGeometry(shape),
		m_Physics(physics)
	{
		PxGeometry* pxGeo = ToPxGeometry(shape);
		m_Material = m_Physics->createMaterial(0.5f, 0.5f, 0.5f);
		m_Shape = m_Physics->createShape(*pxGeo, *m_Material);
		delete pxGeo;
	}

	PhysXShape::~PhysXShape()
	{
		PX_RELEASE(m_Material);
		PX_RELEASE(m_Shape);
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

	ColliderShape PhysXShape::GetGeometry()
	{
		return m_ColliderGeometry;
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