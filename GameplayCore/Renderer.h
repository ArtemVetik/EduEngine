#pragma once
#include "framework.h"
#include "Component.h"
#include "../Graphics/BufferD3D12.h"

namespace EduEngine
{
	class GAMEPLAYCORE_API GameObject;

	class GAMEPLAYCORE_API Renderer : public Component
	{
	public:
		Renderer(GameObject* parent);

		std::shared_ptr<VertexBufferD3D12> VertexBuffer;
		std::shared_ptr<IndexBufferD3D12> IndexBuffer;

		void Update() override;
	};
}