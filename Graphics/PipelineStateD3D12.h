#pragma once
#include "pch.h"
#include "ShaderD3D12.h"
#include "RootSignatureD3D12.h"

namespace EduEngine
{
	class GRAPHICS_API PipelineStateD3D12
	{
	public:
		PipelineStateD3D12();

		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology);
		void SetBlendState(D3D12_BLEND_DESC blendState);
		void SetInputLayout(D3D12_INPUT_LAYOUT_DESC inputLayout);
		void SetRootSignature(RootSignatureD3D12* rootSignature);
		void SetShader(ShaderD3D12* shader);

		void Build(const RenderDeviceD3D12* pDevice);

		ID3D12PipelineState* GetD3D12PipelineState() const;

	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_Desc;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PSO;
	};
}

