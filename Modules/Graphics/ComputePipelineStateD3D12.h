#pragma once
#include "framework.h"
#include "ShaderD3D12.h"
#include "RootSignatureD3D12.h"

namespace EduEngine
{
	class GRAPHICS_API ComputePipelineStateD3D12
	{
	public:
		ComputePipelineStateD3D12();

		void SetRootSignature(RootSignatureD3D12* rootSignature);
		void SetRootSignature(ID3D12RootSignature* rootSignature);
		void SetShader(ShaderD3D12* shader);

		void Build(const RenderDeviceD3D12* pDevice);

		ID3D12PipelineState* GetD3D12PipelineState() const;

	private:
		D3D12_COMPUTE_PIPELINE_STATE_DESC m_Desc;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PSO;
	};
}