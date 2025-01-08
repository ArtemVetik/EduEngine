#include "pch.h"
#include "PhysicsWorld.h"
#include "PhysXObject.h"
#include "PhysXShape.h"

namespace EduEngine
{
	PxFilterFlags CollisionFilterShader(
		PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void*, PxU32) {
		pairFlags = PxPairFlag::eCONTACT_DEFAULT;
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
		pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS;
		return PxFilterFlag::eDEFAULT;
	}

	PhysicsWorld::PhysicsWorld()
	{
		m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

		m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale(), true);

		PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		m_Dispatcher = PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = m_Dispatcher;
		sceneDesc.filterShader = CollisionFilterShader;
		sceneDesc.simulationEventCallback = &m_EventCallback;

		m_Scene = m_Physics->createScene(sceneDesc);
	}

	PhysicsWorld::~PhysicsWorld()
	{
		m_Shapes.clear();
		m_Objects.clear();

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

	IPhysicsObject* PhysicsWorld::AddBody(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Quaternion rotation, bool isStatic)
	{
		auto physXObject = std::make_shared<PhysXObject>(m_Physics, m_Scene, position, rotation, isStatic);
		m_Objects.emplace_back(physXObject);
		return physXObject.get();
	}

	void PhysicsWorld::RemoveBody(IPhysicsObject* object)
	{
		PhysXObject* physXObject = dynamic_cast<PhysXObject*>(object);
		assert(physXObject != nullptr);

		m_Objects.erase(std::remove_if(m_Objects.begin(), m_Objects.end(),
			[&physXObject](const std::shared_ptr<PhysXObject>& ptr) {
				return ptr.get() == physXObject;
			}),
			m_Objects.end());
	}

	IPhysicsShape* PhysicsWorld::CreateShape(ColliderShape& geometry, void* userData)
	{
		auto physXShape = std::make_shared<PhysXShape>(geometry, m_Physics, userData);
		m_Shapes.emplace_back(physXShape);
		return physXShape.get();
	}

	void PhysicsWorld::RemoveShape(IPhysicsShape* shape)
	{
		PhysXShape* physXShape = dynamic_cast<PhysXShape*>(shape);
		assert(physXShape != nullptr);

		m_Shapes.erase(std::remove_if(m_Shapes.begin(), m_Shapes.end(),
			[&physXShape](const std::shared_ptr<PhysXShape>& ptr) {
				return ptr.get() == physXShape;
			}),
			m_Shapes.end());
	}
}