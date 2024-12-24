#pragma once
#include "framework.h"
#include "TextureD3D12.h"

namespace EduEngine
{
	class GRAPHICS_API GBuffer
	{
	public:
		GBuffer(int count, const DXGI_FORMAT* formats);

		void Resize(RenderDeviceD3D12* device, UINT width, UINT height);

		ID3D12Resource* GetGBuffer(int index) const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetGBufferRTVView(int index) const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetGBufferSRVView(int index) const;

		static constexpr int MaxGBufferCount = 8;
	private:
		int m_bufferCount;
		DXGI_FORMAT m_Formats[MaxGBufferCount];

		std::unique_ptr<TextureD3D12> m_GBuffers[MaxGBufferCount];
	};
}