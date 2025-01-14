#pragma once
#include "../Graphics/GBuffer.h"
#include "../Graphics/SwapChain.h"
#include "RenderObject.h"
#include "RenderPasses.h"
#include "Camera.h"
#include "CSMRendering.h"

namespace EduEngine
{
	class DeferredRendering
	{
	public:
		DeferredRendering(RenderDeviceD3D12* device, SwapChain* swapChain);

		void InitResources();

		void PrepareRenderGeometry(Camera* camera, const CSMRendering* csmRendering, D3D12_RECT* scissorRect);
		void RenderGeomerty(const RenderObject* renderObject);

		void RenderLights(Camera* camera, const std::vector<std::shared_ptr<Light>>& lights, D3D12_RECT* scissorRect);
		void RenderToneMapping(Camera* camera);

		GBuffer* GetGBuffer() const { return m_GBuffer.get(); }

	private:
		RenderDeviceD3D12* m_Device;
		SwapChain* m_SwapChain;

		std::unique_ptr<GBufferPass> m_GBufferPass;
		std::unique_ptr<DeferredLightPass> m_DeferredLightPass;
		std::unique_ptr<ToneMappingPass> m_ToneMappingPass;
		std::unique_ptr<GBuffer> m_GBuffer;
		std::unique_ptr<IndexBufferD3D12> m_QuadIndexBuff;
		std::unique_ptr<VertexBufferD3D12> m_QuadVertexBuff;

		std::unique_ptr<TextureHeapView> m_NullTex;
	};
}