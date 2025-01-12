#pragma once
#include "IRenderEngine.h"
#include "RenderPasses.h"
#include "../Graphics/BufferD3D12.h"
#include "TextureD3D12Impl.h"
#include "DeferredRendering.h"

namespace EduEngine
{
	class ParticleSystemD3D12 : public IParticleSystem
	{
	public:
		ParticleSystemD3D12(RenderDeviceD3D12* device, const RenderSettings* renderSettings);

		void Compute(const Timer& timer, D3D12_GPU_DESCRIPTOR_HANDLE normalTex, const SwapChain* swapChain);
		void Draw(Camera* camera, const Timer& timer, float aspectRatio);

		void SetMaxParticles(UINT num) override;
		void SetColorTexture(ITexture* texture) override;
		void SetScreenSpaceCollision(bool enabled) override;

		UINT GetMaxParticles() const;

	private:
		void InitBuffers(UINT particlesCount);
		void InitComputePass();

	private:
		RenderDeviceD3D12* m_Device;
		const RenderSettings* m_RenderSettings;
		Camera* m_LastRenderCamera;

		std::unique_ptr<ParticlesComputePass> m_ComputePass;
		std::unique_ptr<ParticlesDrawPass> m_DrawPass;

		std::unique_ptr<UploadBufferD3D12> m_DrawListUpload;
		std::unique_ptr<BufferD3D12> m_ParticlesPool;
		std::unique_ptr<BufferD3D12> m_DeadList;
		std::unique_ptr<BufferD3D12> m_DrawList;
		std::unique_ptr<BufferD3D12> m_DrawArgs;
		std::unique_ptr<BufferD3D12> m_DeadListCounter;
		TextureD3D12Impl* m_ColorTexture;

		bool m_AsyncCompute;
		bool m_ScreenSpaceCollision;
		bool m_DirtyBuffers;
		UINT m_MaxParticles;
		float m_Timer;

		struct Particle
		{
			DirectX::XMFLOAT4 Color;
			DirectX::XMFLOAT3 Position;
			float Age;
			DirectX::XMFLOAT3 Velocity;
			float Size;
			float Alive;
			DirectX::XMFLOAT3 Padding;
		};
	};
}