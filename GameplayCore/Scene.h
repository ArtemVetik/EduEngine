#pragma once
#include "framework.h"
#include <vector>
#include <memory>
#include "GameObject.h"

namespace EduEngine
{
	class GAMEPLAYCORE_API Scene
	{
	public:
		void AddGameObject(std::shared_ptr<GameObject> gameObject);

		std::vector<std::shared_ptr<GameObject>> m_Objects;
	private:
	};
}