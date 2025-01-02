#pragma once
#include "../Graphics/BufferD3D12.h"
#include "../Graphics/TextureD3D12.h"
#include "RenderPasses.h"
#include "Camera.h"

namespace EduEngine
{
	class SkyboxRendering
	{
	public:
		SkyboxRendering(RenderDeviceD3D12* device);
		
		void Render(Camera* camera);

	private:
		RenderDeviceD3D12* m_Device;

		std::unique_ptr<SkyboxPass> m_SkyboxPass;
		std::unique_ptr<VertexBufferD3D12> m_SphereVB;
		std::unique_ptr<IndexBufferD3D12> m_SphereIB;
	};
}