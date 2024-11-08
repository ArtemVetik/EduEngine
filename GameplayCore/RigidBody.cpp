#include "pch.h"
#include "RigidBody.h"
#include "GameObject.h"

namespace EduEngine
{
	RigidBody::RigidBody(GameObject* parent) :
		Component(parent),
		m_Geometry(0.5f, 0.5f, 0.5f),
		m_IsStatic(false)
	{
	}

	void RigidBody::Initialize(IPhysicsWorld* physicsWorld, DebugRendererSystem* debugRenderer)
	{
		m_Physics = physicsWorld;
		m_DebugRenderer = debugRenderer;

		auto transform = GetGameObject()->GetTransform();
		m_Body = m_Physics->AddBody(transform->GetPosition(), transform->GetRotation(), m_Geometry, m_IsStatic);
	}

	void RigidBody::Update()
	{
		Vector3 position = m_Body->GetPosition();
		Quaternion rotation = m_Body->GetRotation();

		GetGameObject()->GetTransform()->SetPosition(position);
		GetGameObject()->GetTransform()->SetRotation(rotation);

		DirectX::XMVECTOR quaternion = DirectX::XMVectorSet(rotation.x, rotation.y, rotation.z, rotation.w);
		DirectX::XMVECTOR translation = DirectX::XMVectorSet(position.x, position.y, position.z, 0);
		auto transformMatrix = DirectX::XMMatrixAffineTransformation(
			DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
			DirectX::XMVectorZero(),
			quaternion,
			translation
		);

		auto shape = m_Body->GetShape();

		if (shape.type == ColliderType::Box)
		{
			auto boundingBox = DirectX::BoundingBox({}, { shape.box.width, shape.box.height, shape.box.depth});
			m_DebugRenderer->DrawBoundingBox(boundingBox, transformMatrix);
		}
		else if (shape.type == ColliderType::Capsule)
		{
			m_DebugRenderer->DrawCapsule(shape.capsule.radius, shape.capsule.halfHeight, DirectX::Colors::Green, transformMatrix, 16);
		}
		else if (shape.type == ColliderType::Sphere)
		{
			m_DebugRenderer->DrawSphere(shape.sphere.radius, DirectX::Colors::Green, transformMatrix, 16);
		}
	}

	void RigidBody::SetGeometry(ColliderShape geometry)
	{
		m_Geometry = geometry;

		auto transform = GetGameObject()->GetTransform();
		m_Body = m_Physics->AddBody(transform->GetPosition(), transform->GetRotation(), m_Geometry, m_IsStatic);
	}

	void RigidBody::SetStatic(bool isStatic)
	{
		if (m_IsStatic == isStatic)
			return;

		m_IsStatic = isStatic;
		auto transform = GetGameObject()->GetTransform();
		m_Body = m_Physics->AddBody(transform->GetPosition(), transform->GetRotation(), m_Geometry, m_IsStatic);
	}

	void RigidBody::AddForce(const Vector3& force, ForceMode forceMode)
	{
		m_Body->AddForce(force, forceMode);
	}
}