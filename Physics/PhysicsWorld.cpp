#include "pch.h"
#include "PhysicsWorld.h"
#include "PhysXObject.h"

namespace EduEngine
{
	PhysicsWorld::PhysicsWorld()
	{
		m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

		m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale(), true);

		PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		m_Dispatcher = PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = m_Dispatcher;
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		m_Scene = m_Physics->createScene(sceneDesc);

		m_Material = m_Physics->createMaterial(0.5f, 0.5f, 0.6f);

		PxRigidStatic* groundPlane = PxCreatePlane(*m_Physics, PxPlane(0, 1, 0, 0), *m_Material);
		m_Scene->addActor(*groundPlane);
	}

	PhysicsWorld::~PhysicsWorld()
	{
		PX_RELEASE(m_Scene);
		PX_RELEASE(m_Dispatcher);
		PX_RELEASE(m_Physics);
		PX_RELEASE(m_Foundation);
	}

	void PhysicsWorld::Update()
	{
		m_Scene->simulate(1 / 60.0f);
		m_Scene->fetchResults(true);
	}

	void PhysicsWorld::RemoveActorFromScene(PxActor& actor)
	{
		m_Scene->removeActor(actor);
	}

	std::shared_ptr<IPhysicsObject> PhysicsWorld::AddBody(Vector3 position, Quaternion rotation, ColliderShape& geometry, bool isStatic)
	{
		PxGeometry* pxGeo = GetGeometry(geometry);
		PxShape* shape = m_Physics->createShape(*pxGeo, *m_Material);

		PxRigidActor* body;
		PxVec3 pxPos(position.x, position.y, position.z);
		PxQuat pxQuat(rotation.x, rotation.y, rotation.z, rotation.w);
		PxTransform pxTransform(pxPos, pxQuat);

		if (isStatic)
			body = m_Physics->createRigidStatic(pxTransform);
		else
			body = m_Physics->createRigidDynamic(pxTransform);

		body->attachShape(*shape);

		if (!isStatic)
			PxRigidBodyExt::updateMassAndInertia(*body->is<PxRigidDynamic>(), 10.0f);

		m_Scene->addActor(*body);

		shape->release();
		delete pxGeo;

		return std::make_shared<PhysXObject>(body, this);
	}

	PxGeometry* PhysicsWorld::GetGeometry(ColliderShape& shape)
	{
		switch (shape.type)
		{
		case ColliderType::Box: return new PxBoxGeometry(shape.box.width, shape.box.height, shape.box.depth);
		case ColliderType::Capsule: return new PxCapsuleGeometry(shape.capsule.radius, shape.capsule.halfHeight);
		case ColliderType::Sphere: return new PxSphereGeometry(shape.sphere.radius);
		default: throw;
		}
	}
}