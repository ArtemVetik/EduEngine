#pragma once
#include "framework.h"
#include "IPhysicsShape.h"
#include <PxPhysicsAPI.h>

namespace EduEngine
{
	using namespace physx;

	class PHYSICS_API PhysXShape : public IPhysicsShape
	{
	public:
		PhysXShape(ColliderShape& shape, PxPhysics* physics);
		~PhysXShape();

		void SetGeometry(ColliderShape& shape) override;
		void SetMaterial(float staticFriction, float dynamicFriction, float restitution) override;
		ColliderShape GetGeometry() override;

		PxShape& GetPxShape() const { return *m_Shape; };

	private:
		PxGeometry* ToPxGeometry(ColliderShape& shape);

	private:
		PxPhysics* m_Physics;
		PxShape* m_Shape;
		PxMaterial* m_Material;
		ColliderShape m_ColliderGeometry;
	};
}