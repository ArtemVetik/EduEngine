#pragma once
#include <memory>
#include "IRenderEngine.h"
#include "../Graphics/BufferD3D12.h"

namespace EduEngine
{
	class RenderObject : public IRenderObject
	{
	public:
		std::shared_ptr<VertexBufferD3D12> VertexBuffer;
		std::shared_ptr<IndexBufferD3D12> IndexBuffer;
	};
}