#pragma once
#include "SimpleMath.h"
#include "Component.h"

namespace EduEngine
{
	using namespace DirectX::SimpleMath;

	class GAMEPLAYCORE_API GameObject;

	class GAMEPLAYCORE_API Transform : public Component
	{
	public:
		Transform(GameObject* parent);

		void Update() override;

		Vector3 GetPosition() const { return m_Position; }
		Quaternion GetRotation() const { return m_Rotation; }
		Vector3 GetScale() const { return m_Scale; }
		Matrix GetWorldMatrix() const { return m_WorldMatrix; }

		void SetPosition(Vector3 position);
		void SetRotation(Quaternion rotation);
		void SetScale(Vector3 scale);

	private:
		void UpdateWorldMatrix();

	private:
		Vector3 m_Position;
		Quaternion m_Rotation;
		Vector3 m_Scale;
		Matrix m_WorldMatrix;

		bool m_Dirty;
	};
}