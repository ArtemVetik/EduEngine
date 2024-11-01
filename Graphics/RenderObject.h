#pragma once
#include "pch.h"
#include "BufferD3D12.h"

namespace EduEngine
{
	struct ObjectConstants
	{
		DirectX::XMFLOAT4X4 World;
	};

	struct PassConstants
	{
		DirectX::XMFLOAT4X4 ViewProj;
	};

	struct GRAPHICS_API RenderObject
	{
		DirectX::XMFLOAT4X4 World;
		VertexBufferD3D12* VertexBuffer;
		IndexBufferD3D12* IndexBuffer;
	};
}