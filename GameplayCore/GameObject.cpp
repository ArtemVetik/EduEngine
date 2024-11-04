#include "pch.h"
#include "GameObject.h"

namespace EduEngine
{
	GameObject::GameObject()
	{
		m_Guid = IDGenerator::Generate();
		assert(IDGenerator::CompareGUID(m_Guid, IDGenerator::INVALID_GUID) == false);

		m_Transform = std::make_shared<Transform>(this);
	}

	std::shared_ptr<Transform> GameObject::GetTransform() const
	{
		assert(m_Transform);
		assert(m_Transform->GetGameObject() == this);

		return m_Transform;
	}

	void GameObject::Update()
	{
		m_Transform->Update();

		for (auto& component : m_Components)
			component->Update();
	}
}