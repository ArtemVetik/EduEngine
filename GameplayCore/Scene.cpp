#include "pch.h"
#include "Scene.h"

namespace EduEngine
{
	void Scene::AddGameObject(std::shared_ptr<GameObject> gameObject)
	{
		m_Objects.push_back(gameObject);
	}
}