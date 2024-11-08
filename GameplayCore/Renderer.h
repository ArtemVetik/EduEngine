#pragma once
#include "framework.h"
#include "Component.h"
#include "../RenderEngine/IRenderEngine.h"

namespace EduEngine
{
	class GAMEPLAYCORE_API GameObject;

	class GAMEPLAYCORE_API Renderer : public Component
	{
	public:
		Renderer(GameObject* parent);
		~Renderer();

		void Initialize(IRenderEngine* renderEngine);
		void SetMesh(MeshData meshData);

		void Update() override;

	private:
		IRenderEngine* m_RenderEngine;
		std::shared_ptr<IRenderObject> m_Object;
	};
}