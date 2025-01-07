#pragma once
#include "../Graphics/TextureD3D12.h"
#include "RenderObject.h"
#include "RenderPasses.h"
#include "RenderSettings.h"

namespace EduEngine
{
	using namespace DirectX;

	class CSMRendering
	{
	public:
		CSMRendering(RenderDeviceD3D12* device, const RenderSettings* settings, int count, const XMFLOAT2* sizes, const float* cascadeSplits);

		void Render(Camera* camera, Light* light, const std::vector<std::shared_ptr<RenderObject>>& renderObjects);

		int GetCascadeCount() const { return m_CascadeCount; }
		XMMATRIX GetCascadeTransform(int index) const { return m_CascadeTransforms[index]; }
		float GetCascadeDistance(int index) const { return m_CascadeSplits[index] * m_Settings->GetShadowDistance(); }
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return m_ShadowMaps[0]->GetView(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->GetGpuHandle(); }

		static constexpr int MaxCascades = 4;

	private:
		XMMATRIX CalculateLightView(Light* light);
		XMMATRIX CalculateCascadeProjection(float nearDist, float farDist, Camera* camera, XMMATRIX lightView);

	private:
		RenderDeviceD3D12* m_Device;
		const RenderSettings* m_Settings;

		int m_CascadeCount;
		float m_CascadeSplits[MaxCascades];
		XMMATRIX m_CascadeTransforms[MaxCascades];
		std::unique_ptr<TextureD3D12> m_ShadowMaps[MaxCascades];
		std::unique_ptr<ShadowMapPass> m_ShadowMapPass;
	};
}