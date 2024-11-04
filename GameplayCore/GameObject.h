#pragma once
#include <memory>
#include <vector>
#include <type_traits>
#include "framework.h"
#include "Transform.h"
#include "IDGenerator.h"

namespace EduEngine
{
	class GAMEPLAYCORE_API GameObject
	{
	public:
		GameObject();

		std::shared_ptr<Transform> GetTransform() const;

		void Update();

		template <class T>
		typename std::enable_if<std::is_base_of<Component, T>::value && !std::is_same<T, Transform>::value, std::shared_ptr<T>>::type
			AddComponent();

		template <class T = Component>
		std::shared_ptr<T> GetComponent();

	private:
		GUID m_Guid;

		std::vector<std::shared_ptr<Component>> m_Components;
		std::shared_ptr<Transform> m_Transform;
	};

	template<class T>
	inline typename std::enable_if<std::is_base_of<Component, T>::value && !std::is_same<T, Transform>::value, std::shared_ptr<T>>::type
		GameObject::AddComponent()
	{
		auto component = std::make_shared<T>(this);
		m_Components.push_back(component);
		return component;
	}

	template<class T>
	inline std::shared_ptr<T> GameObject::GetComponent()
	{
		for (auto&& component : m_Components)
		{
			auto ptr = component.get();
			if (dynamic_cast<T*>(ptr) != nullptr)
			{
				return std::static_pointer_cast<T>(component);
			}
		}
		return nullptr;
	}
}