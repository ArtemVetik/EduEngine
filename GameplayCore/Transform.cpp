#include "pch.h"
#include "Transform.h"

namespace EduEngine
{
	Transform::Transform(GameObject* parent) :
		Component(parent),
		m_Position{0, 0, 0},
		m_Rotation{0, 0, 0, 0},
		m_Scale{1, 1, 1},
		m_Dirty{ false }
	{
		UpdateWorldMatrix();
	}

	void Transform::Update()
	{
		if (m_Dirty)
		{
			UpdateWorldMatrix();
			m_Dirty = false;
		}
	}

	void Transform::SetPosition(Vector3 position)
	{
		m_Position = position;
		m_Dirty = true;
	}

	void Transform::SetRotation(Quaternion rotation)
	{
		m_Rotation = rotation;
		m_Dirty = true;
	}

	void Transform::SetScale(Vector3 scale)
	{
		m_Scale = scale;
		m_Dirty = true;
	}

	void Transform::UpdateWorldMatrix()
	{
		m_WorldMatrix = Matrix::CreateScale(m_Scale) *
			Matrix::CreateFromQuaternion(m_Rotation) *
			Matrix::CreateTranslation(m_Position);
	}
}