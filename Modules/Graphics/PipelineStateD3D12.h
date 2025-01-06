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
		~PipelineStateD3D12();

		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology);
		void SetBlendState(D3D12_BLEND_DESC blendState);
		void SetRTBlendState(int rtIndex, D3D12_RENDER_TARGET_BLEND_DESC blendState);
		void SetInputLayout(D3D12_INPUT_LAYOUT_DESC inputLayout);
		void SetRTVFormat(const DXGI_FORMAT format);
		void SetRTVFormats(int count, const DXGI_FORMAT* formats);
		void SetRasterizerState(D3D12_RASTERIZER_DESC rasterizerDesc);
		void SetDepthStencilState(D3D12_DEPTH_STENCIL_DESC depthStencilDesc);
		void SetDepthStencilFormat(DXGI_FORMAT format);
		void SetRootSignature(RootSignatureD3D12* rootSignature);
		void SetRootSignature(ID3D12RootSignature* rootSignature);
		void SetShader(ShaderD3D12* shader);

		void Build(RenderDeviceD3D12* pDevice);

		void SetName(const wchar_t* name);

		ID3D12PipelineState* GetD3D12PipelineState() const;

	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_Desc;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PSO;

		RenderDeviceD3D12* m_Device;
	};
}

