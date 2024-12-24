#include "pch.h"
#include "PipelineStateD3D12.h"

namespace EduEngine
{
	PipelineStateD3D12::PipelineStateD3D12()
	{
		ZeroMemory(&m_Desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

		m_Desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		m_Desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		m_Desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		m_Desc.SampleMask = UINT_MAX;
		m_Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		m_Desc.NumRenderTargets = 1;
		m_Desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_Desc.SampleDesc.Count = 1;
		m_Desc.SampleDesc.Quality = 0;
		m_Desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	}

	void PipelineStateD3D12::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology)
	{
		m_Desc.PrimitiveTopologyType = topology;
	}

	void PipelineStateD3D12::SetBlendState(D3D12_BLEND_DESC blendState)
	{
		m_Desc.BlendState = blendState;
	}

	void PipelineStateD3D12::SetInputLayout(D3D12_INPUT_LAYOUT_DESC inputLayout)
	{
		m_Desc.InputLayout = inputLayout;
	}

	void PipelineStateD3D12::SetRTVFormats(int count, const DXGI_FORMAT* formats)
	{
		m_Desc.NumRenderTargets = count;

		for (int i = 0; i < count; i++)
			m_Desc.RTVFormats[i] = formats[i];
	}

	void PipelineStateD3D12::SetRasterizerState(D3D12_RASTERIZER_DESC rasterizerDesc)
	{
		m_Desc.RasterizerState = rasterizerDesc;
	}

	void PipelineStateD3D12::SetDepthStencilState(D3D12_DEPTH_STENCIL_DESC depthStencilDesc)
	{
		m_Desc.DepthStencilState = depthStencilDesc;
	}

	void PipelineStateD3D12::SetDepthStencilFormat(DXGI_FORMAT format)
	{
		m_Desc.DSVFormat = format;
	}

	void PipelineStateD3D12::SetRootSignature(RootSignatureD3D12* rootSignature)
	{
		m_Desc.pRootSignature = rootSignature->GetD3D12RootSignature();
	}

	void PipelineStateD3D12::SetShader(ShaderD3D12* shader)
	{
		assert(shader != nullptr);

		switch (shader->GetShaderType())
		{
		case EDU_SHADER_TYPE_VERTEX:
			m_Desc.VS = shader->GetShaderBytecode();
			break;
		case EDU_SHADER_TYPE_GEOMETRY:
			m_Desc.GS = shader->GetShaderBytecode();
			break;
		case EDU_SHADER_TYPE_PIXEL:
			m_Desc.PS = shader->GetShaderBytecode();
			break;
		}

		assert(1);
	}

	void PipelineStateD3D12::Build(const RenderDeviceD3D12* pDevice)
	{
		HRESULT hr = pDevice->GetD3D12Device()->CreateGraphicsPipelineState(&m_Desc, IID_PPV_ARGS(&m_PSO));

		THROW_IF_FAILED(hr, L"Failed to create PSO");
	}

	ID3D12PipelineState* PipelineStateD3D12::GetD3D12PipelineState() const
	{
		return m_PSO.Get();
	}
}