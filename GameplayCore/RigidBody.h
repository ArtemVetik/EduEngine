#pragma once
#include "Component.h"
#include "../Physics/IPhysicsWorld.h"
#include "../Graphics/DebugRendererSystem.h"

namespace EduEngine
{
	class GAMEPLAYCORE_API Transform;

	class GAMEPLAYCORE_API RigidBody : public Component
	{
	public:
		RigidBody(GameObject* parent);

		void Initialize(IPhysicsWorld* physicsWorld, DebugRendererSystem* debugRenderer);
		void Update() override;
		
		void SetGeometry(ColliderShape geometry);
		void SetStatic(bool isStatic);

		void AddForce(const Vector3& force, ForceMode forceMode);

	private:
		IPhysicsWorld* m_Physics;
		DebugRendererSystem* m_DebugRenderer = nullptr;

		std::shared_ptr<IPhysicsObject> m_Body = nullptr;
		bool m_IsStatic;
		ColliderShape m_Geometry;
	};
}