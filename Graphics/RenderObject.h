#pragma once
#include "pch.h"
#include "BufferD3D12.h"

namespace EduEngine
{
	struct GRAPHICS_API ObjectConstants
	{
		DirectX::XMFLOAT4X4 World;
	};

	struct GRAPHICS_API PassConstants
	{
		DirectX::XMFLOAT4X4 ViewProj;
	};
}